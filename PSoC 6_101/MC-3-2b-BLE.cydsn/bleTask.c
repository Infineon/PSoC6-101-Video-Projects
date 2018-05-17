/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <limits.h>
#include "global.h"
#include "pwmTask.h"

SemaphoreHandle_t bleSemaphore;

/*******************************************************************************
* Function Name: updateMotorsGatt
********************************************************************************
*
* This function writes the GATT database with current Motor percent.  The write 
* can be CY_BLE_GATT_DB_LOCALLY_INITIATED or CY_BLE_GATT_DB_PEER_INITIATED
* Once the write is done it will notify all connections who have CCCD notify set
*
* \param 
*  motor and enumerated motor_t that is either M1 or M2
* \param
*  percent a number from 0 to 100 as a uint8_t
* \param
*  flags - either CY_BLE_GATT_DB_LOCALLY_INITIATED or CY_BLE_GATT_DB_PEER_INITIATED
* \param
* connectionHandle - either a connectionHandle or a bunch or a zero'd structure 
* (if LOCALLY_INITIATED)
* 
* \return
* void
*
*******************************************************************************/
void updateMotorsGatt(motors_t motor,uint8_t percent,uint8_t flags)
{    
    cy_stc_ble_gatt_handle_value_pair_t myHvp;
    
    if(percent<0)
        percent = 0;
    if(percent>100)
        percent = 100;
  
    switch(motor)
    {
        case M1:
            myHvp.attrHandle = CY_BLE_MOTOR_M1_CHAR_HANDLE;
        break;
        case M2:
            myHvp.attrHandle = CY_BLE_MOTOR_M2_CHAR_HANDLE;
        break;
    }

    myHvp.value.val = (uint8_t *)&percent;
    myHvp.value.actualLen = 1;
    myHvp.value.len = 1;

    if(flags == CY_BLE_GATT_DB_PEER_INITIATED)
    {
        Cy_BLE_GATTS_WriteAttributeValuePeer(&cy_ble_connHandle[0],&myHvp);
        PWM_Message_t myMessage;
        myMessage.motor = motor;
        myMessage.percent = percent;
        myMessage.changeType = POS_ABSOLUTE;
        xQueueSend(pwmQueue,&myMessage,0); 
    }
    else
    {
        Cy_BLE_GATTS_WriteAttributeValueLocal(&myHvp);   
        Cy_BLE_GATTS_SendNotification (&cy_ble_connHandle[0], &myHvp);  
    }
}

void genericEventHandler(uint32_t event, void *eventParameter)
{
    cy_stc_ble_gatts_write_cmd_req_param_t   *writeReqParameter;
    
    /* Take an action based on the current event */
    switch (event)
    {
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            Cy_GPIO_Write(LED9_PORT,LED9_NUM,1);
            printf("BLE: ON/Disconnect\n");
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
    
        break;

        case CY_BLE_EVT_GATT_CONNECT_IND:
            printf("BLE: Connection\n");
            Cy_GPIO_Write(LED9_PORT,LED9_NUM,0); 
            
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            
            writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter;
        
            if(CY_BLE_MOTOR_M1_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {
               updateMotorsGatt(M1,writeReqParameter->handleValPair.value.val[0],CY_BLE_GATT_DB_PEER_INITIATED);
            }
            
            if(CY_BLE_MOTOR_M2_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {     
               updateMotorsGatt(M2,writeReqParameter->handleValPair.value.val[0],CY_BLE_GATT_DB_PEER_INITIATED);
            }
            
            if(CY_BLE_MOTOR_M1_REL_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {
                PWM_Message_t myMessage;
                myMessage.motor = M1;
                myMessage.changeType = POS_RELATIVE;
                myMessage.percent = (int8_t)writeReqParameter->handleValPair.value.val[0];
                xQueueSend(pwmQueue,&myMessage,0);            
            }
            
            if(CY_BLE_MOTOR_M2_REL_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {     
                PWM_Message_t myMessage;
                myMessage.motor = M2;
                myMessage.changeType = POS_RELATIVE;
                myMessage.percent = (int8_t)writeReqParameter->handleValPair.value.val[0];
                xQueueSend(pwmQueue,&myMessage,0);            
            }
            
            if(CY_BLE_MOTOR_M1_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE  ==  writeReqParameter->handleValPair.attrHandle ||
                CY_BLE_MOTOR_M2_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE  ==  writeReqParameter->handleValPair.attrHandle
            )
            { 
                cy_stc_ble_gatts_db_attr_val_info_t myWrite;
                myWrite.offset = 0;
                myWrite.flags = CY_BLE_GATT_DB_PEER_INITIATED;
                myWrite.connHandle = writeReqParameter->connHandle;
                myWrite.handleValuePair = writeReqParameter->handleValPair;
                
                Cy_BLE_GATTS_WriteAttributeValueCCCD(&myWrite);
            }
                     
            Cy_BLE_GATTS_WriteRsp(writeReqParameter->connHandle);
            
            break;
        
        default:
            break;
    }
}

/*****************************************************************************\
 * Function:    bleInterruptNotify
 * Input:       void (it is called inside of the ISR)
 * Returns:     void
 * Description: 
 *   This is called back in the BLE ISR when an event has occured and needs to
 *   be processed.  It will then set/give the sempahore to tell the BLE task to
 *   process events.
\*****************************************************************************/
void bleInterruptNotify()
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(bleSemaphore, &xHigherPriorityTaskWoken); 
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*****************************************************************************\
 * Function:    bleTask
 * Input:       A FreeRTOS Task - void * that is unused
 * Returns:     void
 * Description: 
 *  This task starts the BLE stack... and processes events when the bleSempahore
 *  is set by the ISR.
\*****************************************************************************/

void bleTask(void *arg)
{
    (void)arg;
    
    printf("BLE Task Started\r\n");

    bleSemaphore = xSemaphoreCreateCounting(UINT_MAX,0);
    
    Cy_BLE_Start(genericEventHandler);
    
    
    while(Cy_BLE_GetState() != CY_BLE_STATE_ON) // Get the stack going
    {
        Cy_BLE_ProcessEvents();
    }
    
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    
    for(;;)
    {
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
        
                // If the PWM task says we need to update the GATT database and sends out the motor percent
        // then do it.. 
        if(xEventGroupGetBits(pwmEventGroup) & PWM_EVENT_BLE)
        {
            xEventGroupClearBits(pwmEventGroup,PWM_EVENT_BLE);
            updateMotorsGatt(M1,getMotorPercent(M1),CY_BLE_GATT_DB_LOCALLY_INITIATED);
            updateMotorsGatt(M2,getMotorPercent(M2),CY_BLE_GATT_DB_LOCALLY_INITIATED);
        }
    }
}
/* [] END OF FILE */
