/***************************************************************************//**
* \file cy_ble_defines.h
* \version 2.0
*
* \brief
*  This file contains common defines for the BLE.
*
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef CY_BLE_DEFINES_H
#define CY_BLE_DEFINES_H

/** 
 * Definitions for CY_BLE_MODE
 */
#define CY_BLE_PROFILE                  (0u)  /* Complete BLE Protocol mode enables both BLE Host and Controller. */
#define CY_BLE_HCI                      (1u)  /* BLE Controller Only (HCI over UART) mode which enables the use of the
                                               * device as a BLE controller. */
    
/** 
 * Definitions for CY_BLE_STACK_MODE
 */    
#define CY_BLE_STACK_MODE_SINGLE_SOC    (3u)  /* Host and Controller with a software interface. */
#define CY_BLE_STACK_MODE_DUAL_IPC      (2u)  /* Host and Controller with an IPC interface. */
    
/** 
 * Definitions (masks) for CY_BLE_GAP_ROLE 
 */     
#define CY_BLE_GAP_HCI                  (0x00u) /* Used when the BLE operates in HCI mode. */
#define CY_BLE_GAP_PERIPHERAL           (0x01u) /* The device operates as peripheral. */
#define CY_BLE_GAP_CENTRAL              (0x02u) /* The device operates as central. */
#define CY_BLE_GAP_BROADCASTER          (0x04u) /* The device operates as a broadcaster. */
#define CY_BLE_GAP_OBSERVER             (0x08u) /* The device operates as an observer. */

/** 
 * Definitions (masks) for CY_BLE_GATT_ROLE 
 */         
#define CY_BLE_GATT_SERVER              (0x01u) /* Server */
#define CY_BLE_GATT_CLIENT              (0x02u) /* Client */
#define CY_BLE_GATT_BOTH                (CY_BLE_GATT_SERVER | CY_BLE_GATT_CLIENT)
        
/** 
 * Definitions for Cortex cores
 */    
#define CY_BLE_CORE_CORTEX_M4           (0u)
#define CY_BLE_CORE_CORTEX_M0P          (1u)
#define CY_BLE_CORE_CORTEX_NONE         (0xffu)

/** 
 * Definitions for CY_BLE_BONDING_REQUIREMENT
 */        
#define CY_BLE_BONDING_YES              (0x00u)
#define CY_BLE_BONDING_NO               (0x01u)

/** 
 * Definitions for CY_BLE_SHARING_MODE
 */ 
#define CY_BLE_SHARING_NONE             (0u)  /* The OTA feature is disabled. */
#define CY_BLE_SHARING_EXPORT           (1u)  /* Stack and Profiles */
#define CY_BLE_SHARING_IMPORT           (2u)  /* Profile only. */
    
    
/* Obsolete Definitions for CY_BLE_STACK_MODE options */
#define CY_BLE_CONFIG_STACK_HOST_ONLY   (1u)  /* Do not use. This option is not supported. */
#define CY_BLE_CONFIG_STACK_DEBUG       (2u)  /* Use CY_BLE_STACK_MODE_DUAL_IPC. */
#define CY_BLE_CONFIG_STACK_RELEASE     (3u)  /* Use CY_BLE_STACK_MODE_SINGLE_SOC. */
#define CY_BLE_CONFIG_STACK_DEBUG_UART  (4u)  /* Do not use. This option is not supported. */
#define CY_BLE_CONFIG_STACK_HOST_IPC    (5u)  /* Do not use. This option is not supported. */

#endif /* !defined(CY_BLE_DEFINES_H)*/

/* [] END OF FILE */
