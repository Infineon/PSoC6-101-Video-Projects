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
#include <stdio.h>
#include "bleTask.h"
#include "uartTask.h"
#include "capSenseTask.h"
#include "motionTask.h"
#include "event_groups.h"

EventGroupHandle_t systemInputMode;

/*****************************************************************************\
 * Function:   main
 * Input:      void
 * Returns:    int (this function acutally never returns...)
 * Description: Starts the system ... initalizes the BLE task.. then starts the RTOS
 *   
 *    
\*****************************************************************************/
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    UART_Start();
    setvbuf(stdin,0,_IONBF,0);
    
    systemInputMode = xEventGroupCreate();
    Cy_GPIO_Write(LED8_PORT,LED8_NUM,0);
    xEventGroupSetBits(systemInputMode,MODE_CAPSENSE);
    
    xTaskCreate(bleTask,"bleTask",8*1024,0,1,0);
    xTaskCreate(uartTask,"uartTask",1024,0,1,0);
    xTaskCreate(capsenseTask,"capsenseTask",1024,0,1,0);
    xTaskCreate(motionTask,"motionTask",1024,0,1,0);    
    vTaskStartScheduler();
}

/* [] END OF FILE */
