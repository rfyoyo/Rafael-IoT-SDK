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
/* PURPOSE: Meter Identification cluster defintions
*/

#ifndef ZB_ZCL_METER_IDENTIFICATION_H
#define ZB_ZCL_METER_IDENTIFICATION_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION */

/** @addtogroup ZB_ZCL_METER_IDENTIFICATION
  * @{
  * @name Meter Identification cluster attributes
  * @{
  */

/** @brief Meter Identification server attributes identifiers
    @see ZCL spec, subclause 3.18.2.1
*/
enum zb_zcl_meter_identification_attr_e
{
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME      = 0x0000,  /**< @e CompanyName is a ZCL Octet String field capable
                                                                  *  of storing up to 16 character string (the first
                                                                  *  Octet indicates length) encoded in the UTF-8 format.
                                                                  *  Company Name defines the meter manufacturer name,
                                                                  *  decided by manufacturer
                                                                  */
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID     = 0x0001,  /**< @e MeterTypeID defines the Meter installation features,
                                                                  * decided by manufacturer.
                                                                  */
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID   = 0x0004,  /**< @e DataQualityID defines the Meter Simple Metering
                                                                  * information certification type, decided by manufacturer.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_CUSTOMER_NAME     = 0x0005,  /**< @e CustomerName is a ZCL Character String field capable
                                                                  * of storing up to 16 character string (the first Octet
                                                                  * indicates length) encoded in the ASCII format.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_MODEL             = 0x0006,  /**< @e Model is a ZCL Octet String field capable of storing
                                                                  * up to 16 character string (the first Octet indicates length)
                                                                  * encoded in the UTF-8 format. @e Model defines the meter model
                                                                  * name, decided by manufacturer.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_PART_NUMBER       = 0x0007,  /**< @e PartNumber is a ZCL Octet String field capable of storing
                                                                  * up to 16 character string (the first Octet indicates length)
                                                                  * encoded in the UTF-8 format. @e PartNumber defines the meter part
                                                                  * number, decided by manufacturer.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_PRODUCT_REVISION  = 0x0008,  /**< @e ProductRevision is a ZCL Octet String field capable
                                                                  * of storing up to 6 character string (the first Octet
                                                                  * indicates length) encoded in the UTF-8 format.
                                                                  * @e ProductRevision defines the meter revision code,
                                                                  * decided by manufacturer.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_SOFTWARE_REVISION = 0x000A,  /**< @e SoftwareRevision is a ZCL Octet String field capable
                                                                  * of storing up to 6 character string (the first Octet indicates
                                                                  * length) encoded in the UTF-8 format. SoftwareRevision defines
                                                                  * the meter software revision code, decided by manufacturer.
                                                                  */
  /* (O) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_UTILITY_NAME      = 0x000B,  /**< @e UtilityName is a ZCL Character String field capable of
                                                                  * storing up to 16 character string (the first Octet indicates
                                                                  * length) encoded in the ASCII format.
                                                                  */
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_POD               = 0x000C,  /**< @e POD (Point of Delivery) is a ZCL Character String field
                                                                  * capable of storing up to 16 character string (the first Octet
                                                                  * indicates length) encoded in the ASCII format. POD is the unique
                                                                  * identification ID of the premise connection point. It is also a
                                                                  * contractual information known by the clients and indicated in the bill.
                                                                  */
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER   = 0x000D,  /**< @e AvailablePower represents the @e InstantaneousDemand that
                                                                 * can be distributed to the customer (e.g., @e 3.3KW power)
                                                                 * without any risk of overload. The Available Power SHALL use
                                                                 * the same formatting conventions as the one used in the simple
                                                                 * metering cluster formatting attribute set for the
                                                                 * @e InstantaneousDemand attribute, i.e.,
                                                                 * the @e UnitOfMeasure and @e DemandFormatting.
                                                                 */
  /* (M) */
  ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD   = 0x000E   /**< @e PowerThreshold represents a threshold of @e InstantaneousDemand
                                                                  * distributed to the customer (e.g., 4.191KW) that will lead to an
                                                                  * imminent risk of overload.
                                                                  */
};


/** @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID "MeterTypeID" attribute values
 * @see ZCL spec, subclause 3.18.2.1.2
 */
typedef enum zb_zcl_meter_identification_meter_type_e
{
  ZB_ZCL_METER_TYPE_UTILITY_PRIMARY    = 0x0000,  /**< Utility Primary Meter */
  ZB_ZCL_METER_TYPE_UTILITY_PRODUCTION = 0x0001,  /**< Utility Production Meter */
  ZB_ZCL_METER_TYPE_UTILITY_SECONDARY  = 0x0002,  /**< Utility Secondary Meter */
  ZB_ZCL_METER_TYPE_PRIVATE_PRIMARY    = 0x0100,  /**< Private Primary Meter */
  ZB_ZCL_METER_TYPE_PRIVATE_PRODUCTION = 0x0101,  /**< Private Production Meter */
  ZB_ZCL_METER_TYPE_PRIVATE_SECONDARY  = 0x0102,  /**< Private Secondary Meters */
  ZB_ZCL_METER_TYPE_GENERIC            = 0x0110   /**< Generic Meter */
} zb_zcl_meter_identification_meter_type_t;


/** @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID "DataQualityID" attribute values
 * @see ZCL spec, subclause 3.18.2.1.3
 */
typedef enum zb_zcl_meter_identification_data_quality_e
{
  ZB_ZCL_DATA_QUALITY_ALL_DATA_CERTIFIED                       = 0x0000,  /**< All Data Certified */
  ZB_ZCL_DATA_QUALITY_ONLY_INSTANTANEOUS_POWER_NOT_CERTIFIED   = 0x0001,  /**< Only Instantaneous Power not Certified */
  ZB_ZCL_DATA_QUALITY_ONLY_CUMULATED_CONSUMPTION_NOT_CERTIFIED = 0x0002,  /**< Only Cumulated Consumption not Certified */
  ZB_ZCL_DATA_QUALITY_NOT_CERTIFIED_DATA                       = 0x0003   /**< Not Certified data */
} zb_zcl_meter_identification_data_quality_t;

/** @brief Default value for Company Name attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME_DEFAULT_VALUE { 0 }

/** @brief Default value for Meter Type ID attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID_DEFAULT_VALUE 0

/** @brief Default value for Data Quality ID attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID_DEFAULT_VALUE 0

/** @brief Default value for POD (Point of Delivery) attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_POD_DEFAULT_VALUE { 0 }

/** @brief Default value for Available Power attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER_DEFAULT_VALUE ZB_INIT_UINT24(0,0)

/** @brief Default value for Power Threshold attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD_DEFAULT_VALUE ZB_INIT_UINT24(0, 0)

/** @brief Max length of Company Name attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME_MAX_LENGTH 16

/** @brief Max length of POD (Poinf of Delivery) attribute */
#define ZB_ZCl_ATTR_METER_IDENTIFICATION_POD_MAX_LENGTH 16

/** @brief Declare attribute list for Thermostat cluster
    @param attr_list - attribute list name
    @param company_name - pointer to variable to store Company Name attribute value
    @param meter_type_id - pointer to variable to store Meter Type ID attribute value
    @param data_quality_id - pointer to variable to store Data Quality ID attribute value
    @param pod - pointer to variable to store POD (Point of Delivery) attribute value
    @param available_power - pointer to variable to store Availabe Power attribute value
    @param power_threshold - pointer to variable to store Power Threshold attribute value
*/
#define ZB_ZCL_DECLARE_METER_IDENTIFICATION_ATTRIB_LIST(attr_list,                           \
  company_name, meter_type_id, data_quality_id,                                              \
  pod, available_power, power_threshold)                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_COMPANY_NAME, (company_name))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_METER_TYPE_ID, (meter_type_id))      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID, (data_quality_id))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_POD, (pod))                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER, (available_power))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD, (power_threshold))  \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* Meter Identification cluster attributes */



/*! @name Meter Identification cluster commands
    @{
*/

/*! @} */ /* Meter Identification cluster commands */


/*!
    @cond internals_doc
    @internal @name Meter Identification cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_COMPANY_NAME(data_ptr)        \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME,                                              \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_METER_TYPE_ID(data_ptr)       \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID,                                             \
  ZB_ZCL_ATTR_TYPE_U16,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID(data_ptr)     \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID,                                           \
  ZB_ZCL_ATTR_TYPE_U16,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_POD(data_ptr)                 \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_POD,                                                       \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER(data_ptr)     \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER,                                           \
  ZB_ZCL_ATTR_TYPE_S24,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD(data_ptr)     \
{                                                                                             \
  ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD,                                           \
  ZB_ZCL_ATTR_TYPE_S24,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (void*) data_ptr                                                                       \
}

/*! @internal Number of attributes mandatory for reporting in Meter Identification cluster */
#define ZB_ZCL_METER_IDENTIFICATION_REPORT_ATTR_COUNT 0

/*! @}
    @endcond */ /* Thermostat cluster internals */

/*! @} */ /* ZB_ZCL_METER_IDENTIFICATION */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_meter_identification_init_server(void);
void zb_zcl_meter_identification_init_client(void);

#define ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION_SERVER_ROLE_INIT zb_zcl_meter_identification_init_server
#define ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION_CLIENT_ROLE_INIT zb_zcl_meter_identification_init_client

#endif /* ZB_ZCL_METER_IDENTIFICATION_H */
