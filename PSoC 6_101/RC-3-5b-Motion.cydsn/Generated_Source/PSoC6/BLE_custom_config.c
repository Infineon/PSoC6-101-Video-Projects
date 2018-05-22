/***************************************************************************//**
* \file CY_BLE_custom_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure for
*  the Custom Service.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "ble/cy_ble_custom.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_CUSTOM))

#ifdef CY_BLE_CUSTOM_SERVER
/* If any Custom Service with custom characteristics is defined in the
* customizer's GUI, their handles will be present in this array.
*/
/* This array contains attribute handles for the defined Custom Services and their characteristics and descriptors.
   The array index definitions are located in the BLE_custom.h file. */


#endif /* (CY_BLE_CUSTOM_SERVER) */

#ifdef CY_BLE_CUSTOM_CLIENT

static const cy_stc_ble_uuid128_t cy_ble_customUuid128[] = {
    /* Motor */
    { {0x27u, 0x74u, 0x70u, 0xF7u, 0x6Du, 0x4Bu, 0x49u, 0x82u, 0x01u, 0x4Au, 0x9Eu, 0xCDu, 0x56u, 0x04u, 0x55u, 0x50u} },
    /* M1 */
    { {0x1Bu, 0x0Du, 0x34u, 0x93u, 0x0Fu, 0xFEu, 0xF5u, 0x97u, 0x28u, 0x47u, 0x44u, 0x6Bu, 0xA7u, 0xC4u, 0x51u, 0x16u} },
    /* M2 */
    { {0x6Cu, 0xF6u, 0x11u, 0xD1u, 0xF7u, 0xF2u, 0xFBu, 0xBFu, 0x4Bu, 0x41u, 0xFBu, 0x99u, 0xAFu, 0x03u, 0xDAu, 0x91u} },
    /* M1_REL */
    { {0x66u, 0x28u, 0x48u, 0x64u, 0x9Fu, 0x24u, 0x72u, 0x87u, 0x5Eu, 0x47u, 0x45u, 0xC2u, 0x44u, 0x31u, 0x63u, 0xE7u} },
    /* M2_REL */
    { {0x66u, 0x28u, 0x48u, 0x64u, 0x9Fu, 0x24u, 0x72u, 0x87u, 0x5Eu, 0x47u, 0x45u, 0xC2u, 0x44u, 0x31u, 0x63u, 0xE7u} },
};

static const cy_ble_uuid16_t cy_ble_customUuid16[] = {
    /* Characteristic User Description */
    0x2901u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Characteristic User Description */
    0x2901u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Characteristic User Description */
    0x2901u,
    /* Characteristic User Description */
    0x2901u,
};

static cy_stc_ble_customc_desc_t cy_ble_customCDesc[0x06u] = {

    /* Characteristic User Description */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[0], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[1], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Characteristic User Description */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[2], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[3], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Characteristic User Description */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[4], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Characteristic User Description */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[5], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },
};

static cy_stc_ble_customc_char_t cy_ble_customCChar[0x04u] = {

    /* M1 */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[1], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x02u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[0], /* Characteristic Descriptors */ 
    },

    /* M2 */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[2], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x02u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[2], /* Characteristic Descriptors */ 
    },

    /* M1_REL */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[3], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[4], /* Characteristic Descriptors */ 
    },

    /* M2_REL */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[4], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[5], /* Characteristic Descriptors */ 
    },
};

cy_stc_ble_customc_t cy_ble_customCServ[CY_BLE_CUSTOMC_SERVICE_COUNT] = {

    /* Motor */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[0], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x04u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[0], /* Custom Service Characteristics */ 
    },
};


#endif /* (CY_BLE_CUSTOM_CLIENT) */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the Custom Services. */
cy_stc_ble_custom_config_t cy_ble_customConfig =
{
    /* Custom Services GATT DB handles structure */
    #ifdef CY_BLE_CUSTOM_SERVER
    .customs      = cy_ble_customs,
    #endif /* (CY_BLE_CUSTOM_SERVER) */

    /* Custom Service handle */
    #ifdef CY_BLE_CUSTOM_CLIENT
    .customc  = cy_ble_customCServ,
    #endif /* (CY_BLE_CUSTOM_CLIENT) */
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_CUSTOM)) */

/* [] END OF FILE */
