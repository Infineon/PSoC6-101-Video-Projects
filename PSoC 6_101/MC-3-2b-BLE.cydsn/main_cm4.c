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
#include "uartTask.h"
#include "pwmTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ezi2cTask.h"
#include "event_groups.h"
#include "capSenseTask.h"
#include "bleTask.h"

QueueHandle_t pwmQueue;
EventGroupHandle_t pwmEventGroup;

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    LEDBlink_Start(); // Start PWM for the red/green LEDs controlled by the kill switch
    
    pwmQueue = xQueueCreate(4, sizeof(PWM_Message_t)); // Create queue used for sending changes to the pwmTask
    pwmEventGroup = xEventGroupCreate(); // Create event group that allows taskPWM to notify other tasks of a change
    
    /* Create RTOS tasks and start the scheduler */
    xTaskCreate ( uartTask,  "UART Task", 400, 0, 2, 0);
    xTaskCreate ( pwmTask,   "PWM Task",  400, 0, 2, 0);
    xTaskCreate ( ezi2cTask, "EZI2C Task",400,0,2,0);
    xTaskCreate( capsenseTask, "Capsense Task",(2*1024),0,2,0);
    xTaskCreate( bleTask, "BLE Task",(4*1024),0,3,0);
    
    vTaskStartScheduler(); // This function never returns
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
