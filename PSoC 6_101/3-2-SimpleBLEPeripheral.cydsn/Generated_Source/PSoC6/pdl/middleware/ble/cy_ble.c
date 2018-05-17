/***************************************************************************//**
* \file cy_ble.c
* \version 2.0
*
* \brief
*  This file contains the source code for the API of the BLE Middleware.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_ble_event_handler.h"
#include "cy_ble_hal_pvt.h"
#include "cy_ipc_config.h"
#include "ipc/cy_ipc_sema.h"
#include <stdlib.h>

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if (CY_BLE_HOST_CONTR_CORE)

/***************************************
* Global Variables
***************************************/

/* Allocate RAM memory for the stack. This buffer can be reused by the application when
 * the BLE Middleware is stopped. For the export mode, the buffer is allocated in a heap.
 */
#if (CY_BLE_SHARING_MODE_EXPORT)
uint8_t *cy_ble_stackMemoryRam = NULL;
#else
CY_ALIGN(sizeof(uint32_t)) CY_NOINIT uint8_t cy_ble_stackMemoryRam[CY_BLE_STACK_RAM_SIZE];
#endif  /* CY_BLE_SHARING_MODE_EXPORT */

#if (CY_BLE_MODE_PROFILE)
/** An application layer event callback function to receive service events from the BLE Middleware. */    
cy_ble_callback_t Cy_BLE_ApplCallback;
    
/* BLE application IPC notification handler */
cy_ble_app_notify_callback_t Cy_BLE_HostRegisteredCallback;     
#endif  /* (CY_BLE_MODE_PROFILE) */

#if(CY_BLE_SHARING_MODE != CY_BLE_SHARING_IMPORT)
/** Pointer to the global BLE Config structures */
cy_stc_ble_config_t *cy_ble_configPtr = NULL;
#endif /* CY_BLE_SHARING_MODE != CY_BLE_SHARING_IMPORT */

#if (CY_BLE_USE_DEEP_SLEEP != 0u)
  /* Structure with the syspm callback parameters for BLE deep-sleep */
  static cy_stc_syspm_callback_params_t bleDeepSleepCallbackParams = { .base = NULL};
  static cy_stc_syspm_callback_t bleDeepSleepCallback =
  {
    .callback = &Cy_BLE_DeepSleepCallback,
    .type = CY_SYSPM_DEEPSLEEP,
    .skipMode = CY_SYSPM_SKIP_BEFORE_TRANSITION | CY_SYSPM_SKIP_CHECK_FAIL,
    .callbackParams = &bleDeepSleepCallbackParams
  };

  /* Structure with the syspm callback parameters for BLE sleep */
#if(CY_BLE_STACK_MODE_HOST_IPC)
  static cy_stc_syspm_callback_params_t bleSleepCallbackParams = { .base = NULL};
  static cy_stc_syspm_callback_t bleSleepCallback =
  {
    .callback = &Cy_BLE_SleepCallback,
    .type = CY_SYSPM_SLEEP,
    .skipMode = CY_SYSPM_SKIP_BEFORE_TRANSITION | CY_SYSPM_SKIP_CHECK_FAIL,
    .callbackParams = &bleSleepCallbackParams
  };
#endif /* CY_BLE_STACK_MODE_HOST_IPC */
#endif  /* (CY_BLE_USE_DEEP_SLEEP != 0u) */

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_MODE_PROFILE)
/** BLE interrupt notification structure */
cy_stc_ble_intr_notify_t intrNotify = 
        {
            .clientID = CY_BLE_CYPIPE_MSG_INTR_NOTIFY_ID,
            .pktType  = 0xFFu
        };
#endif /*(CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_MODE_PROFILE) */


/******************************************************************************
* Function Name: Cy_BLE_UnregisterHostPMCallbacks
***************************************************************************//**
*
*  Unregister BLE callback for Deepsleep/Sleep
*
******************************************************************************/
void Cy_BLE_UnregisterHostPMCallbacks(void)
{
    #if (CY_BLE_USE_DEEP_SLEEP != 0u)
        /* Unregister BLE callback for Deepsleep support */
        (void)Cy_SysPm_UnregisterCallback(&bleDeepSleepCallback);
        
        /* Unregister sleep mode callback only in dual core mode for the host core */
        #if(CY_BLE_STACK_MODE_HOST_IPC)
            /* Unregister BLE callback for Sleep support */
            (void)Cy_SysPm_UnregisterCallback(&bleSleepCallback);
        #endif /* ( CY_BLE_STACK_MODE_HOST_IPC) */
    #endif /* (CY_BLE_USE_DEEP_SLEEP != 0u) */
}

/**
 * \addtogroup group_ble_common_api_functions
 * @{
 */

/******************************************************************************
* Function Name: Cy_BLE_Init
***************************************************************************//**
*
*  Initializes the BLE Middleware.
*  
*
*  \param config: The configuration structure for the BLE Middleware.
*
* \return
* \ref cy_en_ble_api_result_t : Return value indicates if the function succeeded or
*  failed. The following are possible error codes.
*
*   Error codes                     | Description
*   ------------                    | -----------
*   CY_BLE_SUCCESS                  | The function completed successfully.
*   CY_BLE_ERROR_INVALID_PARAMETER  | On specifying NULL as the input parameter.
*
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Init(cy_stc_ble_config_t *config)
{
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;

    if(config == NULL)
    {
        apiResult = CY_BLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Register a pointer to the config structure */
        cy_ble_configPtr = config;

    #if (CY_BLE_MODE_PROFILE)
        /* Register a callback function via the config structure */
        if(cy_ble_configPtr->callbackFunc != NULL)
        {
            Cy_BLE_ApplCallback = cy_ble_configPtr->callbackFunc;
        }
        cy_ble_eventHandlerFlag = 0u;
        (void)memset((uint8_t*)&cy_ble_busyStatus, 0, sizeof(cy_ble_busyStatus));

        cy_ble_cmdStatus = 0u;
        
        #if (CY_BLE_GATT_ROLE_CLIENT)
        {
            uint32_t idx;
            (void)memset(&cy_ble_serverInfo, 0, sizeof(cy_ble_serverInfo));
            (void)memset(cy_ble_connHandle, -1, sizeof(cy_ble_connHandle));
            (void)memset(&cy_ble_discovery, 0, sizeof(cy_ble_discovery));

            /* Update cy_ble_discovery[].connIndex with init values (CY_BLE_INVALID_CONN_HANDLE_VALUE) */
            for(idx = 0u; idx < CY_BLE_GATTC_COUNT; idx++)
            {
                cy_ble_discovery[idx].connIndex = CY_BLE_INVALID_CONN_HANDLE_VALUE;
            }
        }
        #endif /* CY_BLE_GATT_ROLE_CLIENT */

        #if (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
        cy_ble_advertisingIntervalType = CY_BLE_ADVERTISING_FAST;
        #endif /* (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)  */

        #if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
        cy_ble_scanningIntervalType = CY_BLE_SCANNING_FAST;
        #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER) */

        Cy_BLE_ServiceInit();

        #if ((CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_CENTRAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))
        cy_ble_pendingFlashWrite = 0u;
        (void)memset(cy_ble_peerBonding, (int8_t)CY_BLE_GAP_BONDING_NONE, sizeof(cy_ble_peerBonding));
        #endif /* (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES) */
    #endif     /* (CY_BLE_MODE_PROFILE) */
    }
    
#ifndef CY_DIE_PSOC6ABLE2 
    #if(CY_BLE_STACK_MODE_IPC)
        Cy_IPC_SystemPipeInit();
    #endif /* (CY_BLE_STACK_MODE_IPC) */
#endif /*(CY_DIE_PSOC6ABLE2) */
        
    return(apiResult);
}
/** @} */

/******************************************************************************
* Function Name: Cy_BLE_Enable
***************************************************************************//**
*
*  Initializes and starts the BLE Stack.
*
*  Calling this function results in generation of a #CY_BLE_EVT_STACK_ON event
*  on successful initialization of the BLE Stack.
*
*  This function initializes the BLE Stack which consists of the BLE Stack 
*  Manager, BLE Controller, and BLE Host modules. It takes care of initializing
*  the Profile layer, schedulers, Timer and other platform-related resources
*  required for the BLE Middleware. It also registers the callback function for
*  BLE events that will be registered in the BLE stack.
*
*  Note that this function does not reset the BLE Stack.
* 
*  For the HCI-Mode of operation, this function will not initialize the BLE Host 
*  module.
*
*  Calling this function results in generation of a #CY_BLE_EVT_STACK_ON event
*  on successful initialization of the BLE Stack.
*
*  In the Dual Core mode, this function should be called on both cores in the 
*  following sequence:
*   - call this function on CM0+ core to initialize the Controller.
*   - start CM4 core by calling Cy_SysEnableCM4() function.
*   - call this function on CM4 core to initialize the Host and Profiles.
*
* \return
* \ref cy_en_ble_api_result_t : Return value indicates if the function succeeded
*      or failed. The following are possible error codes.
*
*  <table>
*    <tr>
*      <th>Error codes</th>
*      <th>Description</th>
*    </tr>
*    <tr>
*      <td>CY_BLE_SUCCESS</td>
*      <td>On successful operation.</td>
*    </tr>
*    <tr>
*      <td>CY_BLE_ERROR_REPEATED_ATTEMPTS</td>
*      <td>On invoking this function more than once without calling 
*          Cy_BLE_Disable() function between calls to this function.</td>
*    </tr>
*    <tr>
*      <td>CY_BLE_ERROR_MEMORY_ALLOCATION_FAILED</td>
*      <td>There is insufficient memory available.</td>
*    </tr>
*  </table>
*
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Enable(void)
{
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;

#if (CY_BLE_MODE_PROFILE)
    cy_stc_ble_stk_app_data_buff_t cy_ble_stackDataBuff[CY_BLE_STACK_APP_MIN_POOL] =
    {
        { CY_BLE_GATT_MTU_PLUS_L2CAP_MEM_EXT,   CY_BLE_GATT_MTU_BUFF_COUNT              },
        { CY_BLE_L2CAP_PSM_PLUS_L2CAP_MEM_EXT,  CY_BLE_L2CAP_PSM_COUNT                  },
        { CY_BLE_L2CAP_CBFC_PLUS_L2CAP_MEM_EXT, 2u * CY_BLE_L2CAP_LOGICAL_CHANNEL_COUNT },
        { CY_BLE_L2CAP_MTU_PLUS_L2CAP_MEM_EXT,  2u * CY_BLE_L2CAP_LOGICAL_CHANNEL_COUNT }
    };
#endif  /* (CY_BLE_MODE_PROFILE) */
    
    cy_stc_ble_stack_init_info_t stackInitParam =
    {
    #if (CY_BLE_MODE_PROFILE)
        .CyBleAppCbFunc                      = (cy_ble_app_ev_cb_t)&Cy_BLE_EventHandler,
    #endif /* CY_BLE_MODE_PROFILE */
        .memParam                            =
        {
    #if (CY_BLE_MODE_PROFILE)
            .totalHeapSz                     = (uint16_t)(CY_BLE_STACK_RAM_SIZE - CY_BLE_GATT_PREPARE_WRITE_BUFF_LEN),
            .dataBuff                        = cy_ble_stackDataBuff,
            .totalDataBufferPools            = CY_BLE_STACK_APP_MIN_POOL,

        #if (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES)
            .bleStackFlashPointer            = (const uint8_t*)cy_ble_configPtr->flashStorage->stackFlashptr,
            .bleStackFlashSize               = CY_BLE_STACK_FLASH_SIZE,
        #endif  /* CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES */
    #else
            .totalHeapSz                     = CY_BLE_STACK_RAM_SIZE,
    #endif /* CY_BLE_MODE_PROFILE */
        },
        .stackConfig                         =
        {
            /* Configure DLE */
            .dleConfig.dleMaxTxCapability    = CY_BLE_LL_MAX_TX_PAYLOAD_SIZE,
            .dleConfig.dleMaxRxCapability    = CY_BLE_LL_MAX_RX_PAYLOAD_SIZE,
            .dleConfig.dleNumTxBuffer        = CY_BLE_LL_DEFAULT_NUM_ACL_TX_PACKETS,
            .dleConfig.dleNumRxBuffer        = CY_BLE_LL_DEFAULT_NUM_ACL_RX_PACKETS,

        #if (CY_BLE_MODE_PROFILE)
            /* Configure queue */
            .l2capConfig.l2capBufferPerConn  = CY_BLE_L2CAP_Q_DEPTH_PER_CONN,
        #endif /* (CY_BLE_MODE_PROFILE) */

            /* Configure LL Privacy */
            .privacyConfig.resolvingListSize = CY_BLE_MAX_RESOLVABLE_DEVICES,

            /* Configure bonded device list */
            .bondListConfig.bondListSize     = CY_BLE_MAX_BONDED_DEVICES,

            /* Configure white list */
            .whiteListConfig.whiteListSize   = CY_BLE_MAX_WHITE_LIST_SIZE,

            .featureMask                     = (CY_BLE_DLE_FEATURE | CY_BLE_LL_PRIVACY_FEATURE | 
                                                CY_BLE_SECURE_CONN_FEATURE | CY_BLE_PHY_UPDATE_FEATURE |
                                                CY_BLE_STORE_BONDLIST_FEATURE | CY_BLE_STORE_RESOLVING_LIST_FEATURE |
                                                CY_BLE_STORE_WHITELIST_FEATURE | CY_BLE_TX_POWER_CALIBRATION_FEATURE),
            .maxBleConnections               = CY_BLE_CONN_COUNT
        }
    };

#if (CY_BLE_USE_DEEP_SLEEP != 0u)
    /* Register BLE callback for Deepsleep support */
    (void)Cy_SysPm_RegisterCallback(&bleDeepSleepCallback);
    
    /* Register Sleep mode callback only in dual core mode for the host core */
    #if(CY_BLE_STACK_MODE_HOST_IPC)
    {
        /* Register BLE callback for Sleep support */
        (void)Cy_SysPm_RegisterCallback(&bleSleepCallback);
    }
    #endif /* (CY_BLE_STACK_MODE_HOST_IPC) */
#endif  /* (CY_BLE_USE_DEEP_SLEEP != 0u) */
    
#if(CY_BLE_SHARING_MODE_EXPORT)
    if(cy_ble_stackMemoryRam == NULL)
    {
        cy_ble_stackMemoryRam = (uint8 *)malloc(CY_BLE_STACK_RAM_SIZE);
        if(cy_ble_stackMemoryRam == NULL)
        {
            apiResult = CY_BLE_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    }
#endif /* CY_BLE_SHARING_MODE_EXPORT */
    
    if(apiResult == CY_BLE_SUCCESS)
    {
    #if (CY_BLE_DLE_FEATURE_ENABLED)
        Cy_BLE_EnableDleFeature();              /* Enable DLE code in the stack */
    #endif /* CY_BLE_DLE_FEATURE_ENABLED */

    #if (CY_BLE_LL_PRIVACY_FEATURE_ENABLED)
        Cy_BLE_EnablePrivacyFeature();          /* Enable LL Privacy code in the stack */
    #endif /* CY_BLE_LL_PRIVACY_FEATURE_ENABLED */

    #if (CY_BLE_PHY_UPDATE_FEATURE_ENABLED)
        Cy_BLE_EnablePhyUpdateFeature();        /* Enable PHY Update code in the stack */
    #endif /* CY_BLE_PHY_UPDATE_FEATURE_ENABLED */

        apiResult = Cy_BLE_StackSetFeatureConfig(&stackInitParam.stackConfig);
    }
    
    if(apiResult == CY_BLE_SUCCESS)
    {
        stackInitParam.memParam.memoryHeapPtr = cy_ble_stackMemoryRam;

        /* Initialize the BLE Stack */
        apiResult = Cy_BLE_StackInit(&stackInitParam);

    #if (CY_BLE_MODE_PROFILE)
        if(apiResult == CY_BLE_SUCCESS)
        {
            Cy_BLE_SetState(CY_BLE_STATE_INITIALIZING);

        #if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
            (void)memset(&cy_ble_connState, 0, sizeof(cy_ble_connState));
        #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) */

        #if (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
            Cy_BLE_SetAdvertisementState(CY_BLE_ADV_STATE_STOPPED);
        #endif  /* (CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER) */

        #if (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
            Cy_BLE_SetScanState(CY_BLE_SCAN_STATE_STOPPED);
        #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER) */

        #if (CY_BLE_GATT_ROLE_SERVER)
            {
                cy_stc_ble_gatts_db_info_t dbInfoParam =
                {
                    .gattDbPtr          = (const cy_stc_ble_gatts_db_t*)cy_ble_configPtr->gattDB,
                    .gattDbTotalEntries = cy_ble_configPtr->params->gattDbIndexCount,
                    .gattDbMaxValue     = CY_BLE_GATT_DB_MAX_VALUE_LEN
                };

                apiResult = Cy_BLE_GATTS_DbRegister(&dbInfoParam);
            }
        #endif /* CY_BLE_GATT_ROLE_SERVER */
        }
        cy_ble_cmdStatus = 0u;
    #endif /* (CY_BLE_MODE_PROFILE) */
    }

    return(apiResult);
}


/******************************************************************************
* Function Name: Cy_BLE_Disable
***************************************************************************//**
*
*  This function stops any ongoing operation in the BLE Stack and forces the
*  BLE Stack to shut down.
*
*  Calling this function results in generation of a 
*  #CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE event on a successful stack shutdown.
*
*  For HCI mode:
*  This is a blocking function and no event is generated.
*  Only #CY_BLE_SUCCESS will be returned and other error codes are not applicable.
*  UART interface will be stopped and UART data will not be processed by stack
*  until Cy_BLE_Enable() is invoked.
*
* \return
* \ref cy_en_ble_api_result_t : Return value indicates if the function succeeded or
*  failed. The following are possible error codes.
*
*  <table>
*    <tr>
*      <th>Error codes</th>
*      <th>Description</th>
*    </tr>
*    <tr>
*      <td>CY_BLE_SUCCESS</td>
*      <td>On successful operation.</td>
*    </tr>
*    <tr>
*      <td>CY_BLE_ERROR_INVALID_OPERATION</td>
*      <td>On calling Cy_BLE_Disable before calling Cy_BLE_Enable() 
*          or on Controller core.</td>
*    </tr>
*  </table>
*
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Disable(void)
{
    cy_en_ble_api_result_t apiResult;

#if (CY_BLE_STACK_IPC_CONTR_CORE)
    /* To stop Controller call Cy_BLE_Disable() function on Host core */
    apiResult = CY_BLE_SUCCESS;             
#else
    /* Stops all ongoing activities */
    apiResult = Cy_BLE_StackShutdown();
    
    #if (CY_BLE_SHARING_MODE_EXPORT)
    if(cy_ble_stackMemoryRam != NULL)
    {
        free(cy_ble_stackMemoryRam);
    }
    #endif /* CY_BLE_SHARING_MODE_EXPORT */
#endif     /* CY_BLE_STACK_IPC_CONTR_CORE */

    return(apiResult);
}


/*******************************************************************************
* Function Name: Cy_BLE_ConfigureExtPA
****************************************************************************//**
*
*  This function controls the external Power Amplifier and Low Noise Amplifier
*  outputs.
*
* \param controlValue Enables and controls polarity of the outputs.
*   To control external PA and LNA, the following mask defines are used:
*
*   - BLE_BLESS_EXT_PA_LNA_CTRL_ENABLE_EXT_PA_LNA_Msk - When set to 1, enables
*       the external PA & LNA and BLESS power profile for energy profiler
*   - BLE_BLESS_EXT_PA_LNA_CTRL_CHIP_EN_POL_Msk - Controls the polarity of the 
*       chip-enable control signal:
*       * 0 - High enable, Low disable
*       * 1 - Low enable, High disable, Incorrect power profile
*   - BLE_BLESS_EXT_PA_LNA_CTRL_PA_CTRL_POL_Msk - Controls the polarity of the PA
*       control signal:
*       * 0 - High enable, Low disable
*       * 1 - Low enable, High disable, Incorrect power profile
*   - BLE_BLESS_EXT_PA_LNA_CTRL_LNA_CTRL_POL_Msk - Controls the polarity of the LNA
*       control signal:
*       * 0 - High enable, Low disable
*       * 1 - Low enable, High disable, Incorrect power profile
*   - BLE_BLESS_EXT_PA_LNA_CTRL_OUT_EN_DRIVE_VAL_Msk  - Configures the drive value
*       on the output enables of PA, LNA and CHI_EN signals
*       * 0 - drive 0 on the output enable signals
*       * 1 - drive 1 on the output enable signals
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_ConfigureExtPA(uint32_t controlValue)
{
    BLE->BLESS.EXT_PA_LNA_CTRL = controlValue;
}

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_HOST_CORE)
/*******************************************************************************
* Function Name: Cy_BLE_RegisterInterruptCallback
****************************************************************************//**
*
*  This function registers a callback to expose BLE interrupt notifications to an 
*  application that indicates a different link layer and radio state transition
*  to the user from the BLESS interrupt context. This callback is triggered at 
*  the beginning of a received BLESS interrupt (based on the registered 
*  interrupt mask).
*
*  An application can use an interrupt callback to know when: 
*  * the RF activity is about to begin/end; 
*  * the BLE device changes its state from advertising to connected; 
*  * BLESS transits between active and low-power modes.
*    
*  These BLESS real-time states can be used to synchronize
*  an application with the BLESS or prevent radio interference with other 
*  peripherals, etc.
*    
*  BLE interrupt notifications feature enables via define in cy_ble_config.h:
*  \code   
*  #define CY_BLE_INTR_NOTIFY_FEATURE_ENABLE       (1u)
*  \endcode    
*
*  BLE Dual mode requires an additional define IPC channel and IPC Interrupt 
*  structure to send notifications from the controller core to host core. 
*  Use the following defines:
*  \code    
*  #define CY_BLE_INTR_NOTIFY_IPC_CHAN             (9..15)
*  #define CY_BLE_INTR_NOTIFY_IPC_INTR             (9..15)
*  #define CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR       (0..7)  
*  \endcode
*    
*  \param intrMask
*  All interrupts masks are specified in the #cy_en_ble_interrupt_callback_feature_t
*  enumeration.
*    
*  \param CallBack
*  The pointer to an application notify callback.
*
* \return
* \ref cy_en_ble_api_result_t : The return value indicates if the function succeeded or
*  failed. The possible error codes:
*
*    Errors codes                   | Description
*    ------------                   | -----------
*    CY_BLE_SUCCESS                 | The callback is registered successfully.
*    CY_BLE_ERROR_INVALID_PARAMETER | Validation of the input parameters failed.
*    CY_BLE_ERROR_INVALID_OPERATION | The IPC channel is busy (Dual core mode only).
*
*******************************************************************************/   
cy_en_ble_api_result_t Cy_BLE_RegisterInterruptCallback(uint32_t intrMask, cy_ble_intr_callback_t CallBack)
{ 
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
    
    if(CallBack != NULL)
    {
        /* Store the application callback and interrupt mask */
        Cy_BLE_InterruptCallback = CallBack;
        intrNotify.mask = intrMask;
        
        /* Set up the IPC for Dual Stack mode */
        #if(CY_BLE_STACK_MODE_HOST_IPC)
        {  
            if(!intrNotify.configFlag)
            {
                uint32_t rTimeout = 2000u; /* us */
                cy_en_ipc_pipe_status_t ipcStatus;
                cy_stc_sysint_t intrConfig =
                {
                    .intrPriority = (uint32_t)CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR,
                    .intrSrc      = (IRQn_Type)CY_IPC_INTR_NUM_TO_VECT((int32_t)CY_BLE_INTR_NOTIFY_IPC_INTR)
                };
            
                /* Initialize the interrupt controller for CM4 and IPC Interrupt */
                (void)Cy_SysInt_Init(&intrConfig, &Cy_BLE_HAL_IntrNotifyIpcHandler);
                NVIC_EnableIRQ(intrConfig.intrSrc);
        
                /* Do not bring up an IPC release interrupt here, only set up a notify interrupt */
                Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_BLE_INTR_NOTIFY_IPC_INTR),
                                            CY_IPC_NO_NOTIFICATION, (1uL << CY_BLE_INTR_NOTIFY_IPC_CHAN));
                
                /* 
                 * Send the address of the intrNotify structure to the controller core via CyPipe
                 * The IPC Pipe can be busy, then try sending a message during 2 ms. 
                 */
                do
                {
                    ipcStatus = Cy_IPC_Pipe_SendMessage(CY_BLE_IPC_CONTROLLER_ADDR, CY_BLE_IPC_HOST_ADDR,
                                                        (void *)&intrNotify, NULL);
                    Cy_SysLib_DelayUs(1u);
                    rTimeout--;
                }while((ipcStatus != CY_IPC_PIPE_SUCCESS) && (rTimeout != 0u));
              
                if(ipcStatus != CY_IPC_PIPE_SUCCESS)
                {
                    apiResult = CY_BLE_ERROR_INVALID_OPERATION;
                }
            }
        }
        #else 
            intrNotifyPtr = &intrNotify; 
        #endif /* CY_BLE_STACK_MODE_HOST_IPC */
       
    }
    else
    {
        apiResult = CY_BLE_ERROR_INVALID_PARAMETER;
    }
    return apiResult; 
}


/*******************************************************************************
* Function Name: Cy_BLE_UnRegisterInterruptCallback
****************************************************************************//**
*
*  This function un-registers callback which exposed BLE interrupt 
*  notifications to the application. 
*
* \return
* \ref cy_en_ble_api_result_t : Return value indicates if the function succeeded or
*  failed. The following are possible error codes.
*    
*    Errors codes                   | Description
*    ------------                   | -----------
*    CY_BLE_SUCCESS                 | The callback is registered successfully.
*    CY_BLE_ERROR_INVALID_OPERATION | The IPC channel is busy (Dual core mode only).
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_UnRegisterInterruptCallback(void)
{
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
    if(Cy_BLE_InterruptCallback != NULL)
    {
        /* Clean interrupt mask */
        apiResult = Cy_BLE_RegisterInterruptCallback((uint32_t)CY_BLE_INTR_CALLBACK_NONE, 
                                                     Cy_BLE_InterruptCallback);
        if(apiResult == CY_BLE_SUCCESS)
        {
            /* Clean callback pointer */
            Cy_BLE_InterruptCallback = NULL;
        }
    }
    return(apiResult);    
}

#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_HOST_CORE) */


#if (CY_BLE_MODE_PROFILE)

/*******************************************************************************
* Function Name: Cy_BLE_RegisterAppHostCallback
****************************************************************************//**
*
* This function registers an application Host callback (BLE RTOS hook). 
* The user can trigger an RTOS BLE task if the application Host callback was
* called.
* 
* Theory of operation:
* The BLE Middleware triggers this callback when the BLE Stack controller needs
* to process pending Stack events (by calling Cy_BLE_ProcessEvents()). This
* callback executes from within an ISR and must be very short.  
*    
* \note The application Host callback is called from the context of a high-
* priority ISR. Some RTOS (e.g. FreeRTOS) have restrictions on calling the RTOS
* functions from a high-priority ISR. The user application is responsible for
* integration of the BLE RTOS hook and RTOS functions and must insure that RTOS 
* allows to call RTOS API from BLE interrupt context.  
* If direct call is forbidden one of the possible solutions is to trig a 
* low-priority interrupt from the application Host callback and then call the
* RTOS functions from a low-priority interrupt. For details refer to specific
* RTOS documentation.
*   
* \param CallBack
* The pointer to the application Host callback.
*
* \return
*  Error Codes                        | Description
*  ------------                       | -----------
*   CY_BLE_SUCCESS                    | The callback was registered successfully.
*   CY_BLE_ERROR_INVALID_PARAMETER    | On specifying NULL as the input parameter.
*
*******************************************************************************/

cy_en_ble_api_result_t Cy_BLE_RegisterAppHostCallback(cy_ble_app_notify_callback_t CallBack)
{
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
    
    if(CallBack != NULL)
    {
        Cy_BLE_HostRegisteredCallback = CallBack;
    }
    else
    {
        apiResult = CY_BLE_ERROR_INVALID_PARAMETER;
    }   
    return apiResult;
}

/*******************************************************************************
* Function Name: Cy_BLE_UnRegisterAppHostCallback
****************************************************************************//**
*
* This function un-registers an application Host callback (BLE RTOS hook). 
*
*******************************************************************************/
void Cy_BLE_UnRegisterAppHostCallback(void)
{
    Cy_BLE_HostRegisteredCallback = NULL;
}
#endif /* (CY_BLE_MODE_PROFILE) */


#if (CY_BLE_USE_DEEP_SLEEP != 0u)
/*******************************************************************************
* Function Name: Cy_BLE_DeepSleepCallback
****************************************************************************//**
*
* This function requests the BLE Stack to put Bluetooth Low Energy Sub-System
* (BLESS) to DeepSleep mode.
*
* It is registered to the system power mode by Cy_SysPm_RegisterCallback()
* function with CY_SYSPM_DEEPSLEEP type. After registration it is called by
* Cy_SysPm_DeepSleep() function prior entering the core into the Deep Sleep
* power mode.
*
* When it is called with the enMode parameter set to CY_SYSPM_CHECK_READY,
* function enters BLESS to CY_BLE_BLESS_DEEPSLEEP mode. It also checks the
* BLE Subsystem's current operational mode. When the
* Cy_BLE_StackGetBleSsState() function returns CY_BLE_BLESS_STATE_ECO_ON or
* CY_BLE_BLESS_STATE_DEEPSLEEP state, this function returns CY_SYSPM_PASS and
* allows to put the core into the Deep Sleep power mode.
* At all other times, the function tries to enter core into sleep mode.
*
* This function is available only when BLE low power mode is enabled in the
* Advanced tab of customizer.
*
* \param
* callbackParams  pointer to the structure with the syspm callback parameters
*                 callbackParams->mode - the callback mode, see description 
*                 of the cy_en_syspm_callback_mode_t type.
*
* \return
* * CY_SYSPM_SUCCESS  - Entered and exited from DeepSleep
* * CY_SYSPM_FAIL     - DeepSleep not entered.
*
*******************************************************************************/
cy_en_syspm_status_t Cy_BLE_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    cy_en_syspm_status_t retVal;
    static uint32_t interruptState = 0u;
    cy_en_ble_bless_state_t blessState;
    
    /* Local variable to store the status of BLESS Hardware block */
    cy_en_ble_lp_mode_t sleepMode;

    switch(callbackParams->mode)
    {
        case (CY_SYSPM_CHECK_READY):
        #if (CY_BLE_MODE_PROFILE)
            if(Cy_BLE_GetState() == CY_BLE_STATE_INITIALIZING)
            {
                retVal = CY_SYSPM_FAIL;
            }
            else
        #endif  /*(CY_BLE_MODE_PROFILE) */
            
            if(Cy_IPC_Sema_Status(CY_BLE_SEMA) == CY_IPC_SEMA_STATUS_LOCKED)
            {
                /* System do not enter deepsleep if BLE Host start write operation */
                retVal = CY_SYSPM_FAIL;
            }
            else

        #if(CY_BLE_STACK_CONTR_CORE)       
            if (Cy_BLE_HAL_IsEcoCpuClockSrc() == 1u)
            {            
                /* System never enters deepsleep if BLE ECO is CPU source */
                retVal = CY_SYSPM_FAIL;
            }
            else
        #endif /* (CY_BLE_STACK_CONTR_CORE) */
            {        
        #if(!CY_BLE_STACK_MODE_HOST_IPC)
                cy_en_ble_api_result_t retIsControllerActive;
        #endif /* !CY_BLE_STACK_MODE_HOST_IPC */   
        
        #if(CY_BLE_STACK_MODE_HOST_IPC)
                /* Disable global interrupt to prevent changes from any other interrupt ISR */
                interruptState = Cy_SysLib_EnterCriticalSection();
        #endif /* CY_BLE_STACK_MODE_HOST_IPC */
               
                /* Put BLESS into Deep Sleep and check the return status */
                sleepMode = Cy_BLE_StackEnterLPM(CY_BLE_BLESS_DEEPSLEEP);

        #if(!CY_BLE_STACK_MODE_HOST_IPC)
                /* Disable global interrupt to prevent changes from any other interrupt ISR */
                interruptState = Cy_SysLib_EnterCriticalSection();
        #endif /* !CY_BLE_STACK_MODE_HOST_IPC */

                /* Check the Status of BLESS */
                blessState = Cy_BLE_StackGetBleSsState();

                #if(!CY_BLE_STACK_MODE_HOST_IPC)
                if(blessState == CY_BLE_BLESS_STATE_STOPPED)
                {
                    retVal = CY_SYSPM_SUCCESS;
                }
                else
                #endif /* (!CY_BLE_STACK_MODE_HOST_IPC)*/
                if(sleepMode == CY_BLE_BLESS_DEEPSLEEP)
                {

                    #if(!CY_BLE_STACK_MODE_HOST_IPC)
                    retIsControllerActive = Cy_BLE_IsControllerActive(CY_BLE_CONTROLLER_SLEEP_MODE_DEEPSLEEP);
                    #endif /* !CY_BLE_STACK_MODE_HOST_IPC */

                    if(((blessState == CY_BLE_BLESS_STATE_ECO_ON) || (blessState == CY_BLE_BLESS_STATE_DEEPSLEEP))
                    #if(!CY_BLE_STACK_MODE_HOST_IPC)
                        && (retIsControllerActive == CY_BLE_SUCCESS)
                    #endif /* !CY_BLE_STACK_MODE_HOST_IPC */                    
                        )
                    {
                        /* Enter device deep sleep */
                        retVal = CY_SYSPM_SUCCESS;
                    }
                    else
                    {
                        /* The BLESS hardware block cannot go to Deep Sleep, try sleep mode */
                        retVal = CY_SYSPM_FAIL;
                    }
                }
                else
                {
                #if(!CY_BLE_STACK_MODE_HOST_IPC)
                    retIsControllerActive = Cy_BLE_IsControllerActive(CY_BLE_CONTROLLER_SLEEP_MODE_SLEEP);

                    if((blessState != CY_BLE_BLESS_STATE_EVENT_CLOSE) && 
                       (retIsControllerActive == CY_BLE_SUCCESS))
                    {
                        /* If the BLESS hardware block cannot go to Deep Sleep and BLE Event has not
                         * closed yet, then place CPU to Sleep */
                        (void)Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
                    }
                #endif /* !CY_BLE_STACK_MODE_HOST_IPC */
                    retVal = CY_SYSPM_FAIL;
                }
                if(retVal == CY_SYSPM_FAIL)
                {
                    /* Enable interrupts after failing check */
                    Cy_SysLib_ExitCriticalSection(interruptState);
                }
            }
            break;

        /* Enable interrupts after wakeup */
        case (CY_SYSPM_AFTER_TRANSITION):
            Cy_SysLib_ExitCriticalSection(interruptState);
            retVal = CY_SYSPM_SUCCESS;
            break;

        default:
            retVal = CY_SYSPM_FAIL;
            break;
    }

    return(retVal);
}

#if(CY_BLE_STACK_MODE_HOST_IPC)
/*******************************************************************************
* Function Name: Cy_BLE_SleepCallback
****************************************************************************//**
*
* This function requests the BLE Stack to put the Host to Sleep mode.
*
* It is registered to the system power mode by Cy_SysPm_RegisterCallback()
* function with CY_SYSPM_SLEEP type. After registration it is called by
* Cy_SysPm_Sleep() function prior entering the core into the Sleep
* power mode.
*
* This function is available only when BLE low power mode is enabled in the
* Advanced tab of customizer.
*
* \param
* callbackParams  pointer to the structure with the syspm callback parameters
*                 callbackParams->mode - the callback mode, see description 
*                 of the cy_en_syspm_callback_mode_t type.
*
* \return
* * CY_SYSPM_SUCCESS  - Entered and exited from Sleep
* * CY_SYSPM_FAIL     - Sleep not entered.
*
*******************************************************************************/
cy_en_syspm_status_t Cy_BLE_SleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    cy_en_syspm_status_t retVal;
    static uint32_t interruptState = 0u;
    /* Local variable to store the status of BLESS Hardware block */
    cy_en_ble_lp_mode_t sleepMode;
    
    switch(callbackParams->mode)
    {
        case (CY_SYSPM_CHECK_READY):
            if(Cy_BLE_GetState() == CY_BLE_STATE_INITIALIZING)
            {
                retVal = CY_SYSPM_FAIL;
            }
            else
            {
                /* Disable global interrupt to prevent changes from any other interrupt ISR */
                interruptState = Cy_SysLib_EnterCriticalSection();
                
                /* Put BLESS into Deep Sleep and check the return status */
                sleepMode = Cy_BLE_StackEnterLPM(CY_BLE_BLESS_DEEPSLEEP);
                
                /* Check if host is ready for sleep mode */
                if(sleepMode != CY_BLE_BLESS_DEEPSLEEP) 
                {
                    retVal = CY_SYSPM_FAIL;
                    /* Enable interrupts after failing check */
                    Cy_SysLib_ExitCriticalSection(interruptState);
                }
                else
                {
                    retVal = CY_SYSPM_SUCCESS;
                }
            }
            break;

        /* Enable interrupts after wakeup */
        case (CY_SYSPM_AFTER_TRANSITION):
            Cy_SysLib_ExitCriticalSection(interruptState);
            retVal = CY_SYSPM_SUCCESS;
            break;

        case (CY_SYSPM_CHECK_FAIL):
            Cy_SysLib_ExitCriticalSection(interruptState);
            retVal = CY_SYSPM_FAIL;
            break;
            
        default:
            retVal = CY_SYSPM_FAIL;
            break;
    }

    return(retVal);
}

#endif /* (CY_BLE_STACK_MODE_HOST_IPC) */
#endif /* (CY_BLE_USE_DEEP_SLEEP != 0u) */
#endif /* (CY_BLE_HOST_CONTR_CORE) */


/**
 * Print warnings
 */

/* Inform that BLE Interrupt Notification feature uses the default IPC CHAN/INTR in Dual mode. */
#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_HOST_CORE) && (CY_BLE_STACK_MODE_IPC)
    #if ( (defined (CY_BLE_INTR_NOTIFY_IPC_CHAN_WARNING)) || \
          (defined (CY_BLE_INTR_NOTIFY_IPC_INTR_WARNING)) || \
          (defined (CY_BLE_INTR_NOTIFY_IPC_CHAN_WARNING)))
        #warning Interrupt Notification feature in BLE Dual mode requires an additional \
                 define IPC channel and IPC Interrupt structure to send notification    \
                 from the controller core to host core.                                 \
                 Use the following defines in cy_ble_config.h:                          \
                 CY_BLE_INTR_NOTIFY_IPC_CHAN, CY_BLE_INTR_NOTIFY_IPC_INTR,              \
                 CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR
    #endif /* defined (CY_BLE_INTR_NOTIFY_*****_WARNING)*/
 
    #if defined (CY_BLE_INTR_NOTIFY_IPC_CHAN_WARNING)
        #warning CY_BLE_INTR_NOTIFY_IPC_CHAN not defined in in cy_ble_config.h file \
                 Using default value 15!                   
    #endif /* !defined (CY_BLE_INTR_NOTIFY_IPC_CHAN_WARNING) */

    #if defined (CY_BLE_INTR_NOTIFY_IPC_INTR_WARNING)
        #warning CY_BLE_INTR_NOTIFY_IPC_INTR not defined in in cy_ble_config.h file \
                 Using default value 15!                   
    #endif /* !defined (CY_BLE_INTR_NOTIFY_IPC_INTR_WARNING) */

    #if defined (CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR_WARNING)
        #warning CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR not defined in in cy_ble_config.h file \
                 Using default value 1! 
    #endif /* !defined (CY_BLE_INTR_NOTIFY_IPC_INTR_PRIOR_WARNING) */
#endif /*(CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_HOST_CORE) && (CY_BLE_STACK_MODE_IPC */
#ifdef __cplusplus
}
#endif /* __cplusplus */


/* [] END OF FILE */
