/**************************************************************************//**
 * @file     crypto.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/

#include "project_config.h"

#include "cm3_mcu.h"

#include "rt_crypto.h"

#include <stdio.h>
#include <string.h>


//static crypto_isr_handler_t   user_crypto_isr = NULL;

#define   SB_MASK                                0x1F
#define   SB_MASK_SHIFT                             8

#define   START_SHIFT                              16

#define   REG_STATUS_MASK                   ~(0x1FFF)

#define AES_Enc_inst_index                       (16)
#define AES_Dec_inst_index                       (64)


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

static TaskHandle_t       wait_notify_task;

void wait_crypto_operation_finish(uint8_t op_num, uint8_t sb_num);

#define CRYPTO_ISR_FINISH            1

#endif

/*start crypto function*/
void crypto_start(uint8_t op_num, uint8_t sb_num)
{

    /*
     * We should do this optimize,
     * bceause gcc will expand this function several times... waste flash memory.
     */
    uint32_t reg;

    reg =  inp32(CRYPTO_BASE);
    reg = (reg & REG_STATUS_MASK) | op_num | ((sb_num & SB_MASK) << SB_MASK_SHIFT) | (1 << START_SHIFT) ;
    outp32(CRYPTO_BASE, reg);

}

/* ------ Point operations ------ */

#if MODULE_ENABLE(SUPPORT_MULTITASKING)
/*If system support multi-tasking, we should protect hardware for only one context.*/

void crypto_mutex_lock(void)
{
    /* TODO:  This is OS-depended code
     * if device is busy, the task should wait here to get resource.
     */

    return ;
}

void crypto_mutex_unlock(void)
{
    /* TODO: This is OS-depended code
     * If there is another device wait resource, it could be wakeup the
     * task depends on priority.
     */

    return ;
}


#else
/*
 * For single thread, we can igonre mutex protect.
 */
#define crypto_mutex_lock()          ((void)0)
#define crypto_mutex_unlock()        ((void)0)

#endif



#if MODULE_ENABLE(CRYPTO_AES_ENABLE)

const uint32_t aes_cipher_bin[] =
{
    0x3302A350, 0x10100140, 0x10BA0951, 0x3018013C,
    0x30180900, 0x50080000, 0x504A0080, 0x508C0100,
    0x50CE0180, 0x50000014, 0x50400094, 0x50800114,
    0x50C00194, 0x00000000, 0x68500054, 0x68A00054,
    0x68F00054, 0x470546E0, 0x5000000C, 0x5040008C,
    0x5080010C, 0x50C0018C, 0x900040D0, 0x12140004,
    0x30180940, 0x50080000, 0x504A0080, 0x508C0100,
    0x50CE0180, 0x43EC46E1, 0x311A7800, 0x141AA753,
    0xA0008006
};

const uint32_t aes_cipher_inv_bin[] =
{
    0x10D2A144, 0x1416AB55, 0x3018013C, 0x1297FFFF,
    0x400044D0, 0x1297FFFF, 0x400044D0, 0x1297FFFF,
    0x400044D0, 0x1297FFFF, 0x400044D0, 0x30180940,
    0x50080000, 0x504A0080, 0x508C0100, 0x50CE0180,
    0x50000018, 0x50400098, 0x50800118, 0x50C00198,
    0x00000000, 0x6850007E, 0x68A0007E, 0x68F0007E,
    0x10828800, 0x1297FFFF, 0x400044D0, 0x1297FFFF,
    0x400044D0, 0x1297FFFF, 0x400044D0, 0x1297FFFF,
    0x400044D0, 0x30180940, 0x50080000, 0x504A0080,
    0x508C0100, 0x50CE0180, 0x470546E0, 0x50000010,
    0x50400090, 0x50800110, 0x50C00190, 0x43E546E1,
    0x311A7800, 0xF0000000
};

const uint32_t aes_enc_boot_bin[] =
{
    0x10C00600, 0x181AA753, 0xA0000010, 0xF0000000
};

const uint32_t aes_cbc_enc_bin[] =
{
    0x1022A400, 0x140AA954, 0x10967944, 0x900040AD,
    0x10C0FE00, 0x181AA753, 0xA0000010, 0x10B2893C,
    0x331A893C, 0x900020C0, 0x12400800, 0x43F84191,
    0xF0000000
};

const uint32_t aes_ctr_bin[] =
{
    0x1022A400, 0x140AA954, 0x10967944, 0x900040D0,
    0x10C14E00, 0x181AA753, 0xA0000010, 0x10B2893C,
    0x900020AC, 0x12400800, 0x107AAD47, 0x440306EB,
    0x107AAF47, 0x400006EB, 0x43F54191, 0xF0000000
};

void aes_fw_load(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x440)); /*AES_Enc_ADDRESS*/
    memcpy(target_addr, aes_cipher_bin, sizeof(aes_cipher_bin));

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x500)); /*AES_Dec_ADDRESS*/
    memcpy(target_addr, aes_cipher_inv_bin, sizeof(aes_cipher_inv_bin));

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x400));
    memcpy(target_addr, aes_enc_boot_bin, sizeof(aes_enc_boot_bin));

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x5E0));
    memcpy(target_addr, aes_cbc_enc_bin, sizeof(aes_cbc_enc_bin));

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x680));
    memcpy(target_addr, aes_ctr_bin, sizeof(aes_ctr_bin));

}

void aes_encryption(uint8_t round, uint8_t *in_buf, uint8_t *out_buf)
{

    crypto_copy((uint32_t *)(CRYPTO_BASE + 0xEF0), (uint32_t *)in_buf, 4);

#if defined(__GNUC__)
    /*
     * GCC memcpy has some re-order issue.
     * So we use a stupid one-by-one copy here.
     */
    crypto_copy((uint32_t *)(CRYPTO_BASE + 0x400), (uint32_t *)aes_enc_boot_bin, (sizeof(aes_enc_boot_bin) >> 2));
#else
    memcpy( ((uint32_t *) (CRYPTO_BASE + 0x400)), aes_enc_boot_bin, sizeof(aes_enc_boot_bin));
#endif

    //set (round-1) for (round) loops
    //no mixcolumn for last round
    *((volatile uint32_t *) (CRYPTO_BASE + 0xF40))     = round - 1;


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(3, 0);

#else

    //Start the acceleator engine
    crypto_start(3, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy((uint32_t *)out_buf, (uint32_t *)(CRYPTO_BASE + 0xEF0), 4);

}

void aes_decryption(uint8_t round, uint8_t *in_buf, uint8_t *out_buf)
{
    crypto_copy((uint32_t *)(CRYPTO_BASE + 0xEF0), (uint32_t *)in_buf, 4);
    //set (round-1) for (round) loops
    //no mixcolumn for last round
    *((volatile uint32_t *) (CRYPTO_BASE + 0xF40))     = round - 1;
    *((volatile uint32_t *) (CRYPTO_BASE + 0xF54) )    = 0x100 + (round + 1) * 4;

    *((volatile uint32_t *) (CRYPTO_BASE + 0x400)) = 0xa0000000 | AES_Dec_inst_index;

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(3, 0);

#else

    //Start the acceleator engine
    crypto_start(3, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy((uint32_t *)out_buf, (uint32_t *)(CRYPTO_BASE + 0xEF0), 4);

}

#endif

#if MODULE_ENABLE(CRYPTO_SECT163R2_ENABLE)

uint32_t gf2m_ecc_curve_b163_init(void)
{
    //curve B163
    uint32_t polynomial[] = { 0x000000c9, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000008 };

    crypto_mutex_lock();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C00), polynomial, secp192r1_op_num);

    return STATUS_SUCCESS;
}

const uint32_t gf2m_inv2_b163_data[] =
{
    0x332A0C00, 0x00000000, 0x35024A02, 0x35021802,
    0x35221A00, 0x352A2400, 0x102A0106, 0x10721912,
    0x10BA4A00, 0x9000009A, 0xA70000F0, 0x4705309D,
    0x470930AD, 0x9500009A, 0x430C0843, 0xA00000E9,
    0x96001090, 0x470230BD, 0x900030BE, 0x960030B0,
    0xA00000D1, 0x960020A0, 0x470230CD, 0x900040CE,
    0x960040C0, 0xA00000D1, 0x9000109A, 0x96001090,
    0x900030BC, 0x470230BD, 0x900030BE, 0x960030B0,
    0xA00000D1, 0x9000209A, 0x960020A0, 0x900040BC,
    0x470230CD, 0x900040CE, 0x960040C0, 0xA00000D1,
    0x1418542A, 0xA0008006, 0x00000000
};


static void gf2m_inv2_b163(void)
{

    memcpy( (uint32_t *) (CRYPTO_BASE + 0x720), gf2m_inv2_b163_data, sizeof(gf2m_inv2_b163_data));

}


const uint32_t gf2m_point_double_b163_bin[] =
{
    0x30280000, 0x1028189F, 0x90000090, 0xA70000BB,
    0x00000000, 0x00000000, 0x332A000C, 0x1808542A,
    0xA00000C8, 0x102A1812, 0x10701824, 0x10BA311E,
    0x93064092, 0x102A1812, 0x10BA311E, 0x900040CB,
    0x332A300C, 0x930620BD, 0x10701824, 0x10BA311E,
    0x332A1824, 0x332A3024, 0x930630DC, 0x3328490C,
    0x10701824, 0x10BA311E, 0x40006CC2, 0x900030BC,
    0x332A300C, 0x930610CB, 0x33281918, 0x10701824,
    0x10BA311E, 0x900020A9, 0x00000000, 0x00000000,
    0xA00000BC, 0x35282400, 0x1418562B, 0xA0008006,
    0x00000000
};


static void gf2m_point_double_b163(void)
{
    memcpy((uint32_t *) (CRYPTO_BASE + 0x0658), gf2m_point_double_b163_bin, sizeof(gf2m_point_double_b163_bin));
}

const uint32_t gf2m_point_add_b163_bin[] =
{
    0x30280000, 0x10281812, 0x1070301E, 0x10BA311E,
    0x900000B0, 0xA8000058, 0x900000C0, 0xA700008C,
    0x90000090, 0xA800005C, 0x900000A0, 0xA7000089,
    0x9000009B, 0xA8000064, 0x900000AC, 0xA7000086,
    0x35281800, 0x35282400, 0xA000008C, 0x00000000,
    0x900050AC, 0x9000609B, 0x332A011E, 0x1008006A,
    0x1808542A, 0xA00000C8, 0x332A0118, 0x332A0D1E,
    0x102A1900, 0x1072311E, 0x10BA2424, 0x9306309A,
    0x102A0106, 0x10BA2424, 0x332A1918, 0x332A2518,
    0x930640BD, 0x332A310C, 0x1072311E, 0x10B81824,
    0x900040CB, 0x900040CA, 0x40006CC2, 0x900050DC,
    0x930610DB, 0x9000109C, 0x10B82424, 0x900050D9,
    0x3328191E, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0xA000008C, 0x1018008C, 0x1818562B,
    0xA0000096, 0x33281818, 0x3328241E, 0xA000008C,
    0x141A4924, 0xA0008006, 0x00000000
};

static void gf2m_point_add_b163(void)
{
    memcpy((uint32_t *) (CRYPTO_BASE + 0x0540), gf2m_point_add_b163_bin, sizeof(gf2m_point_add_b163_bin));
}


const uint32_t gf2m_point_mul_b163_bin[] =
{
    0x35281800, 0x35282400, 0x30280000, 0x20214400,
    0x10180007, 0x1818562B, 0xA0000096, 0x10040006,
    0x92001090, 0x97000090, 0xA700000E, 0x10C01C00,
    0x181A4924, 0xA0000050, 0xAD000004, 0x00000000,
    0x00000000, 0xF0000000, 0x00000000
};

void gf2m_point_b163_mult(
    uint32_t *p_result_x, uint32_t *p_result_y,
    uint32_t *target_x, uint32_t *target_y,
    uint32_t *target_k
)
{

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C60), target_x, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C78), target_y, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C18), target_k, secp192r1_op_num);

    gf2m_inv2_b163();
    gf2m_point_double_b163();
    gf2m_point_add_b163();

    memcpy((uint32_t *) (CRYPTO_BASE + 0x0400), gf2m_point_mul_b163_bin, sizeof(gf2m_point_mul_b163_bin));

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(5, 3);

#else

    //Start the acceleator engine
    crypto_start(5, 3);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_copy(p_result_x, (uint32_t *)(CRYPTO_BASE + 0x0C30), secp192r1_op_num);
    crypto_copy(p_result_y, (uint32_t *)(CRYPTO_BASE + 0x0C48), secp192r1_op_num);

    /* Because some memory setting alreday changed
     * So next time we should re-init setting,
     * so we must release crypto mutex here.
     */
    crypto_mutex_unlock();

}

#endif

#if  MODULE_ENABLE(CRYPTO_SECP192R1_ENABLE)


const uint32_t gfp_modMultFast_p192_bin[] =
{
    0x182B5FAF, 0x187361B0, 0x35308C00, 0x35309A00,
    0x30300000, 0x10287146, 0x10708C4D, 0x8804309A,
    0x332B0A46, 0x35031600, 0x3303184C, 0x33231A4D,
    0x35032400, 0x102B0B8C, 0x10708D93, 0x8000309A,
    0x350B0A00, 0x331B0F8C, 0x800030B9, 0x33030B90,
    0x33030F90, 0x33030D91, 0x33031191, 0x350B1200,
    0x800030B9, 0x800200B0, 0xA60001D5, 0x33308D8C,
    0xA00001D1, 0x142B5FAF, 0x147361B0, 0x141773B9,
    0xA0008005, 0x00000000
};

const uint32_t gfp_point_mult_p192_bin1[] =
{
    0x30300000, 0x10282B07, 0x10722B1C, 0x10BA8C00,
    0x35020000, 0x8100009C, 0xA7000010, 0x8100009B,
    0xA700000D, 0x8100009A, 0xA700000F, 0xA300000D,
    0xA000001F, 0x35020001, 0xA000005E, 0x810050ED,
    0x35020002, 0xA000005E, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x33312638,
    0x33313546, 0x33314238, 0x33315146, 0x35003802,
    0x35283A00, 0x333070A1, 0x33328CA8, 0x33307E1C,
    0x10A05600, 0x18177BBD, 0xA0000195, 0x10A05C00,
    0x181775BA, 0xA00000D7, 0x33314238, 0x33315146,
    0x3330383F, 0x33307093, 0x33328C9A, 0x10A06C00,
    0x18177BBD, 0xA0000195, 0x33312638, 0x33313546,
    0x20217A00, 0x10140015, 0x820050D0, 0x970000D0,
    0xA8000040, 0xAD000039, 0x820050D0, 0xA0000052,
    0x10140015, 0x820050D0, 0x970000D0, 0xA8000047,
    0x1029269A, 0x107142A8, 0xA0000049, 0x102942A8,
    0x1071269A, 0x10C09800, 0x181B79BC, 0xA0000064,
    0x112C0601, 0x11500802, 0x10C0A200, 0x181B77BB,
    0xA0000140, 0xAD000040, 0x10140015, 0x820050D0,
    0x970000D0, 0xA8000059, 0x1029269A, 0x107142A8,
    0xA000005B, 0x102942A8, 0x1071269A, 0x10C0BC00,
    0x181B79BC, 0xA0000064, 0xF0000000, 0x00000000
};

const uint32_t gfp_point_mult_p192_bin2[] =
{
    0x30300000, 0x10280202, 0x10294293, 0x10702A00,
    0x10B91993, 0x8106509A, 0xA3000008, 0x33311993,
    0x3330708C, 0x33328C9A, 0x970000B0, 0xA700000D,
    0x33328CA8, 0x10A02000, 0x181773B9, 0xA00001B8,
    0x33311846, 0x3330708C, 0x33328C07, 0x10A02C00,
    0x181773B9, 0xA00001B8, 0x33311846, 0x3330548C,
    0x10A03600, 0x18176BB5, 0xA00000A0, 0x3331181C,
    0x3330708C, 0x33328C0E, 0x10A04200, 0x181773B9,
    0xA00001B8, 0x33311846, 0x3330708C, 0x33328C93,
    0x10602A00, 0x970000B0, 0xA7000028, 0x33328CA1,
    0x10A05600, 0x181773B9, 0xA00001B8, 0x33311846,
    0x10140015, 0x970000D0, 0xA8000032, 0x102942A8,
    0x1071269A, 0xA0000034, 0x1029269A, 0x107142A8,
    0x10C06E00, 0x181B77BB, 0xA0000140, 0x33307093,
    0x33328C9A, 0x33307E8C, 0x10A07A00, 0x18177BBD,
    0xA0000195, 0x33312638, 0x33313546, 0xF0000000,
    0x00000000
};

const uint32_t gfp_inv_p192_bin[] =
{
    0x33306200, 0x30300000, 0x35030A02, 0x35003802,
    0x35283A00, 0x35304600, 0x10285431, 0x10703823,
    0x10BB0B8C, 0x00000000, 0x00000000, 0x00000000,
    0x8100009A, 0xA70000D2, 0x4708309D, 0x470C30AD,
    0x8100009A, 0xA30000C0, 0xA00000C9, 0x00000000,
    0x00000000, 0x00000000, 0x86001090, 0x470230BD,
    0x810300B0, 0x860030B0, 0xA00000AC, 0x860020A0,
    0x470230CD, 0x810400C0, 0x860040C0, 0xA00000AC,
    0x8100109A, 0x86001090, 0x810630BC, 0xA30000C5,
    0x11780C03, 0x470230BD, 0x810300B0, 0x860030B0,
    0xA00000AC, 0x810020A9, 0x860020A0, 0x810640CB,
    0xA30000CE, 0x11980C04, 0x470230CD, 0x810400C0,
    0x860040C0, 0xA00000AC, 0x10A03800, 0x800050B0,
    0x141B6BB5, 0xA0008006, 0x00000000

};

const uint32_t gfp_double_jacobian_p192_bin[] =
{
    0x30300000, 0x10285431, 0x1070387E, 0x10B90B93,
    0x33305438, 0x33306346, 0x3330383F, 0x00000000,
    0x00000000, 0x00000000, 0x800000B0, 0xA7000139,
    0x33307031, 0x33328C31, 0x10A1D000, 0x181773B9,
    0xA00001B8, 0x3330FC46, 0x3330702A, 0x33328C7E,
    0x10A1DC00, 0x181773B9, 0xA00001B8, 0x33310A46,
    0x3330707E, 0x33328C7E, 0x10A1E800, 0x181773B9,
    0xA00001B8, 0x3330FC46, 0x33307031, 0x33328C1C,
    0x10A1F400, 0x181773B9, 0xA00001B8, 0x33306246,
    0x3330701C, 0x33328C1C, 0x10A20000, 0x181773B9,
    0xA00001B8, 0x33303846, 0x10B90B93, 0x8006109B,
    0xA6000105, 0x33305593, 0x800630BB, 0xA6000108,
    0x33303993, 0x8106309B, 0xA300010B, 0x33303993,
    0x3330702A, 0x33328C1C, 0x10A22000, 0x181773B9,
    0xA00001B8, 0x33305446, 0x10B90B93, 0x80063099,
    0xA6000115, 0x33303993, 0x8006109B, 0xA6000118,
    0x33305593, 0x35032602, 0x4702309E, 0x81010090,
    0x86001090, 0x00000000, 0x3330702A, 0x33328C2A,
    0x10A24400, 0x181773B9, 0xA00001B8, 0x33303846,
    0x10B90B93, 0x810630BD, 0xA3000127, 0x33303993,
    0x810630BD, 0xA300012A, 0x33303993, 0x810650DB,
    0xA300012D, 0x33310B93, 0x3330702A, 0x33328C85,
    0x10A26400, 0x181773B9, 0xA00001B8, 0x33305446,
    0x10080146, 0x8106209C, 0xA3000137, 0x33328D93,
    0x3330701C, 0x33307E31, 0x141775BA, 0xA0008005,
    0x00000000
};

const uint32_t gfp_applyz_p192_bin[] =
{
    0x30300000, 0x33305438, 0x33306346, 0x3330383F,
    0x3330701C, 0x33328C1C, 0x10A33C00, 0x181773B9,
    0xA00001B8, 0x3330FC46, 0x3330702A, 0x33328C7E,
    0x10A34800, 0x181773B9, 0xA00001B8, 0x33305446,
    0x3330707E, 0x33328C1C, 0x10A35400, 0x181773B9,
    0xA00001B8, 0x3330FC46, 0x33307031, 0x33328C7E,
    0x10A36000, 0x181773B9, 0xA00001B8, 0x33328C46,
    0x3330702A, 0x14177BBD, 0xA0008005, 0x00000000
};

const uint32_t gfp_xyczadd_p192_bin[] =
{

    0x10B87146, 0x80005090, 0x800060A0, 0x10B87F4D,
    0x800050B0, 0x800060C0, 0x182B65B2, 0x187367B3,
    0x10285431, 0x10703823, 0x10B8FD93, 0x33305438,
    0x33306346, 0x3330383F, 0x3330474D, 0x810650B9,
    0xA3000152, 0x3330FD93, 0x3330707E, 0x33328C7E,
    0x10A2AE00, 0x181773B9, 0xA00001B8, 0x3330FC46,
    0x3330702A, 0x33328C7E, 0x10A2BA00, 0x181773B9,
    0xA00001B8, 0x33305446, 0x3330701C, 0x33328C7E,
    0x10A2C600, 0x181773B9, 0xA00001B8, 0x33303846,
    0x810640CA, 0xA3000167, 0x33304793, 0x33307023,
    0x33328C23, 0x10A2D800, 0x181773B9, 0xA00001B8,
    0x3330FC46, 0x10B8FD93, 0x810650D9, 0xA3000171,
    0x3330FD93, 0x810650DB, 0xA3000174, 0x3330FD93,
    0x810630B9, 0xA3000177, 0x33303993, 0x33307031,
    0x33328C1C, 0x10A2F800, 0x181773B9, 0xA00001B8,
    0x33306246, 0x10B8FD93, 0x8106309D, 0xA3000181,
    0x33303993, 0x33307023, 0x33328C1C, 0x10A30C00,
    0x181773B9, 0xA00001B8, 0x33304646, 0x810640CA,
    0xA300018A, 0x33304793, 0x142B65B2, 0x147367B3,
    0x10B85431, 0x800010D0, 0x800020E0, 0x10B8FC23,
    0x800030D0, 0x800040E0, 0x141777BB, 0xA0008005,
    0x00000000

};


const uint32_t gfp_xyczaddc_p192_bin[] =
{
    0x10B87146, 0x80005090, 0x800060A0, 0x10B87F4D,
    0x800050B0, 0x800060C0, 0x182B65B2, 0x187367B3,
    0x10285431, 0x10703823, 0x10B8FD93, 0x33305438,
    0x33306346, 0x3330383F, 0x3330474D, 0x810650B9,
    0xA3000076, 0x3330FD93, 0x3330707E, 0x33328C7E,
    0x10A0F600, 0x181773B9, 0xA00001B8, 0x3330FC46,
    0x3330702A, 0x33328C7E, 0x10A10200, 0x181773B9,
    0xA00001B8, 0x33305446, 0x3330701C, 0x33328C7E,
    0x10A10E00, 0x181773B9, 0xA00001B8, 0x33303846,
    0x10A0FC00, 0x800650CA, 0xA600008C, 0x3330FD93,
    0x810640CA, 0xA300008F, 0x33304793, 0x10A10A00,
    0x810650B9, 0xA3000093, 0x33310B93, 0x33307031,
    0x33328C85, 0x10A13000, 0x181773B9, 0xA00001B8,
    0x33306246, 0x10A10A00, 0x8006509B, 0xA600009D,
    0x33310B93, 0x33307023, 0x33328C23, 0x10A14400,
    0x181773B9, 0xA00001B8, 0x33303846, 0x10A10A00,
    0x810630BD, 0xA30000A7, 0x33303993, 0x10A11800,
    0x8106509B, 0xA30000AB, 0x33311993, 0x33307023,
    0x33328C8C, 0x10A16000, 0x181773B9, 0xA00001B8,
    0x33304646, 0x810640CA, 0xA30000B4, 0x33304793,
    0x3330707E, 0x33328C7E, 0x10A17200, 0x181773B9,
    0xA00001B8, 0x33311846, 0x10710A8C, 0x810640CB,
    0xA30000BE, 0x33311993, 0x810630C9, 0xA30000C1,
    0x33310B93, 0x33307085, 0x33328C7E, 0x10A18C00,
    0x181773B9, 0xA00001B8, 0x33310A46, 0x10A10A00,
    0x810620DA, 0xA30000CB, 0x33306393, 0x142B65B2,
    0x147367B3, 0x10B91831, 0x800010D0, 0x800020E0,
    0x10B83823, 0x800030D0, 0x800040E0, 0x141779BC,
    0xA0008005, 0x00000000

};

const uint32_t polynomial_p192[secp192r1_op_num] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
const uint32_t const_0_p192[secp192r1_op_num] = {0};
const uint32_t const_1_p192[secp192r1_op_num] = {1};
const uint32_t order_sub1_p192[secp192r1_op_num] = {0xB4D22830, 0x146BC9B1, 0x99DEF836, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};





static void gfp_inv_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
     To jump to gfp_inv:
      1. Prepare source to PA_index
      2. Set return address in gfp_inv_ret_index
      3. Result is in PU_index
    */
    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x680)); //assign the PC point to CRYPTO_VLIW_BASE

    memcpy(target_addr, gfp_inv_p192_bin, sizeof(gfp_inv_p192_bin));

}



static void gfp_modMultFast_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
      Interface:
        input:in1, in2
        output:out1

      Below need backup and restore:
        memory: tmp,tmp2,tmp3
        reg: R5

    */
    target_addr = ((uint32_t *) (CRYPTO_BASE + 0xAE0));

    //copy R1,R2 to tmp to avoid memory pollution
    memcpy(target_addr, gfp_modMultFast_p192_bin, sizeof(gfp_modMultFast_p192_bin));

}


static void gfp_xyczadd_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
      Interface:
        input:R1, R2, R3, R4
        output:in1, in2, in3, in4

      Below need backup and restore:
        memory: root: PA(x1),PB(y1),PU(x2),PV(y2),tmp3(add/sub tmp),tmp4(t5)
                mul: tmp,tmp2,tmp3
        reg: R1,R2,R3,R4,R5,R6

    */
    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x900));

    memcpy(target_addr, gfp_xyczadd_p192_bin, sizeof(gfp_xyczadd_p192_bin));

}



static void gfp_xyczaddc_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
      Interface:
        input:R1, R2, R3, R4
        output:in1, in2, in3, in4

      Below need backup and restore:
        memory: root: PA(x1),PB(y1),PU(x2),PV(y2),tmp3(add/sub tmp),tmp4(t5),tmp5(t6),tmp6(t7)
                mul: tmp,tmp2,tmp3
        reg: R1,R2,R3,R4,R5,R6

    */

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0590));

    memcpy(target_addr, gfp_xyczaddc_p192_bin, sizeof(gfp_xyczaddc_p192_bin));

}


static void gfp_double_jacobian_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
      Interface:
        input:in1, in2, in3
        output:in1, in2, in3

      Below need backup and restore:
        memory: root: PA(x1),PB(y1),PU(z1),tmp4(t4),tmp5(t5),out1(0x1)
                mul: tmp,tmp2,tmp3
        reg: R1,R2,R3,R4,R5,R6

    */
    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x075C));

    memcpy(target_addr, gfp_double_jacobian_p192_bin, sizeof(gfp_double_jacobian_p192_bin));
}

static void gfp_applyz_p192(void)
{
    uint32_t *target_addr;  //variable for the Program Counter(PC)

    /*
      Interface:
        input:in1, in2, in3
        output:in1, in2

      Below need backup and restore:
        memory: root: PA(x1),PB(y1),PU(z1),tmp4(t1)
                mul: tmp,tmp2,tmp3
        reg: R5

    */
    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0A54));

    memcpy(target_addr, gfp_applyz_p192_bin, sizeof(gfp_applyz_p192_bin));

}

uint32_t gfp_point_p192_mult(
    uint32_t *p_result_x, uint32_t *p_result_y,
    uint32_t *target_x, uint32_t *target_y,
    uint32_t *target_k
)
{

    uint32_t *target_addr;  //variable for the Program Counter(PC)


    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0400)); //assign the PC point to CRYPTO_VLIW_BASE

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0CE0), target_x, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0F18), target_y, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C54), target_k, secp192r1_op_num);

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C1C), target_x, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C38), target_y, secp192r1_op_num);

    memcpy(target_addr, gfp_point_mult_p192_bin1, sizeof(gfp_point_mult_p192_bin1));



#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(6, 0);

#else

    //Start the acceleator engine
    crypto_start(6, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    if (*((uint32_t *)(CRYPTO_BASE + 0x0E00)) == 1)
    {
        // 1: key=1 or N-1
        crypto_copy(p_result_x, (uint32_t *)(CRYPTO_BASE + 0x0CE0), secp192r1_op_num);
        crypto_copy(p_result_y, (uint32_t *)(CRYPTO_BASE + 0x0F18), secp192r1_op_num);
        return STATUS_SUCCESS;
    }

    if (*((uint32_t *)(CRYPTO_BASE + 0x0E00)) == 0xFFFFFFFF)
    {
        //-1: key=0 or >= N (ERROR)  Invalid point.
        return STATUS_INVALID_PARAM;
    }

    //=================================================
    //second trigger
    //=================================================

    gfp_inv_p192();

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0400));

    memcpy(target_addr, gfp_point_mult_p192_bin2, sizeof(gfp_point_mult_p192_bin2));

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(6, 0);

#else

    //Start the acceleator engine
    crypto_start(6, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy(p_result_x, (uint32_t *)(CRYPTO_BASE + 0x0CE0), secp192r1_op_num);
    crypto_copy(p_result_y, (uint32_t *)(CRYPTO_BASE + 0x0F18), secp192r1_op_num);

    /* Because some memory setting alreday changed
     * So next time we should re-init setting,
     * so we must release crypto mutex here.
     */
    crypto_mutex_unlock();

    return STATUS_SUCCESS;

}

uint32_t gfp_ecc_curve_p192_init(void)
{
    //curve secp192r1

    crypto_mutex_lock();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C00), (uint32_t *)polynomial_p192, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E54), (uint32_t *)const_0_p192, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E70), (uint32_t *)const_1_p192, secp192r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E1C), (uint32_t *)order_sub1_p192, secp192r1_op_num);


    gfp_modMultFast_p192();
    gfp_double_jacobian_p192();

    gfp_applyz_p192();
    gfp_xyczadd_p192();
    gfp_xyczaddc_p192();

    return STATUS_SUCCESS;

}

#endif              //CRYPTO_SECP192R1_ENABLE

#if MODULE_ENABLE(CRYPTO_SECP256R1_ENABLE)


static const uint32_t polynomial_p256[secp256r1_op_num] =
{
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF
};

static const uint32_t const_0_p256[secp256r1_op_num] = {0};
static const uint32_t const_1_p256[secp256r1_op_num] = {1};

static const uint32_t order_sub1_p256[secp256r1_op_num] =
{
    0xFC632550, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
};


static uint32_t param_a_p256[secp256r1_op_num] =
{
    0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF
};

static uint32_t param_b_p256[secp256r1_op_num] =
{
    0x27D2604B, 0x3BCE3C3E, 0xCC53B0F6, 0x651D06B0,
    0x769886BC, 0xB3EBBD55, 0xAA3A93E7, 0x5AC635D8
};


/*little endian for SECP256R1 generator.*/
const ECPoint_P256 Curve_Gx_p256 =
{
    {
        0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4, 0xA0, 0x33, 0xEB, 0x2D, 0x81, 0x7D, 0x03, 0x77,
        0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, 0xF8, 0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B
    },
    {
        0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB, 0xCE, 0x5E, 0x31, 0x6B, 0x57, 0x33, 0xCE, 0x2B,
        0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, 0x8E, 0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F
    }
};

/*We need a big endian Gx */

const ECPoint_P256 Curve_Gx_p256_BE =
{
    {
        0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2,
        0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96
    },
    {
        0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16,
        0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5
    },
};

static const uint32_t order_p256[secp256r1_op_num] =
{
    0xFC632551, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF
};

const uint32_t gfp_double_jacobian_p256_bin[] =
{
    0x30400000, 0x10286C3F, 0x107048A2, 0x10B957BD,
    0x33406C48, 0x33407F5A, 0x33404851, 0x00000000,
    0x00000000, 0x00000000, 0x800000B0, 0xA7000139,
    0x3340903F, 0x3342B43F, 0x10A1D000, 0x1817D7EB,
    0xA00001B8, 0x3341445A, 0x33409036, 0x3342B4A2,
    0x10A1DC00, 0x1817D7EB, 0xA00001B8, 0x3341565A,
    0x334090A2, 0x3342B4A2, 0x10A1E800, 0x1817D7EB,
    0xA00001B8, 0x3341445A, 0x3340903F, 0x3342B424,
    0x10A1F400, 0x1817D7EB, 0xA00001B8, 0x33407E5A,
    0x33409024, 0x3342B424, 0x10A20000, 0x1817D7EB,
    0xA00001B8, 0x3340485A, 0x10B957BD, 0x8006109B,
    0xA6000105, 0x33406DBD, 0x800630BB, 0xA6000108,
    0x334049BD, 0x8106309B, 0xA300010B, 0x334049BD,
    0x33409036, 0x3342B424, 0x10A22000, 0x1817D7EB,
    0xA00001B8, 0x33406C5A, 0x10B957BD, 0x80063099,
    0xA6000115, 0x334049BD, 0x8006109B, 0xA6000118,
    0x33406DBD, 0x35037A02, 0x4702309E, 0x81010090,
    0x86001090, 0x00000000, 0x33409036, 0x3342B436,
    0x10A24400, 0x1817D7EB, 0xA00001B8, 0x3340485A,
    0x10B957BD, 0x810630BD, 0xA3000127, 0x334049BD,
    0x810630BD, 0xA300012A, 0x334049BD, 0x810650DB,
    0xA300012D, 0x334157BD, 0x33409036, 0x3342B4AB,
    0x10A26400, 0x1817D7EB, 0xA00001B8, 0x33406C5A,
    0x1008015A, 0x8106209C, 0xA3000137, 0x3342B5BD,
    0x33409024, 0x3340A23F, 0x1417D9EC, 0xA0008005,
    0x00000000
};


const uint32_t gfp_applyz_p256_bin[] =
{
    0x30400000, 0x33406C48, 0x33407F5A, 0x33404851,
    0x33409024, 0x3342B424, 0x10A33C00, 0x1817D7EB,
    0xA00001B8, 0x3341445A, 0x33409036, 0x3342B4A2,
    0x10A34800, 0x1817D7EB, 0xA00001B8, 0x33406C5A,
    0x334090A2, 0x3342B424, 0x10A35400, 0x1817D7EB,
    0xA00001B8, 0x3341445A, 0x3340903F, 0x3342B4A2,
    0x10A36000, 0x1817D7EB, 0xA00001B8, 0x3342B45A,
    0x33409036, 0x1417DFEF, 0xA0008005, 0x00000000
};


const uint32_t gfp_xyczadd_p256_bin[] =
{
    0x10B8915A, 0x80005090, 0x800060A0, 0x10B8A363,
    0x800050B0, 0x800060C0, 0x182BC9E4, 0x1873CBE5,
    0x10286C3F, 0x1070482D, 0x10B945BD, 0x33406C48,
    0x33407F5A, 0x33404851, 0x33405B63, 0x810650B9,
    0xA3000152, 0x334145BD, 0x334090A2, 0x3342B4A2,
    0x10A2AE00, 0x1817D7EB, 0xA00001B8, 0x3341445A,
    0x33409036, 0x3342B4A2, 0x10A2BA00, 0x1817D7EB,
    0xA00001B8, 0x33406C5A, 0x33409024, 0x3342B4A2,
    0x10A2C600, 0x1817D7EB, 0xA00001B8, 0x3340485A,
    0x810640CA, 0xA3000167, 0x33405BBD, 0x3340902D,
    0x3342B42D, 0x10A2D800, 0x1817D7EB, 0xA00001B8,
    0x3341445A, 0x10B945BD, 0x810650D9, 0xA3000171,
    0x334145BD, 0x810650DB, 0xA3000174, 0x334145BD,
    0x810630B9, 0xA3000177, 0x334049BD, 0x3340903F,
    0x3342B424, 0x10A2F800, 0x1817D7EB, 0xA00001B8,
    0x33407E5A, 0x10B945BD, 0x8106309D, 0xA3000181,
    0x334049BD, 0x3340902D, 0x3342B424, 0x10A30C00,
    0x1817D7EB, 0xA00001B8, 0x33405A5A, 0x810640CA,
    0xA300018A, 0x33405BBD, 0x142BC9E4, 0x1473CBE5,
    0x10B86C3F, 0x800010D0, 0x800020E0, 0x10B9442D,
    0x800030D0, 0x800040E0, 0x1417DBED, 0xA0008005,
    0x00000000

};


const uint32_t gfp_xyczaddc_p256_bin[] =
{
    0x10B8915A, 0x80005090, 0x800060A0, 0x10B8A363,
    0x800050B0, 0x800060C0, 0x182BC9E4, 0x1873CBE5,
    0x10286C3F, 0x1070482D, 0x10B945BD, 0x33406C48,
    0x33407F5A, 0x33404851, 0x33405B63, 0x810650B9,
    0xA3000076, 0x334145BD, 0x334090A2, 0x3342B4A2,
    0x10A0F600, 0x1817D7EB, 0xA00001B8, 0x3341445A,
    0x33409036, 0x3342B4A2, 0x10A10200, 0x1817D7EB,
    0xA00001B8, 0x33406C5A, 0x33409024, 0x3342B4A2,
    0x10A10E00, 0x1817D7EB, 0xA00001B8, 0x3340485A,
    0x10A14400, 0x800650CA, 0xA600008C, 0x334145BD,
    0x810640CA, 0xA300008F, 0x33405BBD, 0x10A15600,
    0x810650B9, 0xA3000093, 0x334157BD, 0x3340903F,
    0x3342B4AB, 0x10A13000, 0x1817D7EB, 0xA00001B8,
    0x33407E5A, 0x10A15600, 0x8006509B, 0xA600009D,
    0x334157BD, 0x3340902D, 0x3342B42D, 0x10A14400,
    0x1817D7EB, 0xA00001B8, 0x3340485A, 0x10A15600,
    0x810630BD, 0xA30000A7, 0x334049BD, 0x10A16800,
    0x8106509B, 0xA30000AB, 0x334169BD, 0x3340902D,
    0x3342B4B4, 0x10A16000, 0x1817D7EB, 0xA00001B8,
    0x33405A5A, 0x810640CA, 0xA30000B4, 0x33405BBD,
    0x334090A2, 0x3342B4A2, 0x10A17200, 0x1817D7EB,
    0xA00001B8, 0x3341685A, 0x107156B4, 0x810640CB,
    0xA30000BE, 0x334169BD, 0x810630C9, 0xA30000C1,
    0x334157BD, 0x334090AB, 0x3342B4A2, 0x10A18C00,
    0x1817D7EB, 0xA00001B8, 0x3341565A, 0x10A15600,
    0x810620DA, 0xA30000CB, 0x33407FBD, 0x142BC9E4,
    0x1473CBE5, 0x10B9683F, 0x800010D0, 0x800020E0,
    0x10B8482D, 0x800030D0, 0x800040E0, 0x1417DDEE,
    0xA0008005, 0x00000000
};

const uint32_t gfp_modMultFast_p256_bin[] =
{
    0x00000000, 0x182BC3E1, 0x1873C5E2, 0x3578B400,
    0x00000000, 0x1028915A, 0x1070B5BD, 0x88E0309A,
    0x333B565A, 0x35036600, 0x333B6862, 0x35037800,
    0x3500C400, 0x00000000, 0x102B57B4, 0x00000000,
    0x35135600, 0x33235DB7, 0x82001090, 0x800030B9,
    0x331B5DB8, 0x350B6400, 0x82001090, 0x800030B9,
    0x331357B4, 0x35135C00, 0x330B63BA, 0x35036600,
    0x800030B9, 0x331357B5, 0x33135DB9, 0x330363B9,
    0x330365B4, 0x800030B9, 0x800400B0, 0xA60001DE,
    0x3340B5BD, 0xA00001DA, 0x331357B7, 0x35135C00,
    0x330363B4, 0x330365B6, 0x810030B9, 0x331B57B8,
    0x350B5E00, 0x330363B5, 0x330365B7, 0x810030B9,
    0x331357B9, 0x33135DB4, 0x35036200, 0x330365B8,
    0x810030B9, 0x330B57BA, 0x35035A00, 0x33135DB5,
    0x35036200, 0x330365B9, 0x810030B9, 0x10140062,
    0x470300D0, 0x810300B0, 0xA60001F5, 0x142BC3E1,
    0x1473C5E2, 0x1417D7EB, 0xA0008005, 0x00000000
};


const uint32_t gfp_point_mult_p256_bin1[] =
{
    0x30400000, 0x10283709, 0x10723724, 0x10BAB400,
    0x35020000, 0x8100009C, 0xA7000010, 0x8100009B,
    0xA700000D, 0x8100009A, 0xA700000F, 0xA300000D,
    0xA000001F, 0x35020001, 0xA000005E, 0x810050ED,
    0x35020002, 0xA000005E, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x33417A48,
    0x33418D5A, 0x33419E48, 0x3341B15A, 0x35004802,
    0x35384A00, 0x334090CF, 0x3342B4D8, 0x3340A224,
    0x10A05600, 0x1817DFEF, 0xA0000195, 0x10A05C00,
    0x1817D9EC, 0xA00000D7, 0x33419E48, 0x3341B15A,
    0x33404851, 0x334090BD, 0x3342B4C6, 0x10A06C00,
    0x1817DFEF, 0xA0000195, 0x33417A48, 0x33418D5A,
    0x2021FA00, 0x1014001B, 0x820050D0, 0x970000D0,
    0xA8000040, 0xAD000039, 0x820050D0, 0xA0000052,
    0x1014001B, 0x820050D0, 0x970000D0, 0xA8000047,
    0x10297AC6, 0x10719ED8, 0xA0000049, 0x10299ED8,
    0x10717AC6, 0x10C09800, 0x181BDDEE, 0xA0000064,
    0x112C0601, 0x11500802, 0x10C0A200, 0x181BDBED,
    0xA0000140, 0xAD000040, 0x1014001B, 0x820050D0,
    0x970000D0, 0xA8000059, 0x10297AC6, 0x10719ED8,
    0xA000005B, 0x10299ED8, 0x10717AC6, 0x10C0BC00,
    0x181BDDEE, 0xA0000064, 0xF0000000, 0x00000000
};

const uint32_t gfp_point_mult_p256_bin2[] =
{
    0x30400000, 0x10280202, 0x10299EBD, 0x10703600,
    0x10B969BD, 0x8106509A, 0xA3000008, 0x334169BD,
    0x334090B4, 0x3342B4C6, 0x970000B0, 0xA700000D,
    0x3342B4D8, 0x10A02000, 0x1817D7EB, 0xA00001B8,
    0x3341685A, 0x334090B4, 0x3342B409, 0x10A02C00,
    0x1817D7EB, 0xA00001B8, 0x3341685A, 0x33406CB4,
    0x10A03600, 0x1817CFE7, 0xA00000A0, 0x33416824,
    0x334090B4, 0x3342B412, 0x10A04200, 0x1817D7EB,
    0xA00001B8, 0x3341685A, 0x334090B4, 0x3342B4BD,
    0x10603600, 0x970000B0, 0xA7000028, 0x3342B4CF,
    0x10A05600, 0x1817D7EB, 0xA00001B8, 0x3341685A,
    0x1014001B, 0x970000D0, 0xA8000032, 0x10299ED8,
    0x10717AC6, 0xA0000034, 0x10297AC6, 0x10719ED8,
    0x10C06E00, 0x181BDBED, 0xA0000140, 0x334090BD,
    0x3342B4C6, 0x3340A2B4, 0x10A07A00, 0x1817DFEF,
    0xA0000195, 0x33417A48, 0x33418D5A, 0xF0000000,
    0x00000000
};

const uint32_t gfp_inv_p256_bin[] =
{
    0x33407E00, 0x30400000, 0x35035602, 0x35004802,
    0x35384A00, 0x35405A00, 0x10286C3F, 0x1070482D,
    0x10BB57B4, 0x00000000, 0x00000000, 0x00000000,
    0x8100009A, 0xA70000D2, 0x4708309D, 0x470C30AD,
    0x8100009A, 0xA30000C0, 0xA00000C9, 0x00000000,
    0x00000000, 0x00000000, 0x86001090, 0x470230BD,
    0x810300B0, 0x860030B0, 0xA00000AC, 0x860020A0,
    0x470230CD, 0x810400C0, 0x860040C0, 0xA00000AC,
    0x8100109A, 0x86001090, 0x810630BC, 0xA30000C5,
    0x11780C03, 0x470230BD, 0x810300B0, 0x860030B0,
    0xA00000AC, 0x810020A9, 0x860020A0, 0x810640CB,
    0xA30000CE, 0x11980C04, 0x470230CD, 0x810400C0,
    0x860040C0, 0xA00000AC, 0x10A04800, 0x800050B0,
    0x141BCFE7, 0xA0008006, 0x00000000
};

const uint32_t gfp_ecdsa_verify_p256_bin[] =
{
    0x33421200, 0x33400112, 0x33406D2D, 0x10A00C00,
    0x1817CFE7, 0xA00000A0, 0x33432024, 0x33409151,
    0x3342B424, 0x3540B400, 0x3540C600, 0x10289112,
    0x10D37A48, 0x8106409A, 0xA3000010, 0x334091BD,
    0x1028915A, 0x1070B463, 0x8304309A, 0x10217A00,
    0x800010B0, 0x3540B400, 0x3540C600, 0x33409024,
    0x3342B536, 0x1028915A, 0x1070B463, 0x8304309A,
    0x10218C00, 0x800010B0, 0x33400109, 0x30400000,
    0x102AFCCF, 0x10D37B90, 0x3342FD3F, 0x33430F48,
    0x33419E09, 0x3341B012, 0x8106409A, 0xA3000029,
    0x334321BD, 0x10299ED8, 0x1072FD87, 0x10C05C00,
    0x181BDBED, 0xA0000140, 0x33406D90, 0x10A06400,
    0x1817CFE7, 0xA00000A0, 0x3340917E, 0x3342B587,
    0x3340A224, 0x10A07000, 0x1817DFEF, 0xA0000195,
    0x3342FC48, 0x33430F5A, 0x3500A202, 0x3538A400,
    0x00000000, 0x00000000, 0x2021FE00, 0x10D58CBD,
    0x820050D0, 0x970000D0, 0xA8000047, 0x820060E0,
    0x970000E0, 0xA800004D, 0xAD000040, 0x820060E0,
    0x970000E0, 0xA8000050, 0x3342D809, 0x3342EA12,
    0xA0000052, 0x3342D93F, 0x3342EB48, 0xA0000052,
    0x3342D97E, 0x3342EB87, 0xAD000054, 0xA0000085,
    0x3340916C, 0x3342B575, 0x10A0B200, 0x1817D9EC,
    0xA00000D7, 0x3342D848, 0x3342EB5A, 0x10D58CBD,
    0x820050D0, 0x970000D0, 0xA8000063, 0x820060E0,
    0x970000E0, 0xA8000069, 0xA0000084, 0x820060E0,
    0x970000E0, 0xA800006C, 0x33409009, 0x3342B412,
    0xA000006E, 0x3340913F, 0x3342B548, 0xA000006E,
    0x3340917E, 0x3342B587, 0x10A0E200, 0x1817DFEF,
    0xA0000195, 0x33432051, 0x1028915A, 0x1072D975,
    0x10D77AE1, 0x810650B9, 0xA3000078, 0x3341C3BD,
    0x00000000, 0x10C0F800, 0x181BDBED, 0xA0000140,
    0x00000000, 0x00000000, 0x33409190, 0x3342B4E1,
    0x10A10600, 0x1817D7EB, 0xA00001B8, 0x3340A25A,
    0xAD000054, 0x33406C51, 0x10A11200, 0x1817CFE7,
    0xA00000A0, 0x3340A224, 0x3340916C, 0x3342B575,
    0x10A11E00, 0x1817DFEF, 0xA0000195, 0x30400112,
    0x10289112, 0x10D37A5A, 0x8106409A, 0xA3000095,
    0x3340B5BD, 0x1028B536, 0x8100009A, 0xA700009A,
    0x35020000, 0xA000009B, 0x35020002, 0xF0000000
};


const uint32_t gfp_point_add_p256_bin[] =
{
    0x30400000, 0x35020002, 0x1028915A, 0x1070A363,
    0x8100009B, 0xA7000007, 0xA0000010, 0x810000AC,
    0xA700002B, 0x35020000, 0x3540B400, 0x3540C600,
    0xA000003E, 0x00000000, 0x00000000, 0x00000000,
    0x102AFCCF, 0x10D37B90, 0x3342FC48, 0x33430F5A,
    0x33419E51, 0x3341B163, 0x8106409A, 0xA3000019,
    0x334321BD, 0x10299ED8, 0x1072FD87, 0x10C03C00,
    0x181BDBED, 0xA0000140, 0x33406D90, 0x10A04400,
    0x1817CFE7, 0xA00000A0, 0x3340917E, 0x3342B587,
    0x3340A224, 0x10A05000, 0x1817DFEF, 0xA0000195,
    0x3340B448, 0x3340C75A, 0xA000003E, 0x3500A202,
    0x3538A400, 0x10A06000, 0x1817D9EC, 0xA00000D7,
    0x3342D848, 0x3342EB5A, 0x33406C51, 0x10A06C00,
    0x1817CFE7, 0xA00000A0, 0x3340A224, 0x3340916C,
    0x3342B575, 0x10A07800, 0x1817DFEF, 0xA0000195,
    0x3340B448, 0x3340C75A, 0xF0000000
};

const uint32_t gfp_valid_point_p256_bin[] =
{
    0x33407E48, 0x30400000, 0x3340915A, 0x10A00C00,
    0x1817D7EB, 0xA00001B8, 0x3341445A, 0x3340903F,
    0x3342B43F, 0x10A01800, 0x1817D7EB, 0xA00001B8,
    0x3340905A, 0x3342B43F, 0x10A02200, 0x1817D7EB,
    0xA00001B8, 0x3341565A, 0x3340906C, 0x3342B43F,
    0x10A02E00, 0x1817D7EB, 0xA00001B8, 0x1029565A,
    0x1078EBBD, 0x8006109A, 0xA600001C, 0x334157BD,
    0x8006109B, 0xA600001F, 0x334157BD, 0x102956A2,
    0x8100009A, 0xA7000024, 0x35020000, 0xA0000025,
    0x35020002, 0xF0000000,
};

static void gfp_inv_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0680));

    memcpy(target_addr, gfp_inv_p256_bin, sizeof(gfp_inv_p256_bin));

}

static void gfp_modMultFast_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0AE0));

    memcpy(target_addr, gfp_modMultFast_p256_bin, sizeof(gfp_modMultFast_p256_bin));

}

static void gfp_xyczadd_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0900));

    memcpy(target_addr, gfp_xyczadd_p256_bin, sizeof(gfp_xyczadd_p256_bin));

}

static void gfp_xyczaddc_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0590));

    memcpy(target_addr, gfp_xyczaddc_p256_bin, sizeof(gfp_xyczaddc_p256_bin));

}

static void gfp_double_jacobian_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x075C));

    memcpy(target_addr, gfp_double_jacobian_p256_bin, sizeof(gfp_double_jacobian_p256_bin));
}

static void gfp_applyz_p256(void)
{
    uint32_t *target_addr;

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0A54));

    memcpy(target_addr, gfp_applyz_p256_bin, sizeof(gfp_applyz_p256_bin));

}

uint32_t gfp_point_p256_mult(
    ECPoint_P256    *p_result_point,
    ECPoint_P256    *p_based_point,
    uint32_t        *p_target_k
)
{
    uint32_t *target_addr;

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0D20), (uint32_t *) p_based_point->x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0F68), (uint32_t *) p_based_point->y, secp256r1_op_num);

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C6C), (uint32_t *) p_target_k, secp256r1_op_num);

    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C24), (uint32_t *) p_based_point->x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0C48), (uint32_t *) p_based_point->y, secp256r1_op_num);

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0400));
    memcpy(target_addr, gfp_point_mult_p256_bin1, sizeof(gfp_point_mult_p256_bin1));

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    if (*((uint32_t *)(CRYPTO_BASE + 0x0E00)) != 0x0)
    {
        // 1: key=1 or N-1
        //-1: key=0 or >= N (ERROR)
        return  STATUS_INVALID_PARAM;
    }

    //=================================================
    //second trigger
    //=================================================

    gfp_inv_p256();

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x0400));


    if (*((uint32_t *)(CRYPTO_BASE + 0x0400)) == 1)
    {
        // 1: key=1 or N-1
        crypto_copy((uint32_t *) p_result_point->x, (uint32_t *)(CRYPTO_BASE + 0x0D20), secp256r1_op_num);
        crypto_copy((uint32_t *) p_result_point->y, (uint32_t *)(CRYPTO_BASE + 0x0F68), secp256r1_op_num);
        return  STATUS_SUCCESS;
    }
    else if (*((uint32_t *)(CRYPTO_BASE + 0x0400)) == 0xFFFFFFFF)
    {
        //-1: key=0 or >= N (ERROR)
        return  STATUS_INVALID_PARAM;
    }

    memcpy(target_addr, gfp_point_mult_p256_bin2, sizeof(gfp_point_mult_p256_bin2));


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy((uint32_t *) p_result_point->x, (uint32_t *)(CRYPTO_BASE + 0x0D20), secp256r1_op_num);
    crypto_copy((uint32_t *) p_result_point->y, (uint32_t *)(CRYPTO_BASE + 0x0F68), secp256r1_op_num);

    /* Because some memory setting alreday changed
     * So next time we should re-init setting,
     * so we must release crypto mutex here.
     */
    crypto_mutex_unlock();

    return STATUS_SUCCESS;
}

uint32_t gfp_ecc_curve_p256_init(void)
{
    //curve secp256r1

    crypto_mutex_lock();

    /*TODO: crypto device semaphore*/
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C00), (uint32_t *)polynomial_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E6C), (uint32_t *)const_0_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E90), (uint32_t *)const_1_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E24), (uint32_t *)order_sub1_p256, secp256r1_op_num);

    gfp_modMultFast_p256();
    gfp_applyz_p256();
    gfp_double_jacobian_p256();
    gfp_xyczadd_p256();
    gfp_xyczaddc_p256();

    return STATUS_SUCCESS;
}

void gfp_ecdsa_p256_verify_bin(void)
{

    uint32_t *target_addr;  //variable for the Program Counter(PC)

    target_addr = ((uint32_t *) (CRYPTO_BASE + 0x400));

    memcpy(target_addr, gfp_ecdsa_verify_p256_bin, sizeof(gfp_ecdsa_verify_p256_bin));

}

static void gfp_ecdsa_p256_load_bins(void)
{

    gfp_modMultFast_p256();

    gfp_applyz_p256();
    gfp_double_jacobian_p256();
    gfp_xyczadd_p256();
    gfp_inv_p256();
    gfp_ecdsa_p256_verify_bin();

}

/* Returns 1 if p_vli == 0, 0 otherwise. */
static int vli_isZero(uint32_t *p_vli)
{
    uint32_t  i;

    for (i = 0; i < secp256r1_op_num; ++i)
    {
        if (p_vli[i])
        {
            return 0;
        }
    }
    return 1;
}

/* Returns sign of p_left - p_right. */
static int vli_cmp(uint32_t *p_left, uint32_t *p_right)
{
    int i;
    for (i = secp256r1_op_num - 1; i >= 0; --i)
    {
        if (p_left[i] > p_right[i])
        {
            return 1;
        }
        else if (p_left[i] < p_right[i])
        {
            return -1;
        }
    }
    return 0;
}

uint32_t gfp_ecdsa_p256_verify(Signature_P256 *p_signature,
                               uint32_t *p_hash,
                               ECPoint_P256 *p_public_key)
{

    if (vli_isZero((uint32_t *) p_signature->r) || vli_isZero((uint32_t *) p_signature->s))
    {
        /* r, s must not be 0. */
        return STATUS_ERROR;
    }

    if (vli_cmp((uint32_t *)order_p256, (uint32_t *) p_signature->r) != 1 || vli_cmp((uint32_t *)order_p256, (uint32_t *) p_signature->s) != 1)
    {
        /* r, s must be < order_p256. */
        return STATUS_ERROR;
    }

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xED8), (uint32_t *) p_signature->r, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xEB4), (uint32_t *) p_signature->s, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF44), p_hash, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xEFC), (uint32_t *) p_public_key->x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF20), (uint32_t *) p_public_key->y, secp256r1_op_num);

    //Start the acceleator engine

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_mutex_unlock();

    return (*((uint32_t *)(CRYPTO_BASE + 0xE00)) == 1) ? STATUS_SUCCESS : STATUS_ERROR;

}

void gfp_ecdsa_p256_verify_init(void)
{

    crypto_mutex_lock();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C00), (uint32_t *) polynomial_p256, secp256r1_op_num);

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0E48), (uint32_t *) order_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C24), (uint32_t *) Curve_Gx_p256.x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0C48), (uint32_t *) Curve_Gx_p256.y, secp256r1_op_num);

    gfp_ecdsa_p256_load_bins();

}


static const uint32_t gfp_ecdsa_signature_p256_bin[] =
{
    0x33400112, 0x30400112, 0x10289112, 0x10D37B24,
    0x1076A21B, 0x810630BA, 0xA3000008, 0x3342A3BD,
    0x8106109A, 0xA300000B, 0x334091BD, 0x80000090,
    0xA7000028, 0x33406C1B, 0x10A02200, 0x1817CFE7,
    0xA00000A0, 0x33419E24, 0x3340A248, 0x1028A36C,
    0x1070B463, 0x10D77B51, 0x3540B400, 0x3540C600,
    0x8304309A, 0x00000000, 0x10299ED8, 0x800620DB,
    0xA600001E, 0x3341B1BD, 0x1070B463, 0x10D77B5A,
    0x3540B400, 0x3540C600, 0x8304309A, 0x800050B0,
    0x800000D0, 0xA7000028, 0x35020002, 0xA0000029,
    0x35020000, 0xF0000000, 0x00000000
};

uint32_t gfp_ecdsa_p256_signature(Signature_P256 *p_signature,
                                  uint32_t *p_hash_message, uint32_t *p_private_key,
                                  uint32_t *p_random_k)
{
    ECPoint_P256  point;
    uint32_t status;

    /*Notice: before calling this function, gfp_ecc_curve_p256_init should called, so   */

    if (vli_isZero(p_random_k) || vli_cmp( (uint32_t *)order_sub1_p256, p_random_k) != 1)
    {
        // The random number must be between 2 ~ "n-2"  ... (because algorithm)

        crypto_mutex_unlock();        /*because gfp_point_p256_mult will lock crypto, so we need to unlock when fail*/

        return STATUS_INVALID_PARAM;
    }

    /*before calling this function, gfp_ecc_curve_p256_init must be called */
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xE48), (uint32_t *)order_p256, secp256r1_op_num);

    status = gfp_point_p256_mult( (ECPoint_P256 *)&point, (ECPoint_P256 *) &Curve_Gx_p256, p_random_k);


    if (status == STATUS_INVALID_PARAM)
    {
        return status;    /*this random number k is Invalid... k=1?  almost impossible*/
    }

    crypto_mutex_lock();        /*because gfp_point_p256_mult will unlock crypto, so we need to relock.*/

    memcpy((uint32_t *) (CRYPTO_BASE + 0x400), gfp_ecdsa_signature_p256_bin, sizeof(gfp_ecdsa_signature_p256_bin));

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xFB0), p_private_key, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF44), p_hash_message, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xC6C), p_random_k, secp256r1_op_num);


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy( (uint32_t *) (p_signature->r), (uint32_t *)(CRYPTO_BASE + 0xD20), secp256r1_op_num);
    crypto_copy( (uint32_t *) (p_signature->s), (uint32_t *)(CRYPTO_BASE + 0xF68), secp256r1_op_num);

    crypto_mutex_unlock();

    return (*((uint32_t *)(CRYPTO_BASE + 0xE00)) == 1) ? STATUS_SUCCESS : STATUS_ERROR;

}


static void gfp_point_add_p256_load_bin(void)
{

    memcpy((uint32_t *) (CRYPTO_BASE + 0x400), gfp_point_add_p256_bin, sizeof(gfp_point_add_p256_bin));

}

uint32_t gfp_point_p256_add(ECPoint_P256 *p_point_result,
                            ECPoint_P256 *p_point_x1, ECPoint_P256 *p_point_x2)
{
    /*Here we load required function again..*/
    crypto_mutex_lock();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xC00), (uint32_t *)polynomial_p256, secp256r1_op_num);

    gfp_modMultFast_p256();
    gfp_applyz_p256();
    gfp_double_jacobian_p256();
    gfp_xyczadd_p256();

    /*load gfp_inv binary...*/
    gfp_inv_p256();

    gfp_point_add_p256_load_bin();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD20), (uint32_t *) (p_point_x1->x), secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF68), (uint32_t *) (p_point_x1->y), secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD44), (uint32_t *) (p_point_x2->x), secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF8C), (uint32_t *) (p_point_x2->y), secp256r1_op_num);


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_copy((uint32_t *)(p_point_result->x), (uint32_t *)(CRYPTO_BASE + 0xD68), secp256r1_op_num);
    crypto_copy((uint32_t *)(p_point_result->y), (uint32_t *)(CRYPTO_BASE + 0xD8C), secp256r1_op_num);

    crypto_mutex_unlock();

    return (*((uint32_t *)(CRYPTO_BASE + 0xE00)) == 1) ? STATUS_SUCCESS : STATUS_ERROR;
}

uint32_t gfp_valid_point_p256_verify(ECPoint_P256 *p_point)
{
    crypto_mutex_lock();

    gfp_modMultFast_p256();

    memcpy((uint32_t *)(CRYPTO_BASE + 0x400), gfp_valid_point_p256_bin, sizeof(gfp_valid_point_p256_bin));

    /*load p, param_a, param_b*/
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xC00), (uint32_t *) polynomial_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xDB0), (uint32_t *) param_a_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xDD4), (uint32_t *) param_b_p256, secp256r1_op_num);

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD20), (uint32_t *) (p_point->x), secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF68), (uint32_t *) (p_point->y), secp256r1_op_num);


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_mutex_unlock();

    return (*((uint32_t *)(CRYPTO_BASE + 0xE00)) == 1) ? STATUS_SUCCESS : STATUS_ERROR;
}

static const uint32_t gfp_scalar_vxh_bin[] =
{
    0x30400051, 0x1028915A, 0x1070B463, 0x3540B400,
    0x3540C600, 0x80000090, 0xA700000C, 0x800000A0,
    0xA700000C, 0x10B8B5BD, 0x8304309A, 0x800050B0,
    0x102AC65A, 0x8106209A, 0xA3000010, 0x3340B5BD,
    0xF0000000
};

void gfp_scalar_vxh_p256(uint32_t *p_result, uint32_t *p_x, uint32_t *p_hash, uint32_t *p_v)
{
    /*Here we load required function again..*/
    crypto_mutex_lock();

    /*load caculate binary */
    memcpy(((uint32_t *) (CRYPTO_BASE + 0x400)), gfp_scalar_vxh_bin, sizeof(gfp_scalar_vxh_bin));

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD20), (uint32_t *) p_x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF68), (uint32_t *) p_hash, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD44), (uint32_t *) order_p256, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF8C), (uint32_t *) p_v, secp256r1_op_num);

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_copy(p_result, (uint32_t *)(CRYPTO_BASE + 0xD68), secp256r1_op_num);

    crypto_mutex_unlock();

    return;
}

static const uint32_t gfp_scalar_modmult_bin[] =
{
    0x30400051, 0x1028915A, 0x1070B463, 0x3540B400,
    0x3540C600, 0x80000090, 0xA700000C, 0x800000A0,
    0xA700000C, 0x10A0B400, 0x8304309A, 0x800050B0,
    0xF0000000
};

void gfp_scalar_modmult_p256(uint32_t *p_result, uint32_t *p_x, uint32_t *p_y)
{
    /*Here we load required function again..*/
    crypto_mutex_lock();

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD20), (uint32_t *) p_x, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xF68), (uint32_t *) p_y, secp256r1_op_num);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0xD44), (uint32_t *) order_p256, secp256r1_op_num);

    /*load caculate binary */
    memcpy(((uint32_t *) (CRYPTO_BASE + 0x400)), gfp_scalar_modmult_bin, sizeof(gfp_scalar_modmult_bin));

#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(8, 0);

#else

    //Start the acceleator engine
    crypto_start(8, 0);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    crypto_copy(p_result, (uint32_t *)(CRYPTO_BASE + 0xD68), secp256r1_op_num);

    crypto_mutex_unlock();

    return;
}

uint32_t gfp_point_p256_invert(ECPoint_P256 *p_invert, ECPoint_P256 *p_point)
{
    int i, carry = 0, next_carry;

    uint32_t  *p_y, *p_poly, *p_output;

    /*we don't caculate p_point is valid or not here. Maybe it should check the point is vaild or not*/

    /*input address p_point could be output address p_invert*/

    if (p_point != p_invert)
    {
        /*point x coordinate is the same.*/
        memcpy(p_invert->x, p_point->x, (secp256r1_op_num << 2));
    }

    p_y = (uint32_t *) & (p_point->y);
    p_poly = (uint32_t *) & (polynomial_p256);
    p_output = (uint32_t *) & (p_invert->y);

    for (i = 0; i < secp256r1_op_num; i++)
    {
        /*point y coordinate is polynomial_p256 - point->y*/

        if (*p_poly >= *p_y)
        {
            next_carry = 0;
        }
        else
        {
            next_carry = 1;
        }

        *p_output = *p_poly - *p_y - carry;
        carry = next_carry;

        p_output++;
        p_y++;
        p_poly++;
    }

    return STATUS_SUCCESS;
}

#endif

#if MODULE_ENABLE(CRYPTO_C25519_ENABLE)



const uint32_t c25519_add_bin[] =
{
    0x10180028, 0x20200200, 0x00000000, 0x35405000,
    0x8010309A, 0x400026CD, 0x00000000, 0x40008DD0,
    0x801030BE, 0xAD0001EF, 0x141AF178, 0xA0008006
};

const uint32_t c25519_sub_bin[] =
{
    0x10180028, 0x20200200, 0x00000000, 0x35405000,
    0x8110309A, 0x400036CD, 0x00000000, 0x40008DD0,
    0x811030BE, 0xAD0001E0, 0x141AF379, 0xA0008006
};

const uint32_t c25519_muladdred_bin[] =
{
    0x10180028, 0x35006000, 0x20200600, 0x00000000,
    0x400026DC, 0x00000000, 0x12B80402, 0xAD0001BC,
    0x10180028, 0x11140002, 0x00000000, 0x8010309E,
    0x12B80201, 0x35005000, 0x20200600, 0x400026DC,
    0x00000000, 0x12B80402, 0xAD0001C7, 0x1098F028,
    0x801030BE, 0x11140003, 0x1314000A, 0x35405000,
    0x20200200, 0x400026DC, 0x00000000, 0x40008DD0,
    0x801030BE, 0xAD0001D1, 0x141AF57A, 0xA0008006
};

const uint32_t c25519_mul_bin[] =
{
    0x10D36528, 0x357A5000, 0x8800409A, 0x102A5032,
    0x1094F032, 0x33386530, 0x35026000, 0x181AF57A,
    0xA00001B8, 0x141AF77B, 0xA0008006
};


const uint32_t c25519_montdouble_bin[] =
{
    0x1828F67B, 0x11400200, 0x13401600, 0x10767944,
    0x1098F183, 0x181AF178, 0xA00001EA, 0x10768D4E,
    0x1098F187, 0x181AF379, 0xA00001DB, 0x102A793C,
    0x107A798B, 0x181AF77B, 0xA00001A9, 0x102A8D46,
    0x107A8D8F, 0x181AF77B, 0xA00001A9, 0x102A7946,
    0x140CF67B, 0x1098F194, 0x181AF77B, 0xA00001A9,
    0x102A7946, 0x10768D4E, 0x1098F199, 0x181AF379,
    0xA00001DB, 0x102A7946, 0x10767946, 0x1098F39E,
    0x181AF57A, 0xA00001B8, 0x102A7946, 0x140CF67B,
    0x13601600, 0x1098F1A4, 0x181AF77B, 0xA00001A9,
    0x141AF97C, 0xA0008006
};

const uint32_t c25519_mont_bin[] =
{
    0x1828F67B, 0x11400200, 0x13401600, 0x10767944,
    0x1098F129, 0x181AF379, 0xA00001DB, 0x10768D4E,
    0x1098F12D, 0x181AF178, 0xA00001EA, 0x1404F67B,
    0x11400200, 0x13401600, 0x1076A158, 0x1098F134,
    0x181AF379, 0xA00001DB, 0x1076B562, 0x1098F138,
    0x181AF178, 0xA00001EA, 0x102A795A, 0x10627800,
    0x1098F13D, 0x181AF77B, 0xA00001A9, 0x102A8D50,
    0x10628C00, 0x1098F142, 0x181AF77B, 0xA00001A9,
    0x102A7946, 0x1076C96C, 0x1098F147, 0x181AF178,
    0xA00001EA, 0x10768D4E, 0x1098F14B, 0x181AF379,
    0xA00001DB, 0x102AC964, 0x1460F67B, 0x10180150,
    0x181AF77B, 0xA00001A9, 0x102A8D46, 0x107A7954,
    0x181AF77B, 0xA00001A9, 0x102A7864, 0x1460F67B,
    0x13601600, 0x1018015A, 0x181AF77B, 0xA00001A9,
    0x102AB55A, 0x107A795E, 0x181AF77B, 0xA00001A9,
    0x102AA150, 0x107A8D62, 0x181AF77B, 0xA00001A9,
    0x102A7946, 0x140CF67B, 0x10180167, 0x181AF77B,
    0xA00001A9, 0x102A7946, 0x10768D4E, 0x1098F16C,
    0x181AF379, 0xA00001DB, 0x102A7946, 0x10727879,
    0x10D6E346, 0x181AF57A, 0xA00001B8, 0x102A7946,
    0x140CF67B, 0x13601600, 0x10180177, 0x181AF77B,
    0xA00001A9, 0x141AFB7D, 0xA0008006
};

const uint32_t c25519_inv_bin[] =
{
    0x1828F67B, 0x11080001, 0x107A7869, 0x181AF77B,
    0xA00001A9, 0x102A793C, 0x10788C6D, 0x181AF77B,
    0xA00001A9, 0x10288C46, 0x10787871, 0x181AF77B,
    0xA00001A9, 0x1420F67B, 0x1008003C, 0x107A8C76,
    0x181AF77B, 0xA00001A9, 0x102A7946, 0x107AA07A,
    0x181AF77B, 0xA00001A9, 0x102AA150, 0x1078787E,
    0x181AF77B, 0xA00001A9, 0x10287946, 0x107A7882,
    0x181AF77B, 0xA00001A9, 0x102A793C, 0x10787886,
    0x181AF77B, 0xA00001A9, 0x20400200, 0x1028783C,
    0x10788C8B, 0x181AF77B, 0xA00001A9, 0x10288C46,
    0x1078788F, 0x181AF77B, 0xA00001A9, 0xAE000087,
    0x1028793C, 0x107A8C94, 0x181AF77B, 0xA00001A9,
    0x102A8D46, 0x10787898, 0x181AF77B, 0xA00001A9,
    0x1028783C, 0x10788C9C, 0x181AF77B, 0xA00001A9,
    0x20400600, 0x10288C46, 0x107878A1, 0x181AF77B,
    0xA00001A9, 0x1028783C, 0x10788CA5, 0x181AF77B,
    0xA00001A9, 0x10288D46, 0x107A78A9, 0x181AF77B,
    0xA00001A9, 0xAE00009D, 0x102A793C, 0x107878AE,
    0x181AF77B, 0xA00001A9, 0x1028783C, 0x10788CB2,
    0x181AF77B, 0xA00001A9, 0x20401000, 0x10288C46,
    0x107878B7, 0x181AF77B, 0xA00001A9, 0x1028783C,
    0x10788CBB, 0x181AF77B, 0xA00001A9, 0xAE0000B3,
    0x10288D3C, 0x107878C0, 0x181AF77B, 0xA00001A9,
    0x20400800, 0x1028783C, 0x10788CC5, 0x181AF77B,
    0xA00001A9, 0x10288C46, 0x107878C9, 0x181AF77B,
    0xA00001A9, 0xAE0000C1, 0x10287946, 0x107A78CE,
    0x181AF77B, 0xA00001A9, 0x102A793C, 0x107878D2,
    0x181AF77B, 0xA00001A9, 0x1028783C, 0x10788CD6,
    0x181AF77B, 0xA00001A9, 0x20402E00, 0x10288C46,
    0x107878DB, 0x181AF77B, 0xA00001A9, 0x1028783C,
    0x10788CDF, 0x181AF77B, 0xA00001A9, 0xAE0000D7,
    0x10288D3C, 0x107A8CE4, 0x181AF77B, 0xA00001A9,
    0x102A8D46, 0x10788CE8, 0x181AF77B, 0xA00001A9,
    0x10288C46, 0x107878EC, 0x181AF77B, 0xA00001A9,
    0x20406000, 0x1028783C, 0x10788CF1, 0x181AF77B,
    0xA00001A9, 0x10288C46, 0x107878F5, 0x181AF77B,
    0xA00001A9, 0xAE0000ED, 0x10287946, 0x10788CFA,
    0x181AF77B, 0xA00001A9, 0x20403000, 0x10288C46,
    0x107878FF, 0x181AF77B, 0xA00001A9, 0x1028783C,
    0x10788D03, 0x181AF77B, 0xA00001A9, 0xAE0000FB,
    0x10288D3C, 0x10787908, 0x181AF77B, 0xA00001A9,
    0x20400200, 0x1028783C, 0x10788D0D, 0x181AF77B,
    0xA00001A9, 0x10288C46, 0x10787911, 0x181AF77B,
    0xA00001A9, 0xAE000109, 0x1028783C, 0x10788D16,
    0x181AF77B, 0xA00001A9, 0x10288D50, 0x140CF67B,
    0x1018011B, 0x181AF77B, 0xA00001A9, 0x141AFD7E,
    0xA0008006
};


const uint32_t c25519_mod_bin[] =
{
    0x20200200, 0x10A8506E, 0x8110109A, 0x11600200,
    0x13601400, 0x40008DBD, 0x20400A00, 0x12A00200,
    0x400045B0, 0xAE00004D, 0x12A00200, 0x400165B0,
    0x12A00200, 0x40008DD0, 0x10A8506E, 0x8010109D,
    0xAD000047, 0x141AFF7F, 0xA0008006
};

const uint32_t c25519_pointmul_bin[] =
{
    0x1028C850, 0x10741211, 0x1098F005, 0x181AF178,
    0xA00001EA, 0x1028C809, 0x10780009, 0x181AF77B,
    0xA00001A9, 0x10280100, 0x1018000D, 0x181AF97C,
    0xA000017C, 0x10A0B400, 0x2041FA00, 0x820050D0,
    0x970000D0, 0xA8000013, 0xAE00000F, 0x00000000,
    0x10A0B400, 0x820050D0, 0x970000D0, 0xA800001A,
    0x102A0000, 0xA000001B, 0x10280100, 0x10C03C00,
    0x181AFB7D, 0xA0000122, 0xAE000014, 0x10281309,
    0x10C04600, 0x181AFD7E, 0xA0000064, 0x10280109,
    0x10782827, 0x181AF77B, 0xA00001A9, 0x1038282A,
    0x181AFF7F, 0xA0000046, 0xF0000000

};


/* Trim private key   */
inline void ecp_TrimSecretKey(uint8_t *X)
{
    X[0] &= 0xf8;
    X[31] = (X[31] | 0x40) & 0x7f;
}

/*
 *  Init Curve C25519 parameter.
 *  Parameter:
 *     blind_zr is 256 bytes RANDOM data. It uses to add entropy for different
 *  current or caculate timing.
 *
 *
 */

void curve_c25519_init(void)
{

    uint32_t polynomial[] = { 0xFFFFFFED, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                              0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF
                            };



    crypto_mutex_lock();

    /*These address if microcode address*/
    crypto_parm_copy_p1((uint32_t *) (CRYPTO_BASE + 0x0DB8), polynomial, C25519_Length);

    *((uint32_t *) (CRYPTO_BASE + 0x0DE0)) = 38;
    *((uint32_t *) (CRYPTO_BASE + 0x0DE4)) = 121665;


    /*load microcode firmware */
    memcpy((void *) (CRYPTO_BASE + 0x0BA8), c25519_add_bin, sizeof(c25519_add_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0B6C), c25519_sub_bin, sizeof(c25519_sub_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0AE0), c25519_muladdred_bin, sizeof(c25519_muladdred_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0AA4), c25519_mul_bin, sizeof(c25519_mul_bin));
    memcpy((void *) (CRYPTO_BASE + 0x09F0), c25519_montdouble_bin, sizeof(c25519_montdouble_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0888), c25519_mont_bin, sizeof(c25519_mont_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0590), c25519_inv_bin, sizeof(c25519_inv_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0518), c25519_mod_bin, sizeof(c25519_mod_bin));
    memcpy((void *) (CRYPTO_BASE + 0x0400), c25519_pointmul_bin, sizeof(c25519_pointmul_bin));

    return;
}


void curve25519_point_mul(uint32_t *blind_zr, uint32_t *public_key, uint32_t *secret_key, uint32_t *base_point)
{
    uint32_t  temp_key_0, temp_key_8;

    /*TODO: we should change this blind_zr random  value.*/
    uint32_t blind_zr_random_default[] = { 0x2CF92B82, 0x459E8BA8, 0x0B55F9584, 0x6DB0E537,
                                           0xDBF5F46A, 0x2AED4E78, 0x65F44FFD, 0x73FF0C54
                                         };

    /*save the secret_key original byte key[0] and key[31],
      it will be modify in ecp_TrimSecretKey*/
    temp_key_0 = secret_key[0];
    temp_key_8 = secret_key[7];

    /*Notice: this will modify key*/
    ecp_TrimSecretKey((uint8_t *) secret_key);

    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0D90),  base_point, C25519_Length);
    crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0D68),  secret_key, C25519_Length);


    if (blind_zr != NULL)
    {
        crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0D40), blind_zr, C25519_Length);
    }
    else
    {
        crypto_parm_copy_p1((uint32_t *)(CRYPTO_BASE + 0x0D40), blind_zr_random_default, C25519_Length);
    }

    /*restore original key*/
    secret_key[0] = temp_key_0;
    secret_key[7] = temp_key_8;


#if  MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

    //Start the acceleator engine, and release CPU to other task
    wait_crypto_operation_finish(C25519_Length - 1, 31);

#else

    //Start the acceleator engine
    crypto_start(C25519_Length - 1, 31);
    //Waiting for calculation done
    while (!CRYPTO->CRYPTO_CFG.bit.crypto_done);

    //clear the VLW_DEF register
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;
#endif

    //copy result
    crypto_copy(public_key, (uint32_t *)(CRYPTO_BASE + 0x0C50), C25519_Length);

    return;

}

uint32_t curve25519_release(void)
{
    /*
     * TODO: Not implement yet. --- Reserved for future.
     *  this function is designed for multi-tasking
     */
    crypto_mutex_unlock();

    return  STATUS_SUCCESS;
}

#endif

#if MODULE_ENABLE(CRYPTO_FreeRTOS_ISR_ENABLE)

void wait_crypto_operation_finish(uint8_t op_num, uint8_t sb_num)
{
    uint32_t ulNotifiedValue;

    /*get current Task handler*/
    wait_notify_task = xTaskGetCurrentTaskHandle();

    crypto_start(op_num, sb_num);

    /*block the task until crypto opeation finish*/
    xTaskNotifyWait(0, CRYPTO_ISR_FINISH, &ulNotifiedValue, portMAX_DELAY);

    if (ulNotifiedValue != CRYPTO_ISR_FINISH)
    {
        /*Oops...debug.. almost impossible*/
        //printf("Error \n");
        while (1);
    }
}


/*
 * crypto interrupt handler, because only one task
 * can use crypto resource, so we can use tasknotify (light weight event notify)
 */
void  crypto_handler(void)
{
    BaseType_t    xHigherPriorityTaskWoken = pdFALSE;

    /*clear interrupt status*/
    CRYPTO->CRYPTO_CFG.bit.clr_crypto_int = 1;

    /*wakeup task*/
    if (wait_notify_task != NULL)
    {
        xTaskNotifyFromISR(wait_notify_task, CRYPTO_ISR_FINISH, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        wait_notify_task = NULL;
    }
    else
    {
        /*Oops...What happen?? check... almost impossible*/
        while (1);
    }

    return;
}

#endif

