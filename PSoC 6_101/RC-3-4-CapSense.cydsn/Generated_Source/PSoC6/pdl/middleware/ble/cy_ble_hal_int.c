/***************************************************************************//**
* \file cy_ble_hal_int.c
* \version 2.0
*
* \brief
*  This file contains the source code for the Interrupt Service Routine for the
*  HAL section of the BLE Middleware.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include "cy_ble.h"
#include "cy_ble_hal_pvt.h"
#include "ipc/cy_ipc_sema.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_STACK_MODE_IPC) && (CY_BLE_MODE_PROFILE)

/*******************************************************************************
* Function Name: Cy_BLE_HAL_IntrNotifyIpcHandler
****************************************************************************//**
*
*  Handles the IPC notification for Interrupt Notification Feature.
*
* \return
*  None
*
*******************************************************************************/    
void Cy_BLE_HAL_IntrNotifyIpcHandler(void)
{    
    uint32_t interruptMasked;
    IPC_INTR_STRUCT_Type *intrTypePtr = Cy_IPC_Drv_GetIntrBaseAddr(CY_BLE_INTR_NOTIFY_IPC_INTR);
   /**
    * Check that there is really the IPC Notify interrupt,
    * because the same line can be used for the IPC Release interrupt.
    */
    
    interruptMasked = Cy_IPC_Drv_ExtractAcquireMask(Cy_IPC_Drv_GetInterruptStatusMasked(intrTypePtr));

    if ((1uL << (CY_BLE_INTR_NOTIFY_IPC_CHAN)) == interruptMasked )
    {
        Cy_IPC_Drv_ClearInterrupt(intrTypePtr, CY_IPC_NO_NOTIFICATION, interruptMasked);
        
        if(Cy_BLE_InterruptCallback != NULL)
        {
            /* Call the BLE interrupt feature handler */
            Cy_BLE_InterruptCallback(intrNotify.data);
        }
     
        /* Release the IPC channel with the Release interrupt */
        (void)Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(CY_BLE_INTR_NOTIFY_IPC_CHAN),
                                    (1uL << CY_BLE_INTR_NOTIFY_IPC_INTR));
    }
}
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_STACK_MODE_IPC) && (CY_BLE_MODE_PROFILE) */

#if (CY_BLE_HOST_CONTR_CORE)
#if (CY_BLE_STACK_CONTR_CORE)

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED)    
/* Globals */    
cy_stc_ble_intr_notify_t *intrNotifyPtr = NULL;
    
/* Private Function Prototypes  */
static void Cy_BLE_HAL_SendInterruptNotification(void);

/*******************************************************************************
* Function Name: Cy_BLE_HAL_SendInterruptNotification
****************************************************************************//**
*
*  This function sends notification (with interrupt type information) to user 
*  callback for the BLE interrupts subscribed by user. The list of supported 
*  interrupts described in the enumeration cy_en_ble_interrupt_callback_feature_t.
*  Cy_BLE_RegisterInterruptCallback() API used to register callback for receiving
*  the BLE interrupts.
*
*******************************************************************************/
static void Cy_BLE_HAL_SendInterruptNotification(void)
{
    if((intrNotifyPtr != NULL) && (intrNotifyPtr->mask != 0u))
    {
        uint32_t interruptType = (uint32_t)CY_BLE_INTR_CALLBACK_NONE;
        
        /* Read BLE_BLELL->EVENT_INTR */      
        if((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR) != 0u) 
        {
            interruptType |= CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR;    
        }
        else
        {
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_DSM_ENTERED_INTR_Msk) != 0u) &&
               ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED)!= 0u))
            {
                interruptType |= CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED;
            }
            
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_DSM_EXITED_INTR_Msk) != 0u) &&
               ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED) != 0u))
            {
                interruptType |= CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED;
            }
       
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_LL_INTR_Msk) != 0u))
            { 
                /**
                 *  Updates the firmware and hardware to exit sleep mode, when
                 *  called from the interrupt mode, after checking the state machine.
                 */
                Cy_BLE_LlIsrExitLowPowerMode();
           
                if(BLE_BLELL->EVENT_INTR != 0x0u)
                {            
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_CONN_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CLOSE_CE_Msk) != 0u) &&
                            ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE;
                        }
                        
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CE_TX_ACK_Msk) != 0u) &&
                           ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK;
                        }
                        
                        if(((BLE_BLELL->CONN_EXT_INTR & BLE_BLELL_CONN_EXT_INTR_EARLY_INTR_Msk) != 0u) &&
                           ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY;                     
                        }
                        
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CE_RX_Msk) != 0u) &&
                            ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX;
                        }
                    }    
                    
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_SCAN_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_ADV_RX_INTR_Msk) != 0u) &&
                           ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX) != 0u))
                        {
                            /* Return interrupt type Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX;
                        }
                        
                        if(((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_SCAN_RSP_RX_INTR_Msk) != 0u) &&
                           ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX;
                        }  
                    }
                    
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_ADV_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->ADV_INTR & BLE_BLELL_ADV_INTR_CONN_REQ_RX_INTR_Msk) != 0u) &&
                           ((intrNotifyPtr->mask & CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX;
                        }   
                    }
                }
            }
        }
        
        /* Send interrupt type information to registered callback */
        if(interruptType != (uint32_t)CY_BLE_INTR_CALLBACK_NONE)
        {       
            #if (CY_BLE_MODE_PROFILE) 
                if(Cy_BLE_InterruptCallback != NULL)
                {
                    Cy_BLE_InterruptCallback((uint32_t)interruptType);
                }
            #else
                {
                    uint32_t rTimeout = 100u; /* us */                 
                    intrNotifyPtr->data = (uint32_t)interruptType; 
  
                    /* Check if IPC is released */
                    while((Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_BLE_INTR_NOTIFY_IPC_CHAN))) &&
                          (rTimeout !=0u))
                    {
                        Cy_SysLib_DelayUs(1u);
                        rTimeout--;
                    }
                    
                    /* Generates a notify event by IPC interrupt structures */
                    if(rTimeout != 0u)
                    {
                        (void)Cy_IPC_Drv_AcquireNotify(Cy_IPC_Drv_GetIpcBaseAddress(CY_BLE_INTR_NOTIFY_IPC_CHAN), 
                                       (1uL << (uint32_t)CY_BLE_INTR_NOTIFY_IPC_INTR));                    
                    }                   
                    
                }
            #endif /* (CY_BLE_MODE_PROFILE) */
        }
    }
}
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) */
    

/*******************************************************************************
* Function Name: Cy_BLE_BlessInterrupt
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the BLE sub-system.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_BlessInterrupt(void)
{   
#if (CY_BLE_MODE_PROFILE)
    bool sendNotification = false;
    
    /** 
     * Check CONN, ADV, SCAN and INITIATOR engines to send notification 
     * to application Host callback (registered by Cy_BLE_RegisterAppHostCallback()
     * function). 
     */
    if(Cy_BLE_HostRegisteredCallback != NULL)
    {
        if((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_LL_INTR_Msk) != 0u)
        { 
            Cy_BLE_LlIsrExitLowPowerMode();
          
            if( /* Check INITIATOR engine */
                (((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_INIT_INTR_Msk) != 0u) &&
                 ((BLE_BLELL->INIT_INTR & BLE_BLELL_INIT_INTR_INIT_CLOSE_WINDOW_INR_Msk) != 0u)) ||
                /* Check ADV engine */
                (((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_ADV_INTR_Msk) != 0u) &&
                 ((BLE_BLELL->ADV_INTR & BLE_BLELL_ADV_INTR_ADV_CLOSE_INTR_Msk) != 0u)) ||
                /* Check SCAN engine */
                (((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_SCAN_INTR_Msk) != 0u) && 
                 ( ((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_ADV_RX_INTR_Msk) != 0u) ||
                   ((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_SCAN_RSP_RX_INTR_Msk) != 0u)) ) ||  
                /* Check CONN engine */
                (((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_CONN_INTR_Msk) != 0u) &&
                 ((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CLOSE_CE_Msk) != 0u)) )
            { 
                sendNotification = true;
            }
        }
    }
#endif /* (CY_BLE_MODE_PROFILE) */

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED)    
    /** 
     * Send notification (with interrupt type) to user callback for the 
     * BLE interrupts subscribed by user. Cy_BLE_RegisterInterruptCallback API 
     * used to register callback for receiving BLE interrupts.
     */
    Cy_BLE_HAL_SendInterruptNotification();
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) */ 

    /* Call BLE Stack manager bless function handler */
    Cy_BLE_HAL_BlessInterruptHandler();
    /* Host stack takes care of clearing interrupts */   

#if (CY_BLE_MODE_PROFILE)
    /**
     * Call the application application Host callback. This notification indicates
     * that user should call Cy_BLE_ProcessEvents() to process
     * pending Stack events. 
     */
    if((Cy_BLE_HostRegisteredCallback != NULL) && sendNotification)
    {
        Cy_BLE_HostRegisteredCallback();
    }
#endif /* (CY_BLE_MODE_PROFILE) */
}

#endif  /* (CY_BLE_STACK_CONTR_CORE) */

#if (CY_BLE_STACK_MODE_HOST_UART)

/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_Interrupt
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the UART.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_Interrupt(void)
{
    uint8_t uartTxStatus = CY_BLE_INTR_TX_SUCCESS;
    uint32_t srcInterrupt = 0u;

    uint8_t lenRxFifo;
    uint8_t srcCount;
    uint8_t uartRxStatus = CY_BLE_INTR_RX_SUCCESS;
    uint8_t receivedData;

    /* Evaluate RX interrupt event */
    srcInterrupt = Cy_SCB_GetRxInterruptStatusMasked(cy_ble_configPtr->uartHostHw);

    if(0u != (srcInterrupt & CY_SCB_RX_INTR_NOT_EMPTY))
    {
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_PARITY_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_PARITY_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_FRAME_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_FRAME_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_OVERFLOW))
        {
            uartRxStatus |= CY_BLE_INTR_RX_OVERFLOW;
        }
        if(uartRxStatus == CY_BLE_INTR_RX_SUCCESS)
        {
            lenRxFifo = (uint8_t)Cy_SCB_GetNumInRxFifo(cy_ble_configPtr->uartHostHw);
            for(srcCount = 0u; srcCount < lenRxFifo; srcCount++)
            {
                receivedData = (uint8_t)Cy_SCB_ReadRxFifo(cy_ble_configPtr->uartHostHw);
                Cy_BLE_HAL_HciTransportHandler((char8)receivedData, 1u);
            }
        }
        else
        {
            Cy_SCB_ClearRxFifo(cy_ble_configPtr->uartHostHw);
        }
        Cy_SCB_ClearRxInterrupt(cy_ble_configPtr->uartHostHw, srcInterrupt);
    }
    else
    {
        /* No RX interrupt. Do nothing. */
    }

    /* Evaluate TX interrupt event in sequence */
    srcInterrupt = Cy_SCB_GetTxInterruptStatusMasked(cy_ble_configPtr->uartHostHw);

    /* Stack manager TX UART complete */
    if(0u != (srcInterrupt & CY_SCB_TX_INTR_UART_DONE))
    {
        if(0u != (srcInterrupt & CY_SCB_TX_INTR_OVERFLOW))
        {
            /*Stack manager TX UART error */
            uartTxStatus |= CY_BLE_INTR_TX_OVERFLOW;
        }
        Cy_SCB_ClearTxInterrupt(cy_ble_configPtr->uartHostHw, srcInterrupt);
    }
    else
    {
        /* No TX interrupt. Do nothing. */
    }
}
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */

#endif /* (CY_BLE_HOST_CONTR_CORE) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* [] END OF FILE */
