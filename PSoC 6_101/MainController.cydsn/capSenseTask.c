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
#include "pwmTask.h"
#include "global.h"

void capsenseTask(void *arg)
{
    (void)arg;
    
    motors_t currentMotor=M1;
    PWM_Message_t myMessage;

    CapSense_Start();
    CapSense_ScanAllWidgets(); // Start initial scan
        
    for(;;)
    {
        if(!CapSense_IsBusy())
        {
            
            CapSense_ProcessAllWidgets();
            int pos;
            pos=CapSense_GetCentroidPos(CapSense_LINEARSLIDER0_WDGT_ID); // Get finger position on slider
            if(pos<0xFFFF) // This is true if the slider is being touched
            {
                /* Send desired motor position to the PWM queue */
                myMessage.motor = currentMotor;
                myMessage.percent = pos;
                xQueueSend(pwmQueue,&myMessage,0);
            }
            
            if(CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID)) // Check button 0 state
            {
                currentMotor = M1;
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID)) // Check button 1 state
            {
               currentMotor = M2;
            }
               
            CapSense_UpdateAllBaselines();
            CapSense_ScanAllWidgets(); // Start next scan
        }
        else
        {
            /* If CapSense is busy, just sleep the task for a while to let another task have a turn */
            vTaskDelay(50);
        }
    }
}
/* [] END OF FILE */
