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
/* PURPOSE: IAS Control and Indicating Equipment device definition
*/

#ifndef ZB_COMCAST_SECURITY_CONTROL_H
#define ZB_COMCAST_SECURITY_CONTROL_H 1


#if defined ZB_HA_DEFINE_DEVICE_COMCAST_SECURITY_CONTROL || defined DOXYGEN

/** @cond DOXYGEN_HA_SECTION */

/**
 *  @defgroup ZB_COMCAST_SECURITY_CONTROL IAS Control with security
 *  @ingroup ZB_HA_DEVICES
 *  IAS Control with security
 *  @{
    @details
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_IDENTIFY \n
        - @ref ZB_ZCL_IAS_ACE \n
        - @ref ZB_ZCL_IAS_WD \n
        - @ref ZB_ZCL_IAS_ZONE \n
        - @ref ZB_ZCL_POLL_CONTROL

*/

#define ZB_HA_COMCAST_SECURITY_CONTROL_VER 0

/**< Control for test */
#define ZB_HA_COMCAST_SECURITY_CONTROL_ID       0xfff0

/**< Input clusters number. */
#define ZB_HA_COMCAST_SECURITY_CONTROL_IN_CLUSTER_NUM  3

/**< Output clusters number. */
#define ZB_HA_COMCAST_SECURITY_CONTROL_OUT_CLUSTER_NUM 6

/**
 *  @brief Declare cluster list for IAS Control and Indicating Equipment
 *  @param cluster_list_name [IN] - cluster list variable name.
 *  @param basic_attr_list [IN] - attribute list for Basic cluster.
 *  @param identify_attr_list [IN] - attribute list for Identify cluster.
 *  @param ias_ace_attr_list [IN] - attribute list for IAS ACE cluster.
 *  @param poll_control_attr_list [OUT] - attribute list for Poll Control cluster - client.
*/
#define ZB_HA_DECLARE_COMCAST_SECURITY_CONTROL_CLUSTER_LIST(                                  \
  cluster_list_name,                                                                          \
  basic_attr_list,                                                                            \
  identify_attr_list,                                                                         \
  ias_ace_attr_list,                                                                          \
  poll_control_attr_list)                                                                     \
      zb_zcl_cluster_desc_t cluster_list_name[] =                                             \
      {                                                                                       \
        ZB_ZCL_CLUSTER_DESC(                                                                  \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                                  \
          (basic_attr_list),                                                                  \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                         \
          ZB_ZCL_MANUF_CODE_INVALID                                                           \
        ),                                                                                    \
        ZB_ZCL_CLUSTER_DESC(                                                                  \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
          0,                                                            \
          NULL,                                                         \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                   \
          ZB_ZCL_MANUF_CODE_INVALID                                     \
          ),                                                            \
        ZB_ZCL_CLUSTER_DESC(                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                   \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),                               \
          (identify_attr_list),                                                               \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
          ZB_ZCL_MANUF_CODE_INVALID                                                           \
        ),                                                                                    \
        ZB_ZCL_CLUSTER_DESC(                                                                  \
          ZB_ZCL_CLUSTER_ID_IAS_ACE,                                                          \
          ZB_ZCL_ARRAY_SIZE(ias_ace_attr_list, zb_zcl_attr_t),                                \
          (ias_ace_attr_list),                                                                \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                                         \
          ZB_ZCL_MANUF_CODE_INVALID                                                           \
        ),                                                                                    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IAS_WD, 0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ), \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IAS_ZONE, 0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ), \
        ZB_ZCL_CLUSTER_DESC(                                                                  \
          ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                                     \
          ZB_ZCL_ARRAY_SIZE(poll_control_attr_list, zb_zcl_attr_t),                           \
          (poll_control_attr_list),                                                           \
          ZB_ZCL_CLUSTER_CLIENT_ROLE,                                                         \
          ZB_ZCL_MANUF_CODE_INVALID                                                           \
        ),                                                                                    \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, 0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ), \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT, 0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ), \
        ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, 0, NULL, ZB_ZCL_CLUSTER_CLIENT_ROLE, ZB_ZCL_MANUF_CODE_INVALID ) \
      }

/**
 *  @brief Declare simple descriptor for IAS Control and Indicating Equipment
 *  @param ep_name - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param in_clust_num [IN] - number of supported input clusters.
 *  @param out_clust_num [IN] - number of supported output clusters.
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name.
 */
#define ZB_HA_DECLARE_COMCAST_SECURITY_CONTROL_SIMPLE_DESC(                        \
  ep_name, ep_id, in_clust_num, out_clust_num)                                     \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                         \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
      {                                                                            \
        ep_id,                                                                     \
        ZB_AF_HA_PROFILE_ID,                                                       \
        ZB_HA_COMCAST_SECURITY_CONTROL_ID,                                         \
        ZB_HA_COMCAST_SECURITY_CONTROL_VER,                                        \
        0,                                                                         \
        in_clust_num,                                                              \
        out_clust_num,                                                             \
        {                                                                          \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                 \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                              \
          ZB_ZCL_CLUSTER_ID_IAS_ACE,                                               \
          ZB_ZCL_CLUSTER_ID_IAS_WD,                                                \
          ZB_ZCL_CLUSTER_ID_IAS_ZONE,                                              \
          ZB_ZCL_CLUSTER_ID_POLL_CONTROL,                                          \
          ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                      \
          ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,                               \
          ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT                               \
        }                                                                          \
      }

/**
 *  @brief Declare endpoint for IAS Control and Indicating Equipment
 *  @param ep_name [IN] - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param cluster_list [IN] - endpoint cluster list.
 */
#define ZB_HA_DECLARE_COMCAST_SECURITY_CONTROL_EP(ep_name, ep_id, cluster_list) \
      ZB_HA_DECLARE_COMCAST_SECURITY_CONTROL_SIMPLE_DESC(                       \
          ep_name,                                                              \
          ep_id,                                                                \
          ZB_HA_COMCAST_SECURITY_CONTROL_IN_CLUSTER_NUM,                        \
          ZB_HA_COMCAST_SECURITY_CONTROL_OUT_CLUSTER_NUM);                      \
        ZB_AF_DECLARE_ENDPOINT_DESC(                                            \
            ep_name,                                                            \
            ep_id,                                                              \
            ZB_AF_HA_PROFILE_ID,                                                \
            0,                                                                  \
            NULL,                                                               \
            ZB_ZCL_ARRAY_SIZE(                                                  \
                cluster_list,                                                   \
                zb_zcl_cluster_desc_t),                                         \
            cluster_list,                                                       \
            (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                   \
            0, NULL, /* No reporting ctx */                                     \
            0, NULL) /* No CVC ctx */

#define ZB_HA_DECLARE_COMCAST_SECURITY_CONTROL_CTX(device_ctx, ep_name) \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)


/**
 * @}
 */

/** @endcond */ /* DOXYGEN_HA_SECTION */

#endif /* defined ZB_HA_DEFINE_DEVICE_COMCAST_SECURITY_CONTROL || defined DOXYGEN */

#endif /* ZB_COMCAST_SECURITY_CONTROL_H */
