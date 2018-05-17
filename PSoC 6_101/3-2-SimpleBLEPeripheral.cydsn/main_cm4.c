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

SemaphoreHandle_t bleSemaphore;

void genericEventHandler(uint32_t event, void *eventParameter)
{
    cy_stc_ble_gatts_write_cmd_req_param_t   *writeReqParameter;
    
    /* Take an action based on the current event */
    switch (event)
    {
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            PWM_BLINK_Start();    
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            Cy_TCPWM_TriggerReloadOrIndex(PWM_DIM_HW,PWM_DIM_CNT_NUM);
            Cy_TCPWM_PWM_Disable(PWM_DIM_HW,PWM_DIM_CNT_NUM);
    
        break;

        case CY_BLE_EVT_GATT_CONNECT_IND:
            Cy_TCPWM_TriggerReloadOrIndex(PWM_BLINK_HW,PWM_BLINK_CNT_NUM);
            Cy_TCPWM_PWM_Disable(PWM_BLINK_HW,PWM_BLINK_CNT_NUM);     
            PWM_DIM_Start();
            
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter;
        
            if(CY_BLE_LED_GREEN_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {              
                uint32_t val = writeReqParameter->handleValPair.value.val[0];
                if(val>100) val = 100;

                Cy_TCPWM_PWM_SetCompare0(PWM_DIM_HW,PWM_DIM_CNT_NUM,  val);
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
    }
}

// Starts the system
int main(void)
{
    __enable_irq(); 
    
    UART_1_Start();
    setvbuf( stdin, NULL, _IONBF, 0 ); 
    setvbuf( stdout, NULL, _IONBF, 0 ); 
    printf("System Started\r\n");

    PWM_DIM_Start();
    PWM_BLINK_Start();
    
    xTaskCreate(bleTask,"bleTask",8*1024,0,2,0);
    
    vTaskStartScheduler();
    for(;;)
    {
    }
}

/* [] END OF FILE */
