#ifndef  __MAIN_H
#define  __MAIN_H

#include "cpc_system_common.h"
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/icmp6.h>
#include <openthread/cli.h>
#include <openthread/ncp.h>
#include <openthread/coap.h>
#include <openthread_port.h>


void cpc_uart_init(void);
void cpc_hci_init(void);
void cpc_sys_signal(void);
void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);
void cpc_upgrade_init(void);
void zigbee_app_init(void);
void zigbee_cli_init(void);

#endif // __DEMO_GPIO_H