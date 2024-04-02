#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openthread_port.h>
#if (CONFIG_CPC_ENABLE_ZIGBEE_NCP == 1) 
#include <zigbee_platform.h>
#endif
#include "cm3_mcu.h"
#include "hosal_rf.h"
#include "hosal_uart.h"
#include "lmac15p4.h"

#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "cli.h"
#include "log.h"

#define PHY_PIB_TURNAROUND_TIMER 192
#define PHY_PIB_CCA_DETECTED_TIME 128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE 0
#define PHY_PIB_CCA_THRESHOLD 75
#define MAC_PIB_UNIT_BACKOFF_PERIOD 320
#define MAC_PIB_MAC_ACK_WAIT_DURATION 544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE 8
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES 4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS 10
#define MAC_PIB_MAC_MIN_BE 3

static uint32_t s_reboot_flag = 0;
extern hosal_uart_dev_t cpc_uart_dev;
void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode)
{
    log_warn("Set watchdog reset!");
    hosal_uart_finalize(&cpc_uart_dev);
    s_reboot_flag = 1;
}

__STATIC_INLINE  void wdt_isr(void)
{
    if(s_reboot_flag == 0)
        Wdt_Kick();
}

static void init_wdt_init(void)
{
    wdt_config_mode_t wdt_mode;
    wdt_config_tick_t wdt_cfg_ticks;

    wdt_mode.int_enable = 1;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 0;
    wdt_mode.prescale = WDT_PRESCALE_32;

    wdt_cfg_ticks.wdt_ticks = 1200 * 1000;
    wdt_cfg_ticks.int_ticks = 200 * 1000;
    wdt_cfg_ticks.wdt_min_ticks = 0;

    Wdt_Start(wdt_mode, wdt_cfg_ticks, wdt_isr);

    NVIC_SetPriority(Wdt_IRQn, 0x1);
}

void set_pta_grant_pin(uint8_t gpio)
{
    uint32_t pta_in_reg = (uint32_t)gpio | (1UL << 7);

    pin_set_mode(gpio, MODE_GPIO);

    outp32(0x40800058, ((inp32(0x40800058) & 0xFFFFFF00) | pta_in_reg));
    outp32(0x40800010, ((inp32(0x40800010) & 0xFFFFFFFF) | 0x77000000));
    outp32(0x4080003C, ((inp32(0x4080003C) | 0x07000000)));
}

int main(void)
{
    hosal_rf_pta_ctrl_t pta_ctrl;
    hosal_rf_modem_t modem_type;
    hosal_rf_tx_power_ch_comp_t tx_pwr_ch_comp_ctrl;
    hosal_rf_tx_power_comp_seg_t tx_pwr_comp_seg_ctrl;

    init_wdt_init();
    log_init();
    cli_init();

    hosal_rf_init(HOSAL_RF_MODE_MULTI_PROTOCOL);

    cpc_uart_init();
    cpc_hci_init();
    cpc_upgrade_init();

    lmac15p4_init(LMAC15P4_2P4G_OQPSK);

    pta_ctrl.enable = 0;
    pta_ctrl.inverse = 1;
    set_pta_grant_pin(8);

    hosal_rf_ioctl(HOSAL_RF_IOCTL_PTA_CTRL_SET, (void*) &pta_ctrl);

    /* PHY PIB */
    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE, PHY_PIB_CCA_THRESHOLD, PHY_PIB_CCA_DETECTED_TIME);
    /* MAC PIB */
    lmac15p4_mac_pib_set(MAC_PIB_UNIT_BACKOFF_PERIOD, MAC_PIB_MAC_ACK_WAIT_DURATION, 
                       MAC_PIB_MAC_MAX_BE, 
                       MAC_PIB_MAC_MAX_CSMACA_BACKOFFS, 
                       MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME, MAC_PIB_MAC_MAX_FRAME_RETRIES, 
                       MAC_PIB_MAC_MIN_BE);

#if (CONFIG_TX_POWER_SET == 1)
    // BLE
    modem_type = HOSAL_RF_MODEM_BLE;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 28;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 25;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 19;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 35;
    tx_pwr_comp_seg_ctrl.segmentB = 38;
    tx_pwr_comp_seg_ctrl.segmentC = 39;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);

    // ZIGBEE
    modem_type = HOSAL_RF_MODEM_2P4G_OQPSK;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_COMP_SET, (void*) &modem_type);

    tx_pwr_ch_comp_ctrl.tx_power_stage0 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage1 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage2 = 31;
    tx_pwr_ch_comp_ctrl.tx_power_stage3 = 10;
    tx_pwr_ch_comp_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET, (void*) &tx_pwr_ch_comp_ctrl);
    
    // 0~segmentA-1, segmentA~segmentB-1, segmentB~segmentC-1, segmentC~39
    // 0=2402MHz ~ 39=2480MHz
    tx_pwr_comp_seg_ctrl.segmentA = 13;
    tx_pwr_comp_seg_ctrl.segmentB = 26;
    tx_pwr_comp_seg_ctrl.segmentC = 34;
    tx_pwr_comp_seg_ctrl.modem_type = modem_type;
    hosal_rf_ioctl(HOSAL_RF_IOCTL_COMP_SEG_SET, (void *) &tx_pwr_comp_seg_ctrl);
#endif

    otrStart();
#if (CONFIG_CPC_ENABLE_ZIGBEE_NCP == 1) 
    zbStart();
#endif
    return 0;
}
void otrInitUser(otInstance * instance)
{
    otAppNcpInit((otInstance * )instance);
}
