/*******************************************************************************
* File Name: LEDBlink.c
* Version 1.0
*
* Description:
*  This file provides the source code to the API for the LEDBlink
*  component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LEDBlink.h"

/** Indicates whether or not the LEDBlink has been initialized. 
*  The variable is initialized to 0 and set to 1 the first time 
*  LEDBlink_Start() is called. This allows the Component to 
*  restart without reinitialization after the first call to 
*  the LEDBlink_Start() routine.
*/
uint8_t LEDBlink_initVar = 0U;

/** The instance-specific configuration structure. This should be used in the 
*  associated LEDBlink_Init() function.
*/ 
cy_stc_tcpwm_pwm_config_t const LEDBlink_config =
{
    .pwmMode = 4UL,
    .clockPrescaler = 0UL,
    .pwmAlignment = 0UL,
    .deadTimeClocks = 0UL,
    .runMode = 0UL,
    .period0 = 1000UL,
    .period1 = 32768UL,
    .enablePeriodSwap = false,
    .compare0 = 500UL,
    .compare1 = 16384UL,
    .enableCompareSwap = false,
    .interruptSources = 0UL,
    .invertPWMOut = 0UL,
    .invertPWMOutN = 0UL,
    .killMode = 2UL,
    .swapInputMode = 3UL,
    .swapInput = CY_TCPWM_INPUT_CREATOR,
    .reloadInputMode = 3UL,
    .reloadInput = CY_TCPWM_INPUT_CREATOR,
    .startInputMode = 3UL,
    .startInput = CY_TCPWM_INPUT_CREATOR,
    .killInputMode = 3UL,
    .killInput = CY_TCPWM_INPUT_CREATOR,
    .countInputMode = 3UL,
    .countInput = CY_TCPWM_INPUT_CREATOR,
};


/*******************************************************************************
* Function Name: LEDBlink_Start
****************************************************************************//**
*
*  Calls the LEDBlink_Init() when called the first time and enables 
*  the LEDBlink. For subsequent calls the configuration is left 
*  unchanged and the component is just enabled.
*
* \globalvars
*  \ref LEDBlink_initVar
*
*******************************************************************************/
void LEDBlink_Start(void)
{
    if (0U == LEDBlink_initVar)
    {
        (void) Cy_TCPWM_PWM_Init(LEDBlink_HW, LEDBlink_CNT_NUM, &LEDBlink_config);

        LEDBlink_initVar = 1U;
    }

    Cy_TCPWM_Enable_Multiple(LEDBlink_HW, LEDBlink_CNT_MASK);
    
    #if (LEDBlink_INPUT_DISABLED == 7UL)
        Cy_TCPWM_TriggerStart(LEDBlink_HW, LEDBlink_CNT_MASK);
    #endif /* (LEDBlink_INPUT_DISABLED == 7UL) */    
}


/* [] END OF FILE */
