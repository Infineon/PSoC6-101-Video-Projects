/***************************************************************************//**
* \file EZI2C.c
* \version 2.0
*
*  This file provides constants and parameter values for the EZI2C component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(EZI2C_CY_SCB_EZI2C_PDL_H)
#define EZI2C_CY_SCB_EZI2C_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_ezi2c.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*   Initial Parameter Constants
****************************************/

#define EZI2C_ENABLE_SECOND_ADDR  (0U)


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component only APIs. */
void EZI2C_Start(void);

/* Basic functions. */
__STATIC_INLINE cy_en_scb_ezi2c_status_t EZI2C_Init(cy_stc_scb_ezi2c_config_t const *config);
__STATIC_INLINE void EZI2C_DeInit (void);
__STATIC_INLINE void EZI2C_Enable (void);
__STATIC_INLINE void EZI2C_Disable(void);

__STATIC_INLINE uint32_t EZI2C_GetActivity(void);

/* Configuration functions for address 1 */
__STATIC_INLINE void     EZI2C_SetAddress1(uint8_t addr);
__STATIC_INLINE uint32_t EZI2C_GetAddress1(void);
__STATIC_INLINE void     EZI2C_SetBuffer1(uint8_t* buffer, uint32_t size, uint32_t rwBoundary);

#if (EZI2C_ENABLE_SECOND_ADDR)
/* Configuration functions for address 2 */
__STATIC_INLINE void     EZI2C_SetAddress2(uint8_t addr);
__STATIC_INLINE uint32_t EZI2C_GetAddress2(void);
__STATIC_INLINE void     EZI2C_SetBuffer2(uint8_t* buffer, uint32_t size, uint32_t rwBoundary);
#endif /* (EZI2C_ENABLE_SECOND_ADDR) */

__STATIC_INLINE void EZI2C_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t EZI2C_initVar;
extern cy_stc_scb_ezi2c_config_t const EZI2C_config;
extern cy_stc_scb_ezi2c_context_t EZI2C_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the SCB instance */
#define EZI2C_HW     ((CySCB_Type *) EZI2C_SCB__HW)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: EZI2C_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_ezi2c_status_t EZI2C_Init(cy_stc_scb_ezi2c_config_t const *config)
{
    return Cy_SCB_EZI2C_Init(EZI2C_HW, config, &EZI2C_context);
}


/*******************************************************************************
*  Function Name: EZI2C_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_DeInit(void)
{
    Cy_SCB_EZI2C_DeInit(EZI2C_HW);
}


/*******************************************************************************
* Function Name: EZI2C_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_Enable(void)
{
    Cy_SCB_EZI2C_Enable(EZI2C_HW);
}


/*******************************************************************************
* Function Name: EZI2C_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_Disable(void)
{
    Cy_SCB_EZI2C_Disable(EZI2C_HW, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_GetActivity
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_GetActivity() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t EZI2C_GetActivity(void)
{
    return Cy_SCB_EZI2C_GetActivity(EZI2C_HW, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_SetAddress1
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_SetAddress1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_SetAddress1(uint8_t addr)
{
    Cy_SCB_EZI2C_SetAddress1(EZI2C_HW, addr, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_GetAddress1
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_GetAddress1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t EZI2C_GetAddress1(void)
{
    return Cy_SCB_EZI2C_GetAddress1(EZI2C_HW, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_SetBuffer1
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_SetBuffer1() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_SetBuffer1(uint8_t* buffer, uint32_t size, uint32_t rwBoundary)
{
    Cy_SCB_EZI2C_SetBuffer1(EZI2C_HW, buffer, size, rwBoundary, &EZI2C_context);
}


#if (EZI2C_ENABLE_SECOND_ADDR)
/*******************************************************************************
* Function Name: EZI2C_SetAddress2
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_SlaveGetAddress2() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_SetAddress2(uint8_t addr)
{
    Cy_SCB_EZI2C_SetAddress2(EZI2C_HW, addr, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_GetAddress2
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_SlaveGetAddress2() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t EZI2C_GetAddress2(void)
{
    return Cy_SCB_EZI2C_GetAddress2(EZI2C_HW, &EZI2C_context);
}


/*******************************************************************************
* Function Name: EZI2C_SetBuffer2
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_SetBuffer2() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_SetBuffer2(uint8_t* buffer, uint32_t size, uint32_t rwBoundary)
{
    Cy_SCB_EZI2C_SetBuffer2(EZI2C_HW, buffer, size, rwBoundary, &EZI2C_context);
}
#endif /* (EZI2C_ENABLE_SECOND_ADDR) */


/*******************************************************************************
* Function Name: EZI2C_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_EZI2C_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void EZI2C_Interrupt(void)
{
    Cy_SCB_EZI2C_Interrupt(EZI2C_HW, &EZI2C_context);
}

#if defined(__cplusplus)
}
#endif

#endif /* EZI2C_CY_SCB_EZI2C_PDL_H */


/* [] END OF FILE */
