/*******************************************************************************
* File Name: Thermistor.h
* Version 1.20
*
* Description:
*  This header file provides registers and constants associated with the
*  ThermistorCalc component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_THERMISTOR_CALC_Thermistor_H)
#define CY_THERMISTOR_CALC_Thermistor_H

#include "project.h"

#define Thermistor_IMPLEMENTATION         (0u)
#define Thermistor_EQUATION_METHOD        (0u)
#define Thermistor_LUT_METHOD             (1u)

#if (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD)
    #include <math.h>
#endif /* (Thermistor_IMPLEMENTATION == Thermistor_EQUATION_METHOD) */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Thermistor_REF_RESISTOR           (10000)
#define Thermistor_REF_RES_SHIFT          (0u)
#define Thermistor_ACCURACY               (10u)
#define Thermistor_MIN_TEMP               (0 * Thermistor_SCALE)


/***************************************
*        Function Prototypes
***************************************/

uint32 Thermistor_GetResistance(int16 vReference, int16 vThermistor)
                                      ;
int16 Thermistor_GetTemperature(uint32 resT) ;


/***************************************
*           API Constants
***************************************/

#define Thermistor_K2C                    (273.15)
#define Thermistor_SCALE                  (100)

#define Thermistor_THA               (0.0009032679)
#define Thermistor_THB               (0.000248772)
#define Thermistor_THC               (2.041094E-07)


#endif /* CY_THERMISTOR_CALC_Thermistor_H */


/* [] END OF FILE */
