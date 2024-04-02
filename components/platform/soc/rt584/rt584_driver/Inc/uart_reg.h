/**************************************************************************//**
 * @file     uart_reg.h
 * @version  V1.00
 * @brief    UART register definition header file
 *
 * @copyright
 *****************************************************************************/
#ifndef ___UART_REG_H__
#define ___UART_REG_H__


/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup  UART Controller
    Memory Mapped Structure for UART Controller
    RT584 uart is NOT UART 16550 compatible... it is new design.
@{ */

typedef struct {    
    __IO uint32_t RBR;              /*0x00, it is THR also*/
    __IO uint32_t IER;              /*0x04*/
    __IO uint32_t FCR;              /*0x08*/
    __IO uint32_t LCR;              /*0x0C*/
    __IO uint32_t MCR;              /*0x10*/
    __IO uint32_t LSR;              /*0x14*/
    __IO uint32_t MSR;              /*0x18*/
    __IO uint32_t ISR;              /*0x1C*/
    __IO uint32_t DLX;              /*0x20*/
    __IO uint32_t FDL;              /*0x24*/
    __IO uint32_t LSM;              /*0x28*/
    __IO uint32_t WAKE_SLEEP_EN;    /*0x2C*/
    __IO uint32_t UART_EN;          /*0x30*/
    __IO uint32_t xDMA_RX_ADDR;     /*0x34*/
    __IO uint32_t xDMA_RX_LEN;      /*0x38*/
    __IO uint32_t xDMA_TX_ADDR;     /*0x3C*/
    __IO uint32_t xDMA_TX_LEN;      /*0x40*/
    __I  uint32_t xDMA_RX_RLEN;     /*0x44*/
    __I  uint32_t xDMA_TX_RLEN;     /*0x48*/
    __IO uint32_t xDMA_RX_ENABLE;   /*0x4C*/
    __IO uint32_t xDMA_TX_ENABLE;   /*0x50*/
       
} UART_T;


#define THR RBR


/*LCR bit[1~0]  is character length */
#define LCR_WLEN5          0x00      /* Wordlength: 5 bits */
#define LCR_WLEN6          0x01      /* Wordlength: 6 bits */
#define LCR_WLEN7          0x02      /* Wordlength: 7 bits */
#define LCR_WLEN8          0x03      /* Wordlength: 8 bits */

#define LCR_STOP           0x04      /* Stop bits: 0=1 bit, 1=2 bits */

#define LCR_SBC            0x40      /* Set break control */
#define LCR_SPAR           0x20      /* Stick parity (?) */
#define LCR_EPAR           0x10      /* Even parity select */
#define LCR_PARITY         0x08      /* Parity Enable */


/*
 * These are the definitions for the FIFO Control Register
 */

#define FCR_CLEAR_RCVR      0x02     /* Clear the RCVR FIFO */
#define FCR_CLEAR_XMIT      0x04     /* Clear the XMIT FIFO */
#define FCR_DMA_SELECT      0x08     /* For DMA applications */

#define FCR_TRIGGER_MASK    0xC0     /* Mask for the FIFO trigger range */
#define FCR_TRIGGER_1       0x00     /* Mask for trigger set at 1 */
#define FCR_TRIGGER_4       0x40     /* Mask for trigger set at 4 */
#define FCR_TRIGGER_8       0x80     /* Mask for trigger set at 8 */
#define FCR_TRIGGER_14      0xC0     /* Mask for trigger set at 14 */

#define FCR_RTS_LEVEL_14       (0x3<<8)     /*RTS trigger level for RTS flow control*/

/* Clear & enable FIFOs */
#define FCR_DEFVAL     (  FCR_CLEAR_RCVR | FCR_CLEAR_XMIT)


/*
 * These are the definitions for the Modem Control Register
 */

#define MCR_RTS         0x02        /* RTS   */

#define UART_MCR_LOOP   0x10        /* Enable loopback test mode */

#define MCR_HW_AUTOFLOW_EN  0x20    /* RT584 Enable Hardware Auto Flow */

#define LSR_THRE        0x20        /* Transmit-hold-register empty */

#define LSR_TEMP        0x40        /* Transmitter empty*/

#define UART_MSR_CTS        0x10   /*Modem Status Register CTS state*/
#define UART_MSR_DCTS       0x01   /*Modem Status : Delta Clear To Send */

#define UART_ENABLE         0x01   /*Set 1 to start UART after all setting is ready*/
#define UART_DISABLE        0x00   

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI        0x08    /* Enable Modem status interrupt */
#define UART_IER_RLSI       0x04    /* Enable receiver line status interrupt */
#define UART_IER_THRI       0x02    /* Enable Transmitter holding register int. */
#define UART_IER_RDI        0x01    /* Enable receiver data interrupt */

#define UART_IER_RX_TIMEOUT   0x10  /* RT584 received data timeout*/
#define UART_IER_DMA_RX_INTR  0x20  /* RT584 Enable DMA RX Interrupt */
#define UART_IER_DMA_TX_INTR  0x40  /* RT584 Enable DMA TX Interrupt */


#define UART_LSR_TEMT       0x40 /* Transmitter empty */
#define UART_LSR_THRE       0x20 /* Transmit-hold-register empty */
#define UART_LSR_BI         0x10 /* Break interrupt indicator */
#define UART_LSR_FE         0x08 /* Frame error indicator */
#define UART_LSR_PE         0x04 /* Parity error indicator */
#define UART_LSR_OE         0x02 /* Overrun error indicator */
#define UART_LSR_DR         0x01 /* Receiver data ready */

#define UART_LSR_BRK_ERROR_BITS     0x1E /* BI, FE, PE, OE bits */

/*
 * ISR 
 */

#define ISR_RDA_INTR      0x01        /* ISR: Received Data Available */
#define ISR_THRE_INTR     0x02        /* ISR: THRE from 0 to 1      */
#define ISR_LSR_INTR      0x04        /* ISR: LSR status to 1 */
#define ISR_MSR_INTR      0x08        /* ISR: CTSn change */
#define ISR_RX_TIMEOUT    0x10        /* ISR: RX timeout for no input for 4 bytes long */
#define ISR_DMA_RX_INTR   0x20        /* ISR: DMA RX interrupt status */
#define ISR_DMA_TX_INTR   0x40        /* ISR: DMA TX interrupt status */


#define UART_RECEIVE_INTR   (ISR_RX_TIMEOUT|ISR_RDA_INTR)

#define xDMA_Enable         0x01   /*Enable xDMA*/
#define xDMA_Stop           0x00   /*Disable/Clear xDMA*/

/**@}*/ /* end of UART Controller */
/**@}*/ /* end of REGISTER group */


#endif
