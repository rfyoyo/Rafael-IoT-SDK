/**************************************************************************
 *
 * printf is usually used for ouput debug message. 
 * In most case, we only output debug message in non-secure world.
 * So this retarget_drv.c should only runs in non-secure world.
 *
 * But in early develop stage, maybe we need to printf debug message
 * in secure world, too. For this reason, we add one "secure version" retarget_drv_sec.c
 * That is output function handle in secure world.
 * For non-secure world, the output message will call secure world API to
 * output the non-secure message. All input message (getchar) also send to non-secure world.
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "rt584_cm33.h"

#include "uart_drv.h"

#include "sysfun.h"
#include "ringbuffer.h"

#define txbufsize              512

#define rxbufsize              128

#define txbuf_mask            (txbufsize -1)

#define rxbuf_mask            (rxbufsize -1)

#define  STATE_SEND_IDLE            0
#define  STATE_SEND_BUSY            1

ring_buf_t         sendbuf;
//static ring_buf_t         sendbuf;
static volatile uint8_t   send_state;
static uint16_t           send_num;

ring_buf_t         recvbuf;
//static ring_buf_t         recvbuf;
static uint8_t            tempbuffer[4];

static void printf_callback(uint32_t event, void *p_context)
{
    uint16_t   wr_index, rd_index;    
    uint16_t   data_in_fifo;
     
    /*Except UART_EVENT_TX_DONE, this callback function does NOT care any other event*/
    /*This is ISR, so it is already in priority mode*/
 
    if (event & UART_EVENT_TX_DONE) {
                       
        sendbuf.rd_idx += send_num;
        send_num = 0;
        
        /*check is there data in buffer need to be send?*/
        
        rd_index = sendbuf.rd_idx & txbuf_mask;
        wr_index = sendbuf.wr_idx & txbuf_mask;
        
        if (sendbuf.rd_idx == sendbuf.wr_idx) {
            /* buffer empty ---
               except ISR printf, sendbuf.wr_idx will not changed */
            send_state = STATE_SEND_IDLE;
        } else {
            if (rd_index <= wr_index) { 
                /* if fact, rd_index will be less than wr_index
                 * equal is empty
                 */
                send_num = wr_index - rd_index; 
            } else {
                send_num = txbufsize - rd_index;
            }

            uart_tx(0, &(sendbuf.ring_buf[rd_index]), (uint32_t) send_num);
        }
    }
    
    if (event & UART_EVENT_RX_DONE) {
        
        data_in_fifo = (recvbuf.wr_idx - recvbuf.rd_idx) & recvbuf.bufsize_mask;
        
        if (data_in_fifo < rxbuf_mask) 
        {            
            recvbuf.ring_buf[(recvbuf.wr_idx & recvbuf.bufsize_mask)] = tempbuffer[0]; 
            recvbuf.wr_idx++;      /*so now wr_idex will pointer to next free bytes*/
        }
        else
        {
            /*drop this input buffer... because no space for input*/
            /*recv buffer full.. so we need to drop data --- 
              if RT570 care this data, it should get data from recvbuffer ASAP
              or more buffer for received buffer. default is 128 bytes.*/
        }
        
        uart_rx(0, tempbuffer , 1);
    }
                
    return ;
}

/**
  Initialize stdout
 
  \return          STATUS_SUCCESS on success, or  error.
*/
uint32_t console_drv_init(uart_baudrate_t baudrate) {
    
    uart_config_t   debug_console_drv_config;
    uint32_t        status;
    static uint8_t  xsend_buf[txbufsize];    
    static uint8_t  xrecv_buf[rxbufsize];
    
    
    /*init ringbuf for TX*/    
    sendbuf.ring_buf = xsend_buf;
    sendbuf.bufsize_mask = txbuf_mask;
    sendbuf.wr_idx = sendbuf.rd_idx = 0;
    send_state = STATE_SEND_IDLE;
    
    send_num = 0;
        
    recvbuf.ring_buf = xrecv_buf;
    recvbuf.bufsize_mask = rxbuf_mask;
    recvbuf.wr_idx = recvbuf.rd_idx = 0;
           
    /*init debug console uart0, 8bits 1 stopbit, none parity, no flow control.*/
    debug_console_drv_config.baudrate = baudrate;
    debug_console_drv_config.databits = UART_DATA_BITS_8;
    debug_console_drv_config.hwfc     = UART_HWFC_DISABLED;
    debug_console_drv_config.parity   = UART_PARITY_NONE;
    
    /* Important: p_contex will be the second parameter in uart callback.
     * In this example, we do NOT use p_context, (So we just use NULL)
     * but you can use it for whatever you want.
     */
    debug_console_drv_config.p_context = (void *) NULL;
    
    debug_console_drv_config.stopbit  = UART_STOPBIT_ONE;
    debug_console_drv_config.interrupt_priority = IRQ_PRIORITY_LOW;
    
    status = uart_init(0, &debug_console_drv_config, printf_callback);
    
    if (status != STATUS_SUCCESS)
    {   /*almost impossible for this error...*/        
        return  status;
    }
    
    /*uart device is auto power on in uart_init function */  
    uart_rx(0, tempbuffer , 1);

    return (STATUS_SUCCESS);
}
 
static void uart_send(int ch)
{
    uint16_t   data_in_fifo, free_space_in_fifo;

    uint16_t   rd_index;
    
    /* this uart_send has two version:
     * 1. for only single thread/FSM version: printf is only called by one task at any time.
     * 2. there are multiple tasks version: printf maybe called for any thread at the same time.
     */
    
wait_free_space:

    enter_critical_section();

    data_in_fifo = (sendbuf.wr_idx - sendbuf.rd_idx) & txbuf_mask;
    free_space_in_fifo = txbuf_mask - data_in_fifo;
    
    if (free_space_in_fifo) {
        sendbuf.ring_buf[(sendbuf.wr_idx & txbuf_mask)] = (uint8_t) ch;
        sendbuf.wr_idx++;       
    } else {
        /*no space for printf buffer, relese critical section first*/
       
        leave_critical_section();
             
        if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) {
            /*running in interrupt mode*/                     
            return ;        /*no space left, so drop*/        
        } else {
            /*For multi-tasking environment, it can release CPU here*/
            goto wait_free_space;
        }
    }

    /* check current TX state --- 
     * in multiple task environment, this function is protected by disable interrupt.
     */
    if (send_state==STATE_SEND_IDLE) {
        /*check rd position, wr position*/

        rd_index = sendbuf.rd_idx & txbuf_mask;
        send_num = 1;

        send_state = STATE_SEND_BUSY;

        leave_critical_section();
                          
        uart_tx(0, &(sendbuf.ring_buf[rd_index]), (uint32_t) send_num);
    } else {
        /*uart0 is busy in transmit, so data will send next interrupt done.*/
        leave_critical_section();
    }

    return;
}

/*This function is for sleep function*/
void console_sleep(void)
{
    enter_critical_section();

    uart_tx_abort(0);
    /*reinit console.*/
    sendbuf.wr_idx = sendbuf.rd_idx = 0;
    send_state = STATE_SEND_IDLE;
    send_num = 0;
      
    leave_critical_section();
}

uint32_t check_uart0_rx_buffer_data_num(void)
{
    /*
     * return "current number of character" in rx buffer, it could be changed anytime
     * But in most time, application just want to know is there data in rx buffer?
     */
    return ((recvbuf.wr_idx-recvbuf.rd_idx) & recvbuf.bufsize_mask);
}



#if defined(__ARMCC_VERSION)
 
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int fputc(int ch, FILE *p_file) {    

    uart_send(ch);
    
    if (ch == 0x0A) { 
        /*patch for "\n"*/
        uart_send(0x0D);
    }

    return (ch);
}


int fgetc(FILE *p_file)
{
    uint8_t input;
    
    /*
     * Remak: This function is designed for single thread 
     * It does NOT consider multiple threads race condition issue.    
     */

    while (recvbuf.wr_idx == recvbuf.rd_idx) 
        ;       /*block to until host send data*/
    
    input = recvbuf.ring_buf[(recvbuf.rd_idx&recvbuf.bufsize_mask)];
    recvbuf.rd_idx++;
    
    return input;
}

#elif defined(__GNUC__)

int _write(int fd, char *ptr, int len)
{
    int  i = len;
    
    while (i>0) {
        
        uart_send(*ptr);
        
        if (*ptr == 0x0A) {
            uart_send(0x0D);
        }
        
        ptr++;
        i--;        
    }
    
    return len;
}


int _read(int fd, char *ptr, int len)
{

    while (recvbuf.wr_idx == recvbuf.rd_idx) 
        ;       /*block to until host send data*/
    
    *ptr = recvbuf.ring_buf[(recvbuf.rd_idx&recvbuf.bufsize_mask)];
    recvbuf.rd_idx++;
    
    return 1;
}

#endif


#if 0

extern void UART0_Handler(void);

/**
 * @brief       Hard fault handler in C.
 * 
 */
void hard_fault_handler_c(uint32_t *stack)
{   

    /*wait uart console port finish to send */
    /*
     * current in hard fault handler ISR... so uart enterrupt can not  be serviced...
     * So we should service uart interrrupt 
     */
              
    /*if uart TX is running...*/
    while (UART0->xDMA_TX_ENABLE)
    {
        if ((UART0->xDMA_INT_STATUS & xDMA_ISR_TX) == xDMA_ISR_TX) 
        {
            UART0_Handler();               
        }
            
    }
      
    printf("In Hard Fault Handler\n");

    printf("R0  = 0x%x\n", stack[0]);
    printf("R1  = 0x%x\n", stack[1]);
    printf("R2  = 0x%x\n", stack[2]);
    printf("R3  = 0x%x\n", stack[3]);
    printf("R12 = 0x%x\n", stack[4]);
    printf("LR  = 0x%x\n", stack[5]);
    printf("PC  = 0x%x\n", stack[6]);
    printf("PSR = 0x%x\n", stack[7]);
                    
    /*if uart TX is running...*/
    while (UART0->xDMA_TX_ENABLE)
    {
        if ((UART0->xDMA_INT_STATUS & xDMA_ISR_TX) == xDMA_ISR_TX) 
        {
            UART0_Handler();                
        }
            
    }

    while(1);
    
}

/* SCB->AIRCR BFHFNMINS default is 0, so HardFault is
 * secure mode only... so the following code is ???? secure fault?
 *
 */
void HardFault_Handler(void)
{    
    __ASM volatile(
        
#if !defined(__ICCARM__)
         ".syntax unified       \n"
#endif    
         "tst      lr,   #4     \n"
         "ite      eq           \n"
         "mrseq    r0,   msp    \n"
         "mrsne    r0,   psp    \n"
         "b        hard_fault_handler_c     \n"
        );
        
}

#endif

