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
/* PURPOSE: ZCL Continuous Value Change feature commands declarations
*/

#ifndef ZCL_CVC_COMMANDS_H
#define ZCL_CVC_COMMANDS_H 1

#if defined ZB_CVC_FEATURE_SUPPORT

/** @cond DOXYGEN_ZCL_SECTION */

/* Documentation - doc/HA/continuous_value_change-detailed_design.doc */

/* Internal variables,
   see 1.7.4 */

/** @brief CVC Transition Time As Fast As Able */
#define ZB_ZCL_CVC_TRANSITION_TIME_AS_FAST_AS_ABLE 0xffff
/** @brief CVC Transition Time minimal */
#define ZB_ZCL_CVC_TRANSITION_TIME_MINIMAL 0x0001
/** @brief CVC Transition Time error */
#define ZB_ZCL_CVC_TRANSITION_TIME_ERROR 20
/** @brief CVC Transition Time Unit (in msec) */
#define ZB_ZCL_CVC_TRANSITION_TIME_UNIT_MS 100
/** @brief CVC Transition Time Unit (in beacon intervals) */
#define ZB_ZCL_CVC_TRANSITION_TIME_UNIT_BE ZB_TIME_ONE_SECOND / 10
/** @brief CVC Invalid Remaining Time */
#define ZB_ZCL_CVC_INVALID_REMAINING_TIME 0xffff
/** @brief CVC Invalid Alarm Id */
#define ZB_ZCL_CVC_INVALID_ALARM_ID (zb_uint8_t)-1
/** @brief CVC Transition Timer Quant (in msec) */
#define ZB_ZCL_CVC_TRANSITION_TIMER_QUANT_MS 16
/** @brief CVC Transition Timer Quant */
#define ZB_ZCL_CVC_TRANSITION_TIMER_QUANT_BE \
  ZB_MILLISECONDS_TO_BEACON_INTERVAL(ZB_ZCL_CVC_TRANSITION_TIMER_QUANT_MS)
/** @brief CVC Transition Time Unit */
#define ZB_ZCL_CVC_TRANSITION_TIME_UNIT_IN_QUANTS \
  (ZB_ZCL_CVC_TRANSITION_TIME_UNIT_MS / ZB_ZCL_CVC_TRANSITION_TIMER_QUANT_MS)

#define ZB_ZCL_UNDEFINED_CVC_SLOT 0xff
#define ZB_ZCL_MAX_CVC_SLOTS_BY_EP (ZB_ZCL_UNDEFINED_CVC_SLOT / ZB_ZCL_MAX_EP_NUMBER)

/** @brief Value set function that will be used for setting value on device */
typedef zb_ret_t (ZB_CODE * zb_zcl_cvc_value_set_func_t)(zb_uint8_t endpoint, zb_uint16_t* new_value, zb_uint16_t remaining_time);


/** @internal Structure of Input variables
 */
typedef struct zb_zcl_cvc_input_variables_s
{
/** @brief Current Value */
  zb_uint16_t current_value16;
/** @brief End Value */
  zb_uint16_t end_value16;
/** @brief Min Value */
  zb_uint16_t min_value16;
/** @brief Max Value */
  zb_uint16_t max_value16;
/** @brief Overlapping flag */
  zb_uint8_t overlap;
/** @brief Transition Time */
  zb_uint16_t transition_time;
/** @brief Value set function */
  zb_zcl_cvc_value_set_func_t value_set_func;
/** @brief Buffer id for after_processing_cb */
  zb_uint8_t buf_id;
/** @brief After Processing callback */
  zb_callback_t after_processing_cb;
} zb_zcl_cvc_input_variables_t;

/** @internal Structure of Continuous Value Change variables
 */
typedef struct zb_zcl_cvc_variables_s
{
/** @brief Input variables */
  zb_zcl_cvc_input_variables_t input_var;
/** @brief Time to next scheduled operation (delta time) */
  zb_uint16_t delta_time;
/** @brief Delta value for one step */
  zb_int16_t delta_value16;
/** @brief Number of remaining steps for transition */
  zb_uint16_t steps_number;
/** @brief Step number for extra increment delta value */
  zb_uint16_t extra_inc_value_step;
/** @brief Step number for extra increment delta time */
  zb_uint16_t extra_inc_time_step;
/** @brief End time of transition */
  zb_time_t end_time;
/** @brief Available time error */
  zb_uint16_t time_err;
} zb_zcl_cvc_variables_t;

/** @internal Structure of Alarm variables
 */
typedef struct zb_zcl_cvc_alarm_variables_s
{
/** @brief Endpoint id */
  zb_uint8_t endpoint_id;
/** @brief Cluster id */
  zb_uint16_t cluster_id;
/** @brief Attribute id */
  zb_uint16_t attribute_id;
/** @brief Alarm buffer id */
  zb_uint8_t alarm_buf_id;
/** @brief Is Used flag */
  zb_bool_t is_used;
} ZB_PACKED_STRUCT
zb_zcl_cvc_alarm_variables_t;

/*!
  @brief Calculate transition values and put it into buffer.
  @param input_var - pointer to zb_zcl_cvc_input_variables_s containing input data
  @return buffer ID with zb_zcl_cvc_variables_s
*/
zb_uint8_t zb_zcl_cvc_calculate_transition_values(zb_zcl_cvc_input_variables_t* input_var);


/*!
  @brief Initialize and start transition.
  @param endpoint_id - ep id
  @param cluster_id - cluster id
  @param attribute_id - attribute id (from cluster)
  @param alarm_buf_id - buffer_id with zb_zcl_cvc_variables_s
  @return alarm ID
*/
zb_uint8_t zb_zcl_cvc_start_alarm(zb_uint8_t endpoint_id,
                                  zb_uint16_t cluster_id,
                                  zb_uint16_t attribute_id,
                                  zb_uint8_t alarm_buf_id);


/*!
  @brief Initialize and start transition.
  @param alarm_id - alarm id
  @return old transition's buffer (input_var.buf_id)
*/
zb_uint8_t zb_zcl_cvc_stop_transition(zb_uint8_t alarm_id);


/*!
  @brief Check transition running.
  @param endpoint_id - ep id
  @param cluster_id - cluster id
  @param attribute_id - attribute id (from cluster)
  @return alarm ID (ZB_ZCL_CVC_INVALID_ALARM_ID if it is not running)
*/
zb_uint8_t zb_zcl_cvc_check_transition_running(
  zb_uint8_t endpoint_id,
  zb_uint16_t cluster_id,
  zb_uint16_t attribute_id);


/*! @brief Initialize alarm list (stored in device context). */
void zb_zcl_init_cvc_alarm_info(void);


/*!
  @brief Get remaining time of transition.
  @param alarm_id - alarm id
  @return remaining time
*/
zb_uint16_t zb_zcl_cvc_get_remaining_time(zb_uint8_t alarm_id);

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#endif /* ZB_CVC_FEATURE_SUPPORT */

#endif /* ZCL_LEVEL_CVC_COMMANDS_H */
