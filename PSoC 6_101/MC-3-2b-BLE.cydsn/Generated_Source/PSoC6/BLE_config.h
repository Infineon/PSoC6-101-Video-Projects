/***************************************************************************//**
* \file CY_BLE_config.h
* \version 2.0
*
* \brief
*  This file contains auto-generated config defines for the BLE.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CONFIG_H)
#define CY_BLE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cyfitter.h"
#include "syslib/cy_syslib.h"
#include "ble/cy_ble_defines.h"

/**
* \addtogroup group_macros
* @{
*/


/***************************************
* Services Enable defines
***************************************/
#define CY_BLE_CUSTOM
#define CY_BLE_CUSTOM_SERVER

/***************************************
* API Constants
***************************************/
/**
 * The BLE operation mode.
 * The following defined values can be used:
 * CY_BLE_PROFILE (0u) - Complete BLE Protocol mode enables both the BLE Host 
 *                       and Controller.
 * CY_BLE_HCI     (1u) - BLE Controller Only (HCI over UART) mode enables
 *                       the use of a device as a BLE controller.
 */
#define CY_BLE_CONFIG_MODE                          (0u)

/**
 * The core for the Host in Profile mode.
 * The following defined values can be used:
 * CY_BLE_CORE_CORTEX_M4  (0u) - The Host located on CM4.
 * CY_BLE_CORE_CORTEX_M0P (1u) - The Host located on CM0p.
 */
#define CY_BLE_CONFIG_HOST_CORE                     (0u)

/** 
 * The core for the Controller in HCI mode.
 * The following defined values can be used:
 * CY_BLE_CORE_CORTEX_M4  (0u) - The Controller located on CM4.
 * CY_BLE_CORE_CORTEX_M0P (1u) - The Controller located on CM0p
 */
#define CY_BLE_CONFIG_HCI_CONTR_CORE                (1u)

/* BLE core definitions. */
#define CY_BLE_HOST_CORE                            ((CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4) ? \
                                                      CY_CPU_CORTEX_M4 : CY_CPU_CORTEX_M0P)

#define CY_BLE_HCI_CONTR_CORE                       ((CY_BLE_CONFIG_HCI_CONTR_CORE == CY_BLE_CORE_CORTEX_M4) ? \
                                                      CY_CPU_CORTEX_M4 : CY_CPU_CORTEX_M0P)

#define CY_BLE_CONTR_CORE                           ((CY_BLE_CONFIG_MODE == CY_BLE_HCI) ? CY_BLE_HCI_CONTR_CORE : \
                                                        ((CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M0P) ? \
                                                        CY_CPU_CORTEX_M4 : CY_CPU_CORTEX_M0P))
/* BLE modes definitions. */
#define CY_BLE_MODE_PROFILE                         ((CY_BLE_CONFIG_MODE == CY_BLE_PROFILE) && (CY_BLE_HOST_CORE))
#define CY_BLE_MODE_HCI                             ((CY_BLE_CONFIG_MODE == CY_BLE_HCI) && (CY_BLE_HCI_CONTR_CORE))


/* This parameter enables the external PA and LNA chip to enable the control pins. */
#define CY_BLE_CONFIG_EXT_PA_ENABLED                ((0u | \
                                                      0u | \
                                                      0u) != 0u)

/* Identifies if the low-power mode support is required for BLE_PDL.  */
#define CY_BLE_CONFIG_USE_DEEP_SLEEP                (1u)

/** 
 * The BLE Stack core mode.
 * The following defined values can be used:
 * CY_BLE_STACK_MODE_SINGLE_SOC (3u) - The Host and Controller with a software interface.
 * CY_BLE_STACK_MODE_DUAL_IPC   (2u) - The Host and Controller with an IPC interface.
 */
#define CY_BLE_CONFIG_STACK_MODE                    (2u)

#define CY_BLE_CONFIG_STACK_MODE_CONTR_UART         ((CY_BLE_MODE_HCI) || \
                                                    ((CY_BLE_CONFIG_MODE == CY_BLE_PROFILE) && \
                                                    (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG_UART) && \
                                                        CY_BLE_CONTR_CORE)) 

#define CY_BLE_CONFIG_STACK_MODE_HOST_UART          ((CY_BLE_MODE_PROFILE) && \
                                                    (((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_HOST_ONLY) || \
                                                     (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG_UART)) && \
                                                        CY_BLE_HOST_CORE))

#define CY_BLE_CONFIG_STACK_MODE_HOST_IPC           ((CY_BLE_MODE_PROFILE) && \
                                                    (((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_HOST_IPC) || \
                                                     (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG)) && \
                                                        CY_BLE_HOST_CORE))

#define CY_BLE_CONFIG_STACK_CONTR_CORE              ((CY_BLE_MODE_HCI) || \
                                                    ((CY_BLE_CONFIG_MODE == CY_BLE_PROFILE) && \
                                                    ((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_RELEASE) && \
                                                        (CY_BLE_HOST_CORE)) || \
                                                    (((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG) || \
                                                    (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG_UART)) &&\
                                                        (CY_BLE_CONTR_CORE))))

#define CY_BLE_CONFIG_STACK_IPC_CONTR_CORE          ((CY_BLE_CONFIG_MODE == CY_BLE_PROFILE) && \
                                                    (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG) && \
                                                    (CY_BLE_CONTR_CORE))

#define CY_BLE_CONFIG_CONTR_CORE                    ((CY_BLE_CONFIG_MODE == CY_BLE_HCI) ? \
                                                        CY_BLE_CONFIG_HCI_CONTR_CORE : \
                                                    ((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_RELEASE) ? \
                                                        CY_BLE_CONFIG_HOST_CORE : \
                                                    (((CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG) || \
                                                     (CY_BLE_CONFIG_STACK_MODE == CY_BLE_CONFIG_STACK_DEBUG_UART)) ? \
                                                        ((CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4) ? \
                                                            CY_BLE_CORE_CORTEX_M0P : CY_BLE_CORE_CORTEX_M4) : \
                                                        CY_BLE_CORE_CORTEX_NONE)) )

#define CY_BLE_HOST_CONTR_CORE                        (CY_BLE_MODE_PROFILE || CY_BLE_CONFIG_STACK_CONTR_CORE)    

/* Check for the proper BLESS ISR configuration in the DWR. */
#if(CY_BLE_CONFIG_STACK_CONTR_CORE)
    #if ((CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M4) && \
        (!defined BLE_1_bless_isr__INTC_CORTEXM4_ASSIGNED))
        #error Enable BLE_1_bless_isr interrupt for the ARM CM4 core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* (CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M4) */
    
    #if ((CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M0P) && \
        (!defined BLE_1_bless_isr__INTC_CORTEXM0P_ASSIGNED))
        #error Enable BLE_1_bless_isr interrupt for the ARM CM0+ core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M0P) */
#endif /* (CY_BLE_CONFIG_STACK_CONTR_CORE) */

/* Check for the proper UART ISR configuration in the DWR.  */
#if(CY_BLE_CONFIG_STACK_MODE_CONTR_UART)
    #if((CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M4) && \
        (!defined BLE_1_uart_isr__INTC_CORTEXM4_ASSIGNED))
        #error Enable BLE_1_uart_isr interrupt for the ARM CM4 core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M4 */
    
    #if((CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M0P) && \
        (!defined BLE_1_uart_isr__INTC_CORTEXM0P_ASSIGNED))
        #error Enable BLE_1_uart_isr interrupt for the ARM CM0+ core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* CY_BLE_CONFIG_CONTR_CORE == CY_BLE_CORE_CORTEX_M0P */
#endif /* (CY_BLE_CONFIG_STACK_MODE_CONTR_UART) */

/* Check for the proper HOST UART ISR configuration in the DWR.  */
#if(CY_BLE_CONFIG_STACK_MODE_HOST_UART)
    #if((CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4) && \
        (!defined BLE_1_host_uart_isr__INTC_CORTEXM4_ASSIGNED))
        #error Enable BLE_1_host_uart_isr interrupt for the ARM CM4 core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4 */
    
    #if((CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M0P) && \
        (!defined BLE_1_host_uart_isr__INTC_CORTEXM0P_ASSIGNED))
        #error Enable BLE_1_host_uart_isr interrupt for the ARM CM0+ core in the interrupt configuration tab of \
            the Design-Wide Resources (DWR) file (project.cydwr)     
    #endif /* CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M0P */
#endif /* (CY_BLE_CONFIG_STACK_MODE_HOST_UART) */

/** 
 * The maximum number of BLE connections.
 * This parameter displays how many BLE connections (both Central and 
 * Peripheral) are allowed. The valid range is from 1 to 4.
 */
#define CY_BLE_CONFIG_CONN_COUNT                    (1u)

/** The number of BLE connections (client). */
#define CY_BLE_CONFIG_GATTC_COUNT                   (0x00u)

/** The number of GAP Peripheral configurations structures. */
#define CY_BLE_CONFIG_GAP_PERIPHERAL_COUNT          (0x01u)

/** The number of GAP Broadcaster configurations structures. */
#define CY_BLE_CONFIG_GAP_BROADCASTER_COUNT         (0x00u)

/** The number of GAP Central configurations structures. */
#define CY_BLE_CONFIG_GAP_CENTRAL_COUNT             (0x00u)

/** The number of GAP Observer configurations structures. */
#define CY_BLE_CONFIG_GAP_OBSERVER_COUNT            (0x00u)

/** The number of Security configurations structures. */
#define CY_BLE_CONFIG_AUTH_INFO_COUNT               (0x01u)

/** LL Privacy 1.2 feature. */
#define CY_BLE_CONFIG_ENABLE_LL_PRIVACY             (0x01u)

/** LE 2 Mbps feature. */
#define CY_BLE_CONFIG_ENABLE_PHY_UPDATE             (0u)

/** Radio power calibration. */
#define CY_BLE_CONFIG_TX_POWER_CALIBRATION_ENABLE     (0u)      

/** The GAP security level. */
#define CY_BLE_CONFIG_GAP_SECURITY_LEVEL            (0x00u)

/** 
 * Sharing mode.
 * This option is used in the over-the-air (OTA) implementation. It allows 
 * sharing the BLE component code between two component instances: one instance
 * with profile-specific code and one with the stack.  
 * The following defined values can be used:
 * CY_BLE_SHARING_NONE   (0u) - The OTA feature is disabled.
 * CY_BLE_SHARING_EXPORT (1u) - Stack and Profiles.
 * CY_BLE_SHARING_IMPORT (2u) - Profile only.
 */
#define CY_BLE_CONFIG_SHARING_MODE                  (0u)

/**
 * The GAP Role. 
 * The following defined masks can be used:
 * CY_BLE_GAP_PERIPHERAL  (0x01u) - The device operates as peripheral.
 * CY_BLE_GAP_CENTRAL     (0x02u) - The device operates as central. 
 * CY_BLE_GAP_BROADCASTER (0x04u) - The device operates as a broadcaster.
 * CY_BLE_GAP_OBSERVER    (0x08u) - The device operates as an observer.
 * CY_BLE_GAP_HCI         (0x00u) - Used when the BLE operates in HCI mode.
 */ 
#define CY_BLE_CONFIG_GAP_ROLE                      (0x01u)

/** The Bonding Requirement. */
#define CY_BLE_CONFIG_BONDING_REQUIREMENT           (0x00u)

/** The maximum number of bonded devices to be supported by this device.  */
#define CY_BLE_CONFIG_MAX_BONDED_DEVICES            (16u)

/** 
 * The maximum number of peer devices whose addresses should be resolved 
 * by this device.
 */
#define CY_BLE_CONFIG_MAX_RESOLVABLE_DEVICES        (16u)

/** The maximum number of devices that can be added to the white list. */
#define CY_BLE_CONFIG_MAX_WHITE_LIST_SIZE           (16u)

#define CY_BLE_SECURITY_CONFIGURATION_0_INDEX   (0x00u)

#define CY_BLE_ADV_PKT_0_INDEX_FLAGS   (0x00u)
#define CY_BLE_ADV_PKT_0_INDEX_LOCAL_NAME   (0x03u)
#define CY_BLE_ADV_PKT_0_INDEX_SERVICE_UUID_128   (0x0Cu)
#define CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX   (0x00u)


/* The additional queue depth provided by the customizer.  */
#define CY_BLE_CONFIG_ADD_Q_DEPTH_PER_CONN          (0u)

/**
 * GATT MTU Size.
 * The maximum Transmission Unit size (bytes) of an attribute to be used in the design.
 * The valid range is from 23 to 512 bytes. 
 * This value is used to respond to an Exchange MTU request from the GATT Client.
 */
#define CY_BLE_CONFIG_GATT_MTU                      (0x0017u)

/** The GATT Maximum attribute length. */
#define CY_BLE_CONFIG_GATT_DB_MAX_VALUE_LEN         (0x0012u)
#define CY_BLE_GATT_DB_INDEX_COUNT                  (0x001Eu)

/** The number of characteristics supporting the Reliable Write property. */
#define CY_BLE_CONFIG_GATT_RELIABLE_CHAR_COUNT      (0x0000u)

/** The total length of characteristics with the Reliable Write property. */
#define CY_BLE_CONFIG_GATT_RELIABLE_CHAR_LENGTH     (0x0000u)

/** 
 * The parameter to enable an application to provide a dynamically allocated
 * buffer for preparing a Write request.  
 */            
#define CY_BLE_CONFIG_GATT_ENABLE_EXTERNAL_PREP_WRITE_BUFF   (0u)

/** The parameter to enable the configuration of the L2CAP logical channels. */
#define CY_BLE_CONFIG_L2CAP_ENABLE                  (1u)
    
#if(CY_BLE_CONFIG_L2CAP_ENABLE != 0u)
    /** L2CAP MTU Size. */
    #define CY_BLE_CONFIG_L2CAP_MTU                     (23u)
    
    /** L2CAP MPS Size. */
    #define CY_BLE_CONFIG_L2CAP_MPS                     (23u)
    
    /** The number of L2CAP Logical channels. */
    #define CY_BLE_CONFIG_L2CAP_LOGICAL_CHANNEL_COUNT   (1u) 

    /** Number of L2CAP PSMs. */
    #define CY_BLE_CONFIG_L2CAP_PSM_COUNT               (1u)
#endif  /* CY_BLE_L2CAP_ENABLE != 0u */

#define CY_BLE_CONFIG_GATT_DB_ATT_VAL_COUNT         (0x16u)

/** Max Tx payload size. */
#define CY_BLE_CONFIG_LL_MAX_TX_PAYLOAD_SIZE        (0x1Bu)

/** Max Rx payload size. */
#define CY_BLE_CONFIG_LL_MAX_RX_PAYLOAD_SIZE        (0x1Bu)

/** GATT Role. */
#define CY_BLE_CONFIG_GATT_ROLE                     (0x01u)
#define CY_BLE_CONFIG_GATT_DB_CCCD_COUNT            (0x06u)

/** Max unique services in the project. */
#define CY_BLE_MAX_SRVI                             (0x01u)

/***************************************
* API Constants for BLE services
* (using in cy_ble_<service>.h)
***************************************/

/* HIDS */
/** The maximum supported count of HID services for the GATT Server role. */
#define CY_BLE_CONFIG_HIDSS_SERVICE_COUNT           (0x00u)

/** The maximum supported count of HID reports for the GATT Server role. */
#define CY_BLE_CONFIG_HIDSS_REPORT_COUNT            (0x00u)
    
/** The maximum supported count of HID services for the GATT Client role. */
#define CY_BLE_CONFIG_HIDSC_SERVICE_COUNT           (0x00u)

/** The maximum supported count of HID reports for the GATT Client role.  */
#define CY_BLE_CONFIG_HIDSC_REPORT_COUNT            (0x00u)





/* BAS */
/** The maximum supported count of BAS services for the GATT Server role. */
#define CY_BLE_CONFIG_BASS_SERVICE_COUNT            (0x00u)

/** The maximum supported count of BAS reports for the GATT Client role. */
#define CY_BLE_CONFIG_BASC_SERVICE_COUNT            (0x00u)



/* ESS */
/** The maximum supported count of ESS characteristics for the GATT Client role. */
#define CY_BLE_CONFIG_ES_TOTAL_CHAR_COUNT           (0x00u)

/* AIOS */
/** The maximum supported count of AIOS characteristics for the GATT Client role. */
#define CY_BLE_CONFIG_AIO_TOTAL_CHAR_COUNT          (0x00u)

/* CUSTOM */
/** The maximum supported count of Custom services for the GATT Server role. */
#define CY_BLE_CONFIG_CUSTOMS_SERVICE_COUNT         (0x01u)

/** The maximum supported count of Custom services for the GATT Client role. */
#define CY_BLE_CONFIG_CUSTOMC_SERVICE_COUNT         (0x00u)

/** The maximum supported count of the Custom Service characteristics. */
#define CY_BLE_CONFIG_CUSTOM_SERVICE_CHAR_COUNT     (0x04u)

/** The maximum supported count of the Custom Service descriptors in one characteristic. */
#define CY_BLE_CONFIG_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT  (0x02u)

/**
 * Below are the indexes and handles of the defined Custom Services and 
 * their characteristics.
 */
#define CY_BLE_MOTOR_SERVICE_INDEX   (0x00u) /* Index of Motor service in the cy_ble_customs array */
#define CY_BLE_MOTOR_M1_CHAR_INDEX   (0x00u) /* Index of M1 characteristic */
#define CY_BLE_MOTOR_M1_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX   (0x00u) /* Index of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M1_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX   (0x01u) /* Index of Client Characteristic Configuration descriptor */
#define CY_BLE_MOTOR_M2_CHAR_INDEX   (0x01u) /* Index of M2 characteristic */
#define CY_BLE_MOTOR_M2_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX   (0x00u) /* Index of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M2_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX   (0x01u) /* Index of Client Characteristic Configuration descriptor */
#define CY_BLE_MOTOR_M1_REL_CHAR_INDEX   (0x02u) /* Index of M1_REL characteristic */
#define CY_BLE_MOTOR_M1_REL_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX   (0x00u) /* Index of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M2_REL_CHAR_INDEX   (0x03u) /* Index of M2_REL characteristic */
#define CY_BLE_MOTOR_M2_REL_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX   (0x00u) /* Index of Characteristic User Description descriptor */


#define CY_BLE_MOTOR_SERVICE_HANDLE   (0x0010u) /* Handle of Motor service */
#define CY_BLE_MOTOR_M1_DECL_HANDLE   (0x0011u) /* Handle of M1 characteristic declaration */
#define CY_BLE_MOTOR_M1_CHAR_HANDLE   (0x0012u) /* Handle of M1 characteristic */
#define CY_BLE_MOTOR_M1_CHARACTERISTIC_USER_DESCRIPTION_DESC_HANDLE   (0x0013u) /* Handle of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M1_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE   (0x0014u) /* Handle of Client Characteristic Configuration descriptor */
#define CY_BLE_MOTOR_M2_DECL_HANDLE   (0x0015u) /* Handle of M2 characteristic declaration */
#define CY_BLE_MOTOR_M2_CHAR_HANDLE   (0x0016u) /* Handle of M2 characteristic */
#define CY_BLE_MOTOR_M2_CHARACTERISTIC_USER_DESCRIPTION_DESC_HANDLE   (0x0017u) /* Handle of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M2_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE   (0x0018u) /* Handle of Client Characteristic Configuration descriptor */
#define CY_BLE_MOTOR_M1_REL_DECL_HANDLE   (0x0019u) /* Handle of M1_REL characteristic declaration */
#define CY_BLE_MOTOR_M1_REL_CHAR_HANDLE   (0x001Au) /* Handle of M1_REL characteristic */
#define CY_BLE_MOTOR_M1_REL_CHARACTERISTIC_USER_DESCRIPTION_DESC_HANDLE   (0x001Bu) /* Handle of Characteristic User Description descriptor */
#define CY_BLE_MOTOR_M2_REL_DECL_HANDLE   (0x001Cu) /* Handle of M2_REL characteristic declaration */
#define CY_BLE_MOTOR_M2_REL_CHAR_HANDLE   (0x001Du) /* Handle of M2_REL characteristic */
#define CY_BLE_MOTOR_M2_REL_CHARACTERISTIC_USER_DESCRIPTION_DESC_HANDLE   (0x001Eu) /* Handle of Characteristic User Description descriptor */



/** @} group_macros */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CY_BLE_CONFIG_H */

/* [] END OF FILE */
