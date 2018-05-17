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
static const cy_stc_ble_customs_t cy_ble_customs[0x01u] = {

    /* Motor service */
    {
        0x0010u, /* Handle of the Motor service */ 
        {

            /* M1 characteristic */
            {
                0x0012u, /* Handle of the M1 characteristic */ 
                
                /* Array of Descriptors handles */
                {
                    0x0013u, /* Handle of the Characteristic User Description descriptor */ 
                    0x0014u, /* Handle of the Client Characteristic Configuration descriptor */ 
                }, 
            },

            /* M2 characteristic */
            {
                0x0016u, /* Handle of the M2 characteristic */ 
                
                /* Array of Descriptors handles */
                {
                    0x0017u, /* Handle of the Characteristic User Description descriptor */ 
                    0x0018u, /* Handle of the Client Characteristic Configuration descriptor */ 
                }, 
            },

            /* M1_REL characteristic */
            {
                0x001Au, /* Handle of the M1_REL characteristic */ 
                
                /* Array of Descriptors handles */
                {
                    0x001Bu, /* Handle of the Characteristic User Description descriptor */ 
                    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, 
                }, 
            },

            /* M2_REL characteristic */
            {
                0x001Du, /* Handle of the M2_REL characteristic */ 
                
                /* Array of Descriptors handles */
                {
                    0x001Eu, /* Handle of the Characteristic User Description descriptor */ 
                    CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, 
                }, 
            },
        }, 
    },
};


#endif /* (CY_BLE_CUSTOM_SERVER) */

#ifdef CY_BLE_CUSTOM_CLIENT


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
