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
/* PURPOSE: Fan Control cluster definitions
*/

#ifndef ZB_ZCL_FAN_CONTROL_H
#define ZB_ZCL_FAN_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */


/* Cluster ZB_ZCL_CLUSTER_ID_FAN_CONTROL */

/*! @addtogroup ZB_ZCL_FAN_CONTROL
    @{
    @name Fan Control cluster attributes
    @{
*/

/*! @brief Fan Control cluster attribute identifiers
    @see ZCL spec, subclause 6.4.2.2
*/
enum zb_zcl_fan_control_attr_e
{
  /*! @brief Fan Mode attribute */
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID             = 0x0000,
  /*! @brief Fan Mode Sequence attribute */
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID    = 0x0001
};

/*! @brief Values for Fan Mode attribute,
    @see ZCL spec, subclause 6.4.2.2.1 */
enum zb_zcl_fan_control_fan_mode_e
{
  /*! Off value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_OFF      = 0x00,
  /*! Low value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_LOW      = 0x01,
  /*! Medium value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_MEDIUM   = 0x02,
  /*! High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_HIGH     = 0x03,
  /*! On value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_ON       = 0x04,
  /*! Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_AUTO     = 0x05,
  /*! Smart value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SMART    = 0x06,

  ZB_ZCL_FAN_CONTROL_FAN_MODE_RESERVED = 0x07
};

/*! @brief Values for Fan Mode Sequence attribute,
    @see ZCL spec, subclause 6.4.2.2.2 */
enum zb_zcl_fan_control_fan_mode_sequence_e
{
  /*! Low/Med/High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH      = 0x00,
  /*! Low/High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH          = 0x01,
  /*! Low/Med/High/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH_AUTO = 0x02,
  /*! Low/High/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH_AUTO     = 0x03,
  /*! On/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ON_AUTO           = 0x04,

  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_RESERVED          = 0x05
};

/** @brief Default value for Fan Mode attribute */
#define ZB_ZCL_FAN_CONTROL_FAN_MODE_DEFAULT_VALUE 0x05

/** @brief Default value for Fan Mode attribute */
#define ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_DEFAULT_VALUE 0x02

/** @brief Declare attribute list for Fan Control cluster
    @param attr_list - attribute list name
    @param fan_mode - pointer to variable to store Fan Mode attribute value
    @param fan_mode_sequence - pointer to variable to store Fan Mode Sequence attribute value
*/
#define ZB_ZCL_DECLARE_FAN_CONTROL_ATTRIB_LIST(attr_list, fan_mode, fan_mode_sequence)    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID, (fan_mode))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID, (fan_mode_sequence)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Fan Control cluster attributes */

/*! @name Fan Control cluster commands
    @{
*/

/*! @} */ /* Fan Control cluster commands */

/*! @cond internals_doc
    @internal @name Fan Control cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID(data_ptr) \
{                                                                            \
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID(data_ptr) \
{                                                                                     \
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                      \
  (void*) data_ptr                                                               \
}

/*! @internal Number of attributes mandatory for reporting in Fan Control cluster */
#define ZB_ZCL_FAN_CONTROL_REPORT_ATTR_COUNT 0

/*! @}
    @endcond */ /* Fan Control cluster internals */

/*! @} */ /* ZCL HA Fan Control cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_fan_control_init_server(void);
void zb_zcl_fan_control_init_client(void);
#define ZB_ZCL_CLUSTER_ID_FAN_CONTROL_SERVER_ROLE_INIT zb_zcl_fan_control_init_server
#define ZB_ZCL_CLUSTER_ID_FAN_CONTROL_CLIENT_ROLE_INIT zb_zcl_fan_control_init_client

#endif /* ZB_ZCL_FAN_CONTROL_H */
