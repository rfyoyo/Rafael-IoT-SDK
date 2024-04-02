/**
 * @file hosal_rf.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "stdlib.h"

typedef int (*hosal_rf_callback_t)(void *p_arg);

/**
 * @brief 
 *
 */
typedef enum
{
    HOSAL_RF_MODE_RUCI_CMD = 0x1,
    HOSAL_RF_MODE_BLE_CONTROLLER = 0x2,
    HOSAL_RF_MODE_MULTI_PROTOCOL = 0x10,
} hosal_rf_mode_t;

typedef enum
{
    HOSAL_RF_STATUS_SUCCESS = 0,
    HOSAL_RF_STATUS_INVALID_PARAMETER,
    HOSAL_RF_STATUS_NO_MEMORY,
    HOSAL_RF_STATUS_BUSY,
    HOSAL_RF_STATUS_INVALID_STATE,
    HOSAL_RF_STATUS_INVALID_CMD,
    HOSAL_RF_STATUS_RX_BUSY,
    HOSAL_RF_STATUS_TX_BUSY,
    HOSAL_RF_STATUS_UNSUPPORT_CMD,
    HOSAL_RF_STATUS_NOT_AVAILABLE,
    HOSAL_RF_STATUS_CONTENT_ERROR
} hosal_rf_status_t;

typedef enum
{
    HOSAL_RF_IOCTL_MODEM_SET = 0,
    HOSAL_RF_IOCTL_MODEM_GET,
    HOSAL_RF_IOCTL_FREQUENCY_SET,
    HOSAL_RF_IOCTL_FREQUENCY_GET,
    HOSAL_RF_IOCTL_TX_CONTINOUS_WAVE_SET,
    HOSAL_RF_IOCTL_TX_START_SET,
    HOSAL_RF_IOCTL_TX_PWR_COMP_SET,
    HOSAL_RF_IOCTL_TX_PWR_CH_COMP_SET,
    HOSAL_RF_IOCTL_COMP_SEG_SET,
    HOSAL_RF_IOCTL_RSSI_GET,
    HOSAL_RF_IOCTL_KEY_SET,
    HOSAL_RF_IOCTL_SLEEP_SET,
    HOSAL_RF_IOCTL_IDLE_SET,
    HOSAL_RF_IOCTL_PTA_CTRL_SET,
    HOSAL_RF_IOCTL_15P4_MAC_PIB_SET,
    HOSAL_RF_IOCTL_15P4_PHY_PIB_SET,
    HOSAL_RF_IOCTL_15P4_ADDRESS_FILTER_SET,
    HOSAL_RF_IOCTL_15P4_AUTO_ACK_SET,
    HOSAL_RF_IOCTL_15P4_AUTO_STATE_SET,
    HOSAL_RF_IOCTL_15P4_ACK_PENDING_BIT_SET,
    HOSAL_RF_IOCTL_15P4_ACK_PACKET_GET,
    HOSAL_RF_IOCTL_15P4_SOURCE_ADDRESS_MATCH_SET,
    HOSAL_RF_IOCTL_15P4_SOURCE_ADDRESS_SHORT_CONTROL_SET,
    HOSAL_RF_IOCTL_15P4_SOURCE_ADDRESS_EXTEND_CONTROL_SET,
    HOSAL_RF_IOCTL_15P4_CSL_UNCERTAINTY_GET,
    HOSAL_RF_IOCTL_15P4_CSL_ACCURACY_GET,
    HOSAL_RF_IOCTL_15P4_CSL_SAMPLE_TIME_UPDATE_SET,
    HOSAL_RF_IOCTL_15P4_CSL_RECEIVER_CTRL_SET,

    HOSAL_RF_IOCTL_15P4_OPERATION_PAN_IDX_SET,

    HOSAL_RF_IOCTL_SUBG_OQPSK_DATA_RATE_SET,
    HOSAL_RF_IOCTL_SUBG_OQPSK_MAC_SET,
    HOSAL_RF_IOCTL_SUBG_OQPSK_PREAMBLE_SET,

    HOSAL_RF_IOCTL_SUBG_FSK_MODEM_CONFIG_SET,
    HOSAL_RF_IOCTL_SUBG_FSK_MAC_SET,
    HOSAL_RF_IOCTL_SUBG_FSK_PREAMBLE_SET,
    HOSAL_RF_IOCTL_SUBG_FSK_SFD_SET,
    HOSAL_RF_IOCTL_SUBG_FSK_FILTER_SET,

} hosal_rf_ioctl_t;

typedef enum
{
    HOSAL_RF_MODEM_FSK                  = 1,
    HOSAL_RF_MODEM_2P4G_OQPSK           = 2,
    HOSAL_RF_MODEM_BLE                  = 3,
    HOSAL_RF_MODEM_SUBG_OQPSK           = 4,
} hosal_rf_modem_t;

typedef enum 
{
    HOSAL_RF_TX_POWER_LVL_20DBM     = 0,
    HOSAL_RF_TX_POWER_LVL_14DBM     = 1,
    HOSAL_RF_TX_POWER_LVL_0BM       = 2,
} hosal_rf_tx_power_level_t;

typedef enum 
{ 
    HOSAL_RF_PHY_DATA_RATE_2M     = 0,
    HOSAL_RF_PHY_DATA_RATE_1M,
    HOSAL_RF_PHY_DATA_RATE_500K,
    HOSAL_RF_PHY_DATA_RATE_200K,
    HOSAL_RF_PHY_DATA_RATE_100K,
    HOSAL_RF_PHY_DATA_RATE_50K,
    HOSAL_RF_PHY_DATA_RATE_300K,
    HOSAL_RF_PHY_DATA_RATE_150K,
    HOSAL_RF_PHY_DATA_RATE_75K,
} hosal_rf_phy_data_rate_t;

typedef struct 
{
    uint32_t backoff_period;
    uint32_t ack_wait_duration;
    uint32_t max_frame_total_wait_time;
    uint8_t max_BE;
    uint8_t min_BE;
    uint8_t max_CSMA_backoffs;
    uint8_t max_frame_retries;
} hosal_rf_15p4_mac_pib_t;

typedef struct 
{
    uint16_t turnaround_time;
    uint16_t cca_duration;
    uint8_t cca_mode;
    uint8_t cca_threshold;
} hosal_rf_15p4_phy_pib_t;

typedef struct 
{
    uint32_t promiscuous;
    uint32_t is_coordinator;
    uint32_t long_address_0;
    uint32_t long_address_1;
    uint16_t short_address;
    uint16_t panid;
} hosal_rf_15p4_address_filter_t;

typedef struct 
{
    uint8_t *pdata;
    uint8_t *ptime;
} hosal_rf_15p4_ack_packet_t;


typedef struct 
{
    uint32_t control_type;
    uint8_t *addr;
} hosal_rf_15p4_src_match_t;

typedef struct
{
    uint16_t data_len;
    uint8_t control;
    uint8_t dsn;
    uint8_t *pData;
} hosal_rf_tx_data_t;

typedef struct 
{
    uint8_t   crc_type;
    uint8_t   whiten_enable;
} hosal_rf_mac_set_t;

typedef struct 
{
    uint8_t   data_rate;
    uint8_t   modulation_index;
} hosal_rf_modem_config_set_t;

typedef struct 
{
    uint8_t enable;
    uint8_t inverse;
} hosal_rf_pta_ctrl_t;

typedef struct
{
    int8_t tx_power_stage0;
    int8_t tx_power_stage1;
    int8_t tx_power_stage2;
    int8_t tx_power_stage3;
    hosal_rf_modem_t modem_type;
} hosal_rf_tx_power_ch_comp_t;

typedef struct
{
    uint8_t segmentA;
    uint8_t segmentB;
    uint8_t segmentC;
    hosal_rf_modem_t modem_type;
} hosal_rf_tx_power_comp_seg_t;

#define HOSAL_RF_BLE_EVENT_CALLBACK         1
#define HOSAL_RF_BLE_RX_CALLBACK            2
#define HOSAL_RF_PCI_EVENT_CALLBACK         3
#define HOSAL_RF_PCI_RX_CALLBACK            4
#define HOSAL_RF_PCI_TX_CALLBACK            5


int hosal_rf_wrire_command(uint8_t *command_ptr, uint32_t command_len);
int hosal_rf_write_tx_data(uint8_t *tx_data_ptr, uint32_t tx_data_len);
uint32_t hosal_rf_read_event(uint8_t *event_data_ptr);
uint32_t hosal_rf_read_rx_data(uint8_t *rx_data_ptr);
void hosal_rf_init(hosal_rf_mode_t mode);
hosal_rf_status_t hosal_rf_ioctl(hosal_rf_ioctl_t ctl, void *p_arg);
int hosal_rf_callback_set(int callback_type, hosal_rf_callback_t pfn_callback, void *arg);