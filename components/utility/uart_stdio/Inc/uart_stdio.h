/**
 * @file uart_stdio.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __UART_STDIO__
#define __UART_STDIO__

#ifdef __cplusplus
extern "C" {
#endif


int uart_stdio_init(void);
int uart_stdio_write(uint8_t *p_data, uint32_t length);
int uart_stdio_read(uint8_t *p_data, uint32_t length);


/** @} */
#endif
#ifdef __cplusplus
}
#endif
