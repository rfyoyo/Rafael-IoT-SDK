/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: Address compression, hash and etc.
*/

#ifndef ZB_ADDR_GLOBALS_H
#define ZB_ADDR_GLOBALS_H 1

/*! @cond internals_doc */
/*! \addtogroup ZB_NWK */
/*! @{ */

/**
   \par Define 'global address context' - common place to store and manage
   various ZigBee addresses. It contains array of Pan ID, IEEE(64-bit) and 16
  -bit address pairs. Protocol tables contains only 1 byte reference to the
   corresponding array. These arrays are hashes indeed, this speed up address
   search. To minimize memory usage open address hash is used.

   Address context consists of:
    - Pan ID's array - open addressing hash contains known Pan IDs. Seems MAC or NWK
   should clear it before new scan operation.
    - IEEE/16-bits address pair hash. Array of IEEE/16bit address pair. IEEE
   address is the key in this hash.
    - 16-bits address hash. The key of this hash is 16-bit address, used for 16
  -bits address and corresponding IEEE address search. This array contains
   reference to IEEE array.

   Clear and reuse elements in IEEE/16bit address map:
   Each IEEE/16-bits address pair has lock counter. Lock counter is zero when
   this address is not used in any table, those not used addresses organized in
   lru list. Head of lru list contains address elements that's not used for a
   long time.
*/

#include "zb_pooled_list.h"
#include "zb_address.h"


/**
   Manufacturer part of IEEE address
*/
typedef ZB_PACKED_PRE struct zb_dev_manufacturer_s
{
  zb_uint8_t device_manufacturer[3]; /*!< Manufactureer identifier - 3 octets */
}
ZB_PACKED_STRUCT
zb_dev_manufacturer_t;

/**
 * @name Address redirect type
 * @anchor mac_command_frame_id
 *
 * Note: These values were members of `enum zb_addr_redirect_type_e` type but were converted to a
 * set of macros due to MISRA violations.
 */
/** @{ */
/** Regular record */
#define ZB_ADDR_REDIRECT_NONE 0x00U
/** Entry contains only short address and redirects to regular record with both addresses */
#define ZB_ADDR_REDIRECT_SHORT 0x01U
/** Entry contains only IEEE address and redirects to regular record with both addresses*/
#define ZB_ADDR_REDIRECT_IEEE 0x02U
/** @} */


/* redirect/direct data structures are killed: they are the same actually. */
/* zb_address_map_t is moved to zb_address.h */


/**
   ext Pan ID and short Pan ID -> pan ref map
 */
typedef ZB_PACKED_PRE struct zb_pan_id_map_s
{
  zb_ext_pan_id_t long_panid;   /*!< 64-bit Pan ID */
  zb_uint16_t     short_panid;  /*!< 16-bit Pan ID */
} ZB_PACKED_STRUCT zb_pan_id_map_t;


/* EE: do not pack that data structure: will not win
 * significantly on RAM, but can get slower/bigger code. */
/**
   Global address context
*/
typedef struct zb_addr_globals_s
{
  zb_uint8_t used_manufacturer[ZB_DEV_MANUFACTURER_TABLE_SIZE / 8U + 1U];   /*!<
                                                                           * dev_manufacturer
                                                                           * usage mask  */
  zb_dev_manufacturer_t dev_manufacturer[ZB_DEV_MANUFACTURER_TABLE_SIZE]; /*!<
                                                                           * Manufacturers
                                                                           * array */

  zb_pan_id_map_t pan_map[ZB_PANID_TABLE_SIZE]; /*!< Pan ID's hash */
  zb_uint8_t used_pan_addr[ZB_PANID_TABLE_SIZE / 8U + 1U]; /*!< pan_addr usage mask  */

#ifndef ZB_CONFIGURABLE_MEM
  zb_address_map_t addr_map[ZB_IEEE_ADDR_TABLE_SIZE]; /*!< Address map - open
                                                       * hash by 64-bit address */
  zb_uint8_t short_sorted[ZB_IEEE_ADDR_TABLE_SIZE]; /*!< 16-bits address sort array */
#else
  zb_address_map_t *addr_map;
  zb_uint8_t *short_sorted;
#endif
  zb_ushort_t n_elements;                            /*!< # of elements in the
                                                     * address translation table  */
  zb_ushort_t n_sorted_elements; /*!< # of elements in the short_sorted[] */
  zb_ushort_t clock_i;           /*!< # of elements in the addr_map[] */
} zb_addr_globals_t;


/*! @} */
/*! @endcond */

#endif /* ZB_ADDR_GLOBALS_H */
