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

QueueHandle_t pwmQueue;

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    pwmQueue = xQueueCreate(4, sizeof(PWM_Message_t));
    
    /* Create RTOS tasks and start the scheduler */
    xTaskCreate ( uartTask, "UART Task", 400, 0, 2, 0);
    xTaskCreate ( pwmTask,  "PWM Task",  400, 0, 2, 0);
    vTaskStartScheduler(); // This function never returns
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
