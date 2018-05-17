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
    /* LED */
    { {0x37u, 0x82u, 0xA8u, 0xB5u, 0x6Fu, 0x31u, 0xC4u, 0x9Au, 0xA1u, 0x48u, 0xF5u, 0xF5u, 0x25u, 0xACu, 0xEEu, 0xE6u} },
    /* GREEN */
    { {0x8Eu, 0x8Eu, 0xDBu, 0xEEu, 0x85u, 0x4Eu, 0xFBu, 0xABu, 0x28u, 0x41u, 0xB3u, 0xC3u, 0x45u, 0x93u, 0x26u, 0x39u} },
};

static const cy_ble_uuid16_t cy_ble_customUuid16[] = {
    /* Characteristic User Description */
    0x2901u,
};

static cy_stc_ble_customc_desc_t cy_ble_customCDesc[0x01u] = {

    /* Characteristic User Description */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[0], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },
};

static cy_stc_ble_customc_char_t cy_ble_customCChar[0x01u] = {

    /* GREEN */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[1], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[0], /* Characteristic Descriptors */ 
    },
};

cy_stc_ble_customc_t cy_ble_customCServ[CY_BLE_CUSTOMC_SERVICE_COUNT] = {

    /* LED */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[0], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
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
