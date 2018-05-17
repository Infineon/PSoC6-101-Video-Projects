/***************************************************************************//**
* \file CY_BLE.h
* \version 2.0
* 
* \brief
*  Contains the function prototypes and constants available to the BLE component.
* 
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CY_BLE_H)
#define CY_BLE_CY_BLE_H

#include "ble/cy_ble.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/***************************************
* Function Prototypes
***************************************/

/**
 \addtogroup group_functions
 @{
*/

cy_en_ble_api_result_t Cy_BLE_Start(cy_ble_callback_t  callbackFunc);
cy_en_ble_api_result_t Cy_BLE_Stop(void);

/** @} group_functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BLE_GAP_H  */


/* [] END OF FILE */
