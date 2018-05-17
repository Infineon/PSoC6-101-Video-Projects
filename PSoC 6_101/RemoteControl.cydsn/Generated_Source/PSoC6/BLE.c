/***************************************************************************//**
* \file CY_BLE.c
* \version 2.0
* 
* \brief
*  This file contains the source code for the Common APIs of the BLE Component.
* 
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ble/cy_ble_event_handler.h"
#include "ble/cy_ble_hal_pvt.h"
#include "ble/cy_ble.h"
#include "BLE.h"
#include "BLE_config.h"
#include <stdlib.h>
#include "cyfitter_sysint.h"
#include "cyfitter_sysint_cfg.h"

#if(CY_BLE_CONFIG_STACK_MODE_CONTR_UART) /* HCI mode */ 
#include "BLE_1_HAL_Uart.h"
#endif  /* (CY_BLE_CONFIG_STACK_MODE_CONTR_UART) */   

#if(CY_BLE_CONFIG_STACK_MODE_HOST_UART)
#include "BLE_1_Host_Uart.h"
#endif /* CY_BLE_CONFIG_STACK_MODE_HOST_UART */

#if(CY_BLE_HOST_CONTR_CORE)
    
/***************************************
* Global Variables
***************************************/

/**
 \addtogroup group_globals
 @{
*/

/** Indicates whether the BLE has been initialized. The variable is initialized to
   0 and set to 1 the first time Cy_BLE_Start() is called. This allows the component
   to restart without reinitialization after the first call to the Cy_BLE_Start() 
   routine. If reinitialization of the component is required, the variable should 
   be set to 0 before the Cy_BLE_Start() routine is called. Alternatively, the BLE
   can be reinitialized by calling the Cy_BLE_Init() function.
*/
uint8_t  cy_ble_initVar = 0u;

/** @} group_globals */
    
/******************************************************************************
* Function Name: Cy_BLE_Start
***************************************************************************//**
* 
*  This function initializes the BLE Stack which consists of the BLE Stack 
*  Manager, BLE Controller, and BLE Host modules. It takes care of initializing
*  the Profile layer, schedulers, Timer and other platform-related
*  resources required for the BLE component. It also registers the callback 
*  function for BLE events that will be registered in the BLE stack.
* 
*  Note that this function does not reset the BLE Stack.
* 
*  For the HCI-Mode of operation, this function will not initialize the BLE Host 
*  module.
* 
*  Calling this function results in generation of a CY_BLE_EVT_STACK_ON event
*  on successful initialization of the BLE Stack.
*
*  In the Dual Core mode, this function should be called on both cores in the 
*  following sequence:
*   - call this function on CM0+ core to initialize the Controller.
*   - start CM4 core by calling Cy_SysEnableCM4() function.
*   - call this function on CM4 core to initialize the Host and Profiles.
* 
*  \param callbackFunc: Event callback function to receive events from BLE stack.
*   cy_ble_callback_t is a function pointer type.
* 
* \return
*  cy_en_ble_api_result_t : Return value indicates if the function succeeded or 
*  failed. The following are possible error codes.
* 
*   <table>
*  	  <tr>
*       <th>Error codes</th>
*       <th>Description</th>
*     </tr>
*     <tr>
*       <td>CY_BLE_SUCCESS</td>
*       <td>On successful operation.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_INVALID_PARAMETER</td>
*       <td>On passing a NULL pointer to the function when the BLE stack is not built in the HCI mode. 
*			CY_BLE_ERROR_INVALID_PARAMETER is never returned in HCI mode.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_REPEATED_ATTEMPTS</td>
*       <td>On invoking this function more than once without calling Cy_BLE_StackShutdown() 
*           function between calls to this function.</td>
*     </tr>
*     <tr>
*       <td>CY_BLE_ERROR_MEMORY_ALLOCATION_FAILED</td>
*       <td>There is insufficient memory available.</td>
*     </tr>
*   </table>
* 
*  \globalvars
*   The cy_ble_initVar variable is used to indicate the initial
*   configuration of this component. The variable is initialized to a zero (0u)
*   and set to one (1u) the first time Cy_BLE_Start() is called. This
*   allows for component initialization without re-initialization in all
*   subsequent calls to the Cy_BLE_Start() routine.
* 
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Start(cy_ble_callback_t callbackFunc)
{
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
    
#if CY_BLE_MODE_PROFILE   
    /* If not initialized, then initialize all the required hardware and software */
    if(callbackFunc != NULL)
    {
        cy_ble_config.callbackFunc = callbackFunc;
        if(cy_ble_initVar == 0u)
        {
    #if(CY_BLE_CONFIG_STACK_CONTR_CORE)
        cy_ble_config.blessIsrConfig = &BLE_1_bless_isr_cfg;
    #endif /* (CY_BLE_CONFIG_STACK_CONTR_CORE) */

    #if(CY_BLE_CONFIG_STACK_MODE_HOST_UART)
        cy_ble_config.uartHostIsrConfig = &BLE_1_host_uart_isr_cfg;
        cy_ble_config.uartHostConfig = &BLE_1_Host_Uart_config;
        cy_ble_config.uartHostHw = BLE_1_Host_Uart_HW;
    #endif  /* (CY_BLE_CONFIG_STACK_MODE_HOST_UART) */

            apiResult = Cy_BLE_Init(&cy_ble_config);
            cy_ble_initVar = 1u;
        }
        if(apiResult == CY_BLE_SUCCESS)
        {
            apiResult = Cy_BLE_Enable();
        }
    }
    else
    {
        apiResult = CY_BLE_ERROR_INVALID_PARAMETER;
    }
#else
{
    static cy_stc_ble_config_t config =
    {
    #if(CY_BLE_CONFIG_STACK_CONTR_CORE)
        .blessIsrConfig = &BLE_1_bless_isr_cfg,
    #endif /* (CY_BLE_CONFIG_STACK_CONTR_CORE) */
    
    #if(CY_BLE_CONFIG_STACK_MODE_CONTR_UART)
        .uartIsrConfig = &BLE_1_uart_isr_cfg,
        .uartConfig = &BLE_1_HAL_Uart_config,
        .uartHw = BLE_1_HAL_Uart_HW
    #endif /* CY_BLE_CONFIG_STACK_MODE_CONTR_UART */
    };
        apiResult = Cy_BLE_Init(&config);
        cy_ble_initVar = 1u;
        
        if(apiResult == CY_BLE_SUCCESS)
        {
            apiResult = Cy_BLE_Enable();
        }
}
#endif  /*  profile mode */
    
    return (apiResult);
}


/******************************************************************************
* Function Name: Cy_BLE_Stop
***************************************************************************//**
* 
*  This function stops any ongoing operation in the BLE Stack and forces the 
*  BLE Stack to shut down. The only function that can be called after calling
*  this function is Cy_BLE_Start() when Cy_BLE_GetState() API returns 
*  CY_BLE_STATE_STOPPED state. 
*
*  Calling this function results in generation of 
*  a CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE event on a successful stack shutdown.
* 
* \return
*  cy_en_ble_api_result_t : Return value indicates if the function succeeded or
*  failed. Following are the possible error codes.
*
*    Errors codes                    | Description
*    ------------                    | -----------
*    CY_BLE_SUCCESS                  | On successful operation.
*    CY_BLE_ERROR_INVALID_OPERATION  | On calling Cy_BLE_Stop before calling Cy_BLE_Start.
*
******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_Stop(void)
{
    return( Cy_BLE_Disable() );
}

#endif /* CY_BLE_HOST_CONTR_CORE */

/* [] END OF FILE */
