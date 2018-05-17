/***************************************************************************//**
* \file CY_BLE_ias_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure
*  for the Immediate Alert Service of the BLE Component.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "ble/cy_ble_ias.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_IAS))

#ifdef CY_BLE_IAS_SERVER
static const cy_stc_ble_iass_t cy_ble_iass =
{
    0x0010u,    /* Handle of the IAS service */
    0x0012u,    /* Handle of the Alert Level characteristic */
};
#endif /* (CY_BLE_IAS_SERVER) */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the Immediate Alert Service. */
cy_stc_ble_ias_config_t cy_ble_iasConfig =
{
    /* Service GATT DB handles structure */
    #ifdef CY_BLE_IAS_SERVER
    .iass = &cy_ble_iass,
    #else
    .iass = NULL,
    #endif /* CY_BLE_IAS_SERVER */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc = NULL,
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_IAS)) */

/* [] END OF FILE */
