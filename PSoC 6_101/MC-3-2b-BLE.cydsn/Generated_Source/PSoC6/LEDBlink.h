/*******************************************************************************
* File Name: LEDBlink.h
* Version 1.0
*
* Description:
*  This file provides constants and parameter values for the LEDBlink
*  component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(LEDBlink_CY_TCPWM_PWM_PDL_H)
#define LEDBlink_CY_TCPWM_PWM_PDL_H

#include "cyfitter.h"
#include "tcpwm/cy_tcpwm_pwm.h"

   
/*******************************************************************************
* Variables
*******************************************************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t  LEDBlink_initVar;
extern cy_stc_tcpwm_pwm_config_t const LEDBlink_config;
/** @} group_globals */


/***************************************
*        Function Prototypes
****************************************/
/**
* \addtogroup group_general
* @{
*/
void LEDBlink_Start(void);
__STATIC_INLINE cy_en_tcpwm_status_t LEDBlink_Init(cy_stc_tcpwm_pwm_config_t const *config);
__STATIC_INLINE void LEDBlink_DeInit(void);
__STATIC_INLINE void LEDBlink_Enable(void);
__STATIC_INLINE void LEDBlink_Disable(void);
__STATIC_INLINE uint32_t LEDBlink_GetStatus(void);
__STATIC_INLINE void LEDBlink_SetCompare0(uint32_t compare0);
__STATIC_INLINE uint32_t LEDBlink_GetCompare0(void);
__STATIC_INLINE void LEDBlink_SetCompare1(uint32_t compare1);
__STATIC_INLINE uint32_t LEDBlink_GetCompare1(void);
__STATIC_INLINE void LEDBlink_EnableCompareSwap(bool enable);
__STATIC_INLINE void LEDBlink_SetCounter(uint32_t count);
__STATIC_INLINE uint32_t LEDBlink_GetCounter(void);
__STATIC_INLINE void LEDBlink_SetPeriod0(uint32_t period0);
__STATIC_INLINE uint32_t LEDBlink_GetPeriod0(void);
__STATIC_INLINE void LEDBlink_SetPeriod1(uint32_t period1);
__STATIC_INLINE uint32_t LEDBlink_GetPeriod1(void);
__STATIC_INLINE void LEDBlink_EnablePeriodSwap(bool enable);
__STATIC_INLINE void LEDBlink_TriggerStart(void);
__STATIC_INLINE void LEDBlink_TriggerReload(void);
__STATIC_INLINE void LEDBlink_TriggerKill(void);
__STATIC_INLINE void LEDBlink_TriggerSwap(void);
__STATIC_INLINE uint32_t LEDBlink_GetInterruptStatus(void);
__STATIC_INLINE void LEDBlink_ClearInterrupt(uint32_t source);
__STATIC_INLINE void LEDBlink_SetInterrupt(uint32_t source);
__STATIC_INLINE void LEDBlink_SetInterruptMask(uint32_t mask);
__STATIC_INLINE uint32_t LEDBlink_GetInterruptMask(void);
__STATIC_INLINE uint32_t LEDBlink_GetInterruptStatusMasked(void);
/** @} general */


/***************************************
*           API Constants
***************************************/

/**
* \addtogroup group_macros
* @{
*/
/** This is a ptr to the base address of the TCPWM instance */
#define LEDBlink_HW                 (LEDBlink_TCPWM__HW)

/** This is a ptr to the base address of the TCPWM CNT instance */
#define LEDBlink_CNT_HW             (LEDBlink_TCPWM__CNT_HW)

/** This is the counter instance number in the selected TCPWM */
#define LEDBlink_CNT_NUM            (LEDBlink_TCPWM__CNT_IDX)

/** This is the bit field representing the counter instance in the selected TCPWM */
#define LEDBlink_CNT_MASK           (1UL << LEDBlink_CNT_NUM)
/** @} group_macros */

#define LEDBlink_INPUT_MODE_MASK    (0x3U)
#define LEDBlink_INPUT_DISABLED     (7U)


/*******************************************************************************
* Function Name: LEDBlink_Init
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_tcpwm_status_t LEDBlink_Init(cy_stc_tcpwm_pwm_config_t const *config)
{
    return(Cy_TCPWM_PWM_Init(LEDBlink_HW, LEDBlink_CNT_NUM, config));
}


/*******************************************************************************
* Function Name: LEDBlink_DeInit
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_DeInit(void)                   
{
    Cy_TCPWM_PWM_DeInit(LEDBlink_HW, LEDBlink_CNT_NUM, &LEDBlink_config);
}

/*******************************************************************************
* Function Name: LEDBlink_Enable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Enable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_Enable(void)                   
{
    Cy_TCPWM_Enable_Multiple(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_Disable
****************************************************************************//**
*
* Invokes the Cy_TCPWM_Disable_Multiple() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_Disable(void)                  
{
    Cy_TCPWM_Disable_Multiple(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_GetStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetStatus(void)                
{
    return(Cy_TCPWM_PWM_GetStatus(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_SetCompare0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCompare0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetCompare0(uint32_t compare0)      
{
    Cy_TCPWM_PWM_SetCompare0(LEDBlink_HW, LEDBlink_CNT_NUM, compare0);
}


/*******************************************************************************
* Function Name: LEDBlink_GetCompare0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCompare0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetCompare0(void)              
{
    return(Cy_TCPWM_PWM_GetCompare0(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_SetCompare1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCompare1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetCompare1(uint32_t compare1)      
{
    Cy_TCPWM_PWM_SetCompare1(LEDBlink_HW, LEDBlink_CNT_NUM, compare1);
}


/*******************************************************************************
* Function Name: LEDBlink_GetCompare1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCompare1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetCompare1(void)              
{
    return(Cy_TCPWM_PWM_GetCompare1(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_EnableCompareSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_EnableCompareSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_EnableCompareSwap(bool enable)  
{
    Cy_TCPWM_PWM_EnableCompareSwap(LEDBlink_HW, LEDBlink_CNT_NUM, enable);
}


/*******************************************************************************
* Function Name: LEDBlink_SetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetCounter(uint32_t count)          
{
    Cy_TCPWM_PWM_SetCounter(LEDBlink_HW, LEDBlink_CNT_NUM, count);
}


/*******************************************************************************
* Function Name: LEDBlink_GetCounter
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetCounter() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetCounter(void)               
{
    return(Cy_TCPWM_PWM_GetCounter(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_SetPeriod0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetPeriod0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetPeriod0(uint32_t period0)          
{
    Cy_TCPWM_PWM_SetPeriod0(LEDBlink_HW, LEDBlink_CNT_NUM, period0);
}


/*******************************************************************************
* Function Name: LEDBlink_GetPeriod0
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetPeriod0() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetPeriod0(void)                
{
    return(Cy_TCPWM_PWM_GetPeriod0(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_SetPeriod1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_SetPeriod1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetPeriod1(uint32_t period1)
{
    Cy_TCPWM_PWM_SetPeriod1(LEDBlink_HW, LEDBlink_CNT_NUM, period1);
}


/*******************************************************************************
* Function Name: LEDBlink_GetPeriod1
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_GetPeriod1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetPeriod1(void)                
{
    return(Cy_TCPWM_PWM_GetPeriod1(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_EnablePeriodSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_PWM_EnablePeriodSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_EnablePeriodSwap(bool enable)
{
    Cy_TCPWM_PWM_EnablePeriodSwap(LEDBlink_HW, LEDBlink_CNT_NUM, enable);
}


/*******************************************************************************
* Function Name: LEDBlink_TriggerStart
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_TriggerStart(void)             
{
    Cy_TCPWM_TriggerStart(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_TriggerReload
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerReloadOrIndex() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_TriggerReload(void)     
{
    Cy_TCPWM_TriggerReloadOrIndex(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_TriggerKill
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerStopOrKill() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_TriggerKill(void)
{
    Cy_TCPWM_TriggerStopOrKill(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_TriggerSwap
****************************************************************************//**
*
* Invokes the Cy_TCPWM_TriggerCaptureOrSwap() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_TriggerSwap(void)     
{
    Cy_TCPWM_TriggerCaptureOrSwap(LEDBlink_HW, LEDBlink_CNT_MASK);
}


/*******************************************************************************
* Function Name: LEDBlink_GetInterruptStatus
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetInterruptStatus(void)       
{
    return(Cy_TCPWM_GetInterruptStatus(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_ClearInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_ClearInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_ClearInterrupt(uint32_t source)     
{
    Cy_TCPWM_ClearInterrupt(LEDBlink_HW, LEDBlink_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: LEDBlink_SetInterrupt
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetInterrupt(uint32_t source)
{
    Cy_TCPWM_SetInterrupt(LEDBlink_HW, LEDBlink_CNT_NUM, source);
}


/*******************************************************************************
* Function Name: LEDBlink_SetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_SetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void LEDBlink_SetInterruptMask(uint32_t mask)     
{
    Cy_TCPWM_SetInterruptMask(LEDBlink_HW, LEDBlink_CNT_NUM, mask);
}


/*******************************************************************************
* Function Name: LEDBlink_GetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetInterruptMask(void)         
{
    return(Cy_TCPWM_GetInterruptMask(LEDBlink_HW, LEDBlink_CNT_NUM));
}


/*******************************************************************************
* Function Name: LEDBlink_GetInterruptStatusMasked
****************************************************************************//**
*
* Invokes the Cy_TCPWM_GetInterruptStatusMasked() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t LEDBlink_GetInterruptStatusMasked(void)
{
    return(Cy_TCPWM_GetInterruptStatusMasked(LEDBlink_HW, LEDBlink_CNT_NUM));
}

#endif /* LEDBlink_CY_TCPWM_PWM_PDL_H */


/* [] END OF FILE */
