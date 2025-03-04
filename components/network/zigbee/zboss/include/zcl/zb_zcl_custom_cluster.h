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
/* PURPOSE: Time cluster definitions
*/

#ifndef ZB_ZCL_CUSTOM_ATTR_H
#define ZB_ZCL_CUSTOM_ATTR_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond (DOXYGEN_HA_SECTION && internals_doc) */

/** @addtogroup zcl_api
 * @{
 */

#define ZB_ZCL_CLUSTER_ID_CUSTOM 0x1A0A
#define ZB_CUSTOM_DEVICE_ID 0x29A
#define ZB_CUSTOM_DEVICE_VERSION 6

/*! @name Custom Attributes cluster attributes
    @{
*/

/*! @brief Custom Attributes attribute identifiers
    @see ZCL spec, subclause 3.12.2.2
*/
enum zb_zcl_custom_cluster_attr_e
{
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID           = 0x0001,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID          = 0x0002,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID        = 0x0003,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID     = 0x0004,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID         = 0x0005,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID  = 0x0006,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID     = 0x0007,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID = 0x0008,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID         = 0x0009,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID  = 0x000a,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U16_ID          = 0x000b,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U32_ID          = 0x000c
};

#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BYTE_ARRAY_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_DEFAULT_VALUE ZB_FALSE
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_DEFAULT_VALUE { 0 }

/*! @brief Max size of char string attribute (with length byte) */
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_MAX_SIZE 11

/*! @brief Max size of byte array attribute (with length bytes) */
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BYTE_ARRAY_MAX_SIZE 66

/*! @} */ /* Custom Attributes cluster attributes */

/*! @name Custom Attributes commands
    @{
*/

/*! @brief Custom cluster commands identifiers */
enum zb_zcl_custom_cluster_cmd_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_ID = 0x21,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_ID = 0x22,
  ZB_ZCL_CUSTOM_CLUSTER_CMD3_ID = 0x23
};

/*! @brief Custom cluster responses identifiers */
enum zb_zcl_custom_cluster_resp_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_RESP_ID = 0xF1,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_RESP_ID = 0xF2
};

/******** Custom cluster command 1 ********/

enum zb_zcl_custom_cluster_cmd1_mode_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_MODE1 = 0x01,
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_MODE2 = 0x02
};

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd1_req_s
{
  zb_uint8_t mode;
  zb_uint8_t value;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd1_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD1_REQ(buf, req, parse_status)         \
{                                                                          \
  zb_zcl_custom_cluster_cmd1_req_t *req_ptr =                              \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd1_req_t) ?          \
    (zb_zcl_custom_cluster_cmd1_req_t*)zb_buf_begin(buf) : NULL;           \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                              \
  if (req_ptr)                                                             \
  {                                                                        \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_custom_cluster_cmd1_req_t));  \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
  }                                                                        \
}

/*! @brief Send Custom cluster command 1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param mode - mode value (@see zb_zcl_custom_cluster_cmd1_mode_t)
    @param value - some value
*/
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD1_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, mode, value)                                    \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD1_ID);          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (mode));                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (value));                                \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/******** Custom cluster Response to command 1 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd1_resp_s
{
  zb_uint8_t status;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd1_resp_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD1_RESP(buf, resp, parse_status)          \
{                                                                             \
  zb_zcl_custom_cluster_cmd1_resp_t *resp_ptr =                               \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd1_resp_t) ?            \
    (zb_zcl_custom_cluster_cmd1_resp_t*)zb_buf_begin(buf) : NULL;             \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                                 \
  if (resp_ptr)                                                               \
  {                                                                           \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_custom_cluster_cmd1_resp_t));  \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                               \
  }                                                                           \
}

/*! @brief Send Custom cluster response to command 1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param status - status value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD1_RESP(                        \
  buffer, seq, dst_addr, dst_addr_mode,                              \
  dst_ep, src_ep, cb, status)                                        \
{                                                                    \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                 \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(cmd_ptr);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                   \
    cmd_ptr, (seq), ZB_ZCL_CUSTOM_CLUSTER_CMD1_RESP_ID);             \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (status));                        \
  ZB_ZCL_FINISH_PACKET((buffer), cmd_ptr)                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                         \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),       \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));            \
}

/******** Custom cluster command 2 ********/

enum zb_zcl_custom_cluster_cmd2_param_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM1 = 0x01,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM2 = 0x02,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM3 = 0x03,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM4 = 0x04
};

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd2_req_s
{
  zb_uint8_t param;
  zb_uint16_t value;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd2_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD2_REQ(buf, req, parse_status)         \
{                                                                          \
  zb_zcl_custom_cluster_cmd2_req_t *req_ptr =                              \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd2_req_t) ?          \
    (zb_zcl_custom_cluster_cmd2_req_t*)zb_buf_begin(buf) : NULL;           \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                              \
  if (req_ptr)                                                             \
  {                                                                        \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_custom_cluster_cmd2_req_t));  \
    if (req_ptr->value < 0xFFFF)                                           \
    {                                                                      \
      parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                          \
    }                                                                      \
  }                                                                        \
}

/*! @brief Send Custom cluster command 2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param param - some parameter
    @param value - some value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD2_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, param, value)                                   \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD2_ID);          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (param));                                \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (value));                           \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/******** Custom cluster Response to command 2 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd2_resp_s
{
  zb_uint8_t status;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd2_resp_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD2_RESP(buf, resp, status)                \
{                                                                             \
  zb_zcl_custom_cluster_cmd2_resp_t *resp_ptr =                               \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd2_resp_t) ?            \
    (zb_zcl_custom_cluster_cmd2_resp_t*)zb_buf_begin(buf) : NULL;             \
  status = ZB_ZCL_PARSE_STATUS_FAILURE;                                       \
  if (resp_ptr)                                                               \
  {                                                                           \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_custom_cluster_cmd2_resp_t));  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                           \
}

/*! @brief Send Custom cluster response to command 2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param status - status value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD2_RESP(                        \
  buffer, seq, dst_addr, dst_addr_mode,                              \
  dst_ep, src_ep, cb, status)                                        \
{                                                                    \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                 \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(cmd_ptr);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                   \
    cmd_ptr, seq, ZB_ZCL_CUSTOM_CLUSTER_CMD2_RESP_ID);               \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (status));                        \
  ZB_ZCL_FINISH_PACKET((buffer), cmd_ptr)                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                         \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),       \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));            \
}

/******** Custom cluster command 3 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd3_req_s
{
  zb_char_t zcl_str[ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_MAX_SIZE];
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd3_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD3_REQ(buf, req, parse_status)  \
{                                                                   \
  zb_uint8_t zcl_str_len = *((zb_uint8_t*)zb_buf_begin(buf));       \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                       \
  if (((zb_uint_t)(zcl_str_len + 1)) ==  zb_buf_len(buf))                          \
  {                                                                 \
    ZB_MEMCPY(&(req), zb_buf_begin(buf), zcl_str_len + 1);          \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                     \
  }                                                                 \
}

/*! @brief Send Custom cluster command 3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param zcl_str_ptr - pointer to ZCL string
*/
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD3_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, zcl_str_ptr)                                    \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD3_ID);          \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (zcl_str_ptr));                         \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/*! @} */ /* Custom Attributes commands */

/*! @internal @name Custom Attributes internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID(data_ptr)           \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID(data_ptr)          \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID,                                                \
  ZB_ZCL_ATTR_TYPE_S16,                                                             \
  ZB_ZCL_ATTR_ACCESS_WRITE_ONLY,                                                    \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID(data_ptr)        \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID,                                              \
  ZB_ZCL_ATTR_TYPE_24BIT,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                    \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID(data_ptr)     \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID,                                           \
  ZB_ZCL_ATTR_TYPE_32BITMAP,                                                        \
  ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL,                                                \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID(data_ptr)         \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID,                                               \
  ZB_ZCL_ATTR_TYPE_IEEE_ADDR,                                                       \
  ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL | ZB_ZCL_ATTR_ACCESS_WRITE_ONLY,                \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID,                                        \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                     \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                                                      \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID(data_ptr)     \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID,                                           \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                      \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID,                                       \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                                    \
  ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                     \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID(data_ptr)         \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID,                                               \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                         \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID,                                        \
  ZB_ZCL_ATTR_TYPE_128_BIT_KEY,                                                     \
  ZB_ZCL_ATTR_MANUF_SPEC,                                                           \
  (void*) data_ptr                                                             \
}

/** @internal @brief Declare attribute list for Custom Attributes cluster
*/

#define ZB_ZCL_DECLARE_CUSTOM_ATTR_CLUSTER_ATTRIB_LIST(                                   \
  attr_list, u8_attr, s16_attr, _24bit_attr, _32bitmap_attr, ieee_attr,                   \
  char_string_attr, utc_time_attr, octet_string_attr, bool_attr, _128_bit_key_attr)       \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                           \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID, (u8_attr))                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID, (s16_attr))                   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID, (_24bit_attr))              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID, (_32bitmap_attr))        \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID, (ieee_attr))                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID, (char_string_attr))   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID, (utc_time_attr))         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID, (octet_string_attr)) \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID, (bool_attr))                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID, (_128_bit_key_attr))  \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @internal Number of attributes mandatory for reporting in Custom Attributes cluster */
#define ZB_ZCL_CUSTOM_ATTR_REPORT_ATTR_COUNT 8

/*! @} */ /* Custom Attributes cluster internals */

/*! @} */ /* ZCL Custom Attributes cluster definitions */

/** @endcond */ /* (DOXYGEN_ZCL_SECTION && internals_doc) */

void zb_zcl_custom_attr_init_server(void);
void zb_zcl_custom_attr_init_client(void);
#define ZB_ZCL_CLUSTER_ID_CUSTOM_SERVER_ROLE_INIT zb_zcl_custom_attr_init_server
#define ZB_ZCL_CLUSTER_ID_CUSTOM_CLIENT_ROLE_INIT zb_zcl_custom_attr_init_client

#endif /* ZB_ZCL_CUSTOM_ATTR_H */
