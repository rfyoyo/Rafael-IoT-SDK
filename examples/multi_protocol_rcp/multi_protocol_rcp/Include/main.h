#ifndef  __MAIN_H
#define  __MAIN_H

#include "cpc_system_common.h"

void cpc_uart_init(void);
void cpc_hci_init(void);
void cpc_sys_signal(void);
void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);
void cpc_upgrade_init(void);
void zigbee_app_init(void);
void zigbee_cli_init(void);

#endif // __DEMO_GPIO_H