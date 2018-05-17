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

/* Task to handle LED blinking */
void LED_TASK(void *arg)
{
    (void) arg;
    
    for(;;)
    {
        /* Toggle LED on/off every 500ms */
        Cy_GPIO_Write(RED_PORT,RED_NUM,1);
        vTaskDelay(500);
        Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        vTaskDelay(500);
    
    }
}

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    /* Create a new task and start the RTOS task scheduler */
    xTaskCreate(LED_TASK,"LED Task",400,0,1,0);
    vTaskStartScheduler();
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
