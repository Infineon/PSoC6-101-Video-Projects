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

#define LED_ON  0
#define LED_OFF 1

// This is used to lock and unlock the BLE Task
SemaphoreHandle_t bleSemaphore;

/*****************************************************************************\
 * Function:    genericEventHandler
 * Input:       Cy_BLE Event Handler event and eventParameter
 * Returns:     void
 * Description: 
 *   This funtion is the BLE Event Handler function.  It is called by the BLE
 *   stack when an event occurs 
\*****************************************************************************/
void genericEventHandler(uint32_t event, void *eventParameter)
{
    (void)eventParameter; // not used
    switch (event)
    {
        case CY_BLE_EVT_STACK_ON:
            printf("Stack Started\r\n");
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;

        case CY_BLE_EVT_GATT_CONNECT_IND:
            printf("Connected\r\n");
            Cy_GPIO_Write(led9_PORT,led9_NUM,LED_ON); // Turn the LED9 On             
        break;

        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            printf("Disconnected\r\n");
            Cy_GPIO_Write(led9_PORT,led9_NUM,LED_OFF); // Turn the LED9 Off
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;

                
        default:
        break;
    }
}

/*****************************************************************************\
 * Function:    iasEventHandler
 * Input:       BLE IAS Service Handler Function: 
 *      - eventCode (which only can be CY_BLE_EVT_IASS_WRITE_CHAR_CMD
 *      - eventParam which is a pointer to  (and unused)
 * Returns:     void
 * Description: 
 *   This is called back by the BLE stack when there is a write to the IAS
 *   service.  This only occurs when the GATT Client Writes a new value
 *   for the alert.  The function figures out the state of the alert then
 *   sends a message to the alertTask usign the EventGroup alterState
\*****************************************************************************/
void iasEventHandler(uint32_t eventCode, void *eventParam)
{
    (void)eventParam;
    uint8_t alertLevel;
    
    if(eventCode == CY_BLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        /* Read the updated Alert Level value from the GATT database */
        Cy_BLE_IASS_GetCharacteristicValue(CY_BLE_IAS_ALERT_LEVEL, 
            sizeof(alertLevel), &alertLevel);
        
        switch(alertLevel)
        {
            case CY_BLE_NO_ALERT:
                printf("No alert\r\n");
                Cy_GPIO_Write(red_PORT,red_NUM,LED_OFF);
                Cy_GPIO_Write(green_PORT,green_NUM,LED_ON);
                Cy_GPIO_Write(blue_PORT,blue_NUM,LED_OFF);
            break;
            case CY_BLE_MILD_ALERT:
                printf("Medium alert\r\n");
                Cy_GPIO_Write(red_PORT,red_NUM,LED_OFF);
                Cy_GPIO_Write(green_PORT,green_NUM,LED_OFF);
                Cy_GPIO_Write(blue_PORT,blue_NUM,LED_ON);
                
            break;
            case CY_BLE_HIGH_ALERT:        
                printf("High alert\r\n");
                Cy_GPIO_Write(red_PORT,red_NUM,LED_ON);
                Cy_GPIO_Write(green_PORT,green_NUM,LED_OFF);
                Cy_GPIO_Write(blue_PORT,blue_NUM,LED_OFF);               
            break;
        }   
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
    Cy_BLE_IAS_RegisterAttrCallback (iasEventHandler);
    for(;;)
    {
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
    }
}

int main(void)
{
    __enable_irq(); 
    
    UART_1_Start();
    setvbuf( stdin, NULL, _IONBF, 0 ); 
    setvbuf( stdout, NULL, _IONBF, 0 ); 
    printf("System Started\r\n");

    xTaskCreate(bleTask,"bleTask",8*1024,0,2,0);
    
    vTaskStartScheduler();
    for(;;)
    {
    }
}

/* [] END OF FILE */
