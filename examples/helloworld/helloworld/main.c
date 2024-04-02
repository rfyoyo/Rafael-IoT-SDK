#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cm3_mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_uart.h"
#include "cli.h"

int main(void)
{
    printf("Hello World\r\n");
    cli_init();
    while (1) {
        vTaskDelay(200);
    }    
    return 0;
}
