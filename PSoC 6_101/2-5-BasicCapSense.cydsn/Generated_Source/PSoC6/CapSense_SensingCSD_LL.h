/***************************************************************************//**
* \file CapSense_SensingCSD_LL.h
* \version 2.0
*
* \brief
*   This file provides the headers of APIs specific to CSD sensing implementation.
*
* \see CapSense v2.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2017), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(CY_SENSE_CapSense_SENSINGCSD_LL_H)
#define CY_SENSE_CapSense_SENSINGCSD_LL_H

#include "cyfitter_gpio.h"
#include "syslib/cy_syslib.h"
#include "CapSense_Structure.h"
#include "CapSense_Configuration.h"

/****************************************************************************
* Register and mode mask definition
****************************************************************************/

#define CapSense_CSD_CSDCMP_INIT                                (CapSense_CSD_CSDCMP_CSDCMP_DISABLED)

/* SW_HS_P_SEL switches state for Coarse initialization of CMOD (sense path) */
#if (CapSense_ENABLE == CapSense_CSD_EN)
    #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
        #define CapSense_CSD_HS_P_SEL_COARSE_CMOD               (CapSense_CSD_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
    #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CMOD_CONNECTION)
        #define CapSense_CSD_HS_P_SEL_COARSE_CMOD               (CapSense_CSD_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
    #else
        #define CapSense_CSD_HS_P_SEL_COARSE_CMOD               (CapSense_CSD_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
    #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */
#else
    #define CapSense_CSD_HS_P_SEL_COARSE_CMOD                   (0x00000000uL)
#endif /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) && (CapSense_ENABLE == CapSense_CSD_EN)) */

#if ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
    (CapSense_ENABLE == CapSense_CSD_SHIELD_EN))
    /* SW_HS_P_SEL switches state for Coarse initialization of CTANK (sense path) */
    #if (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)
        #define CapSense_CSD_HS_P_SEL_COARSE_TANK               (CapSense_CSD_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
    #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CTANK_CONNECTION)
        #define CapSense_CSD_HS_P_SEL_COARSE_TANK               (CapSense_CSD_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
    #elif (CapSense_CSD__CMOD_PAD == CapSense_CTANK_CONNECTION)
        #define CapSense_CSD_HS_P_SEL_COARSE_TANK               (CapSense_CSD_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
    #else
        #define CapSense_CSD_HS_P_SEL_COARSE_TANK               (CapSense_CSD_SW_HS_P_SEL_SW_HMMA_STATIC_CLOSE)
    #endif /* (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION) */
#else
    #define CapSense_CSD_HS_P_SEL_COARSE_TANK                   (0x00000000uL)
#endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
           (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)) */

#define CapSense_CSD_SW_HS_P_SEL_COARSE                         (CapSense_CSD_HS_P_SEL_COARSE_CMOD | CapSense_CSD_HS_P_SEL_COARSE_TANK)

/* C_mod config */
#if ((CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) || (CapSense_CSD__CMOD_PAD == CapSense_CTANK_CONNECTION))
    #define CapSense_CSD_SW_FW_MOD_SEL_INIT             (CapSense_CSD_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                 CapSense_CSD_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE |\
                                                                 CapSense_CSD_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE)
#else
    #define CapSense_CSD_SW_FW_MOD_SEL_INIT             (0x00000000uL)
#endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */

/* C_tank config */
#if ((CapSense_CSD__CSH_TANK_PAD == CapSense_CMOD_CONNECTION) || (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION))
    #define CapSense_CSD_SW_FW_TANK_SEL_INIT            (CapSense_CSD_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                 CapSense_CSD_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE |\
                                                                 CapSense_CSD_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE)
#else
    #define CapSense_CSD_SW_FW_TANK_SEL_INIT            (0x00000000uL)
#endif /* (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION) */

#define CapSense_CSD_SW_SHIELD_SEL_INIT                 (CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_HSCMP)

/* Defining default HW configuration according to settings in customizer. */
#define CapSense_DEFAULT_CSD_CONFIG                 (CapSense_CSD_CONFIG_FILTER_DELAY_12MHZ |\
                                                             CapSense_IREF_SRC_CFG |\
                                                             CapSense_PWR_MODE_CFG)

#if(CapSense_SENSING_LEGACY == CapSense_CSD_SENSING_METHOD)

    #if (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN)
        /* Enable CSDCMP */
        #define CapSense_CSD_CSDCMP_SCAN                (CapSense_CSD_CSDCMP_CSDCMP_EN_MSK |\
                                                                 CapSense_CSD_CSDCMP_AZ_EN_MSK)
    #else
        /* Enable CSDCMP */
        #define CapSense_CSD_CSDCMP_SCAN                (CapSense_CSD_CSDCMP_CSDCMP_EN_MSK)
    #endif /* (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN) */

    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
        (CapSense_ENABLE == CapSense_CSD_SHIELD_EN))
        /* SW_HS_P_SEL switches state for Coarse initialization of CTANK (sense path) */
        #if (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_CSD_HS_P_SEL_SCAN_TANK                 (CapSense_CSD_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE)
        #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_CSD_HS_P_SEL_SCAN_TANK                 (CapSense_CSD_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE)
        #elif (CapSense_CSD__CMOD_PAD == CapSense_CTANK_CONNECTION)
            #define CapSense_CSD_HS_P_SEL_SCAN_TANK                 (CapSense_CSD_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE)
        #else
            #define CapSense_CSD_HS_P_SEL_SCAN_TANK                 (CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
        #endif /* (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION) */
        #define CapSense_CSD_SW_HS_P_SEL_SCAN                       (CapSense_CSD_HS_P_SEL_SCAN_TANK)
    #elif(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #define CapSense_CSD_SW_HS_P_SEL_SCAN                       (CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
    #else
        #define CapSense_CSD_SW_HS_P_SEL_SCAN                       (CapSense_CSD_WAVEFORM_STATIC_OPEN)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)) */

    /* SW_FW_MOD_SEL switches state for Coarse initialization of CMOD (sense path) */
    #define CapSense_CSD_SW_FW_MOD_SEL_SCAN                 (0x00000000uL)

    #if((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
        (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
        (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION))
        #define CapSense_CSD_SW_FW_TANK_SEL_SCAN            (CapSense_CSD_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE | \
                                                                 CapSense_CSD_SW_FW_TANK_SEL_SW_F2CB_STATIC_CLOSE)
    #else
        #define CapSense_CSD_SW_FW_TANK_SEL_SCAN            (0x00000000uL)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) && \
               (CapSense_CSD__CSH_TANK_PAD == CapSense_CTANK_CONNECTION)) */

    /* Shield switch default config */
    #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
         (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN          (CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_HSCMP)
        #else
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN          (CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_HSCMP)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #elif(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN          (CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI1 | \
                                                                     CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI2_HSCMP)
        #else
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN          (CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI1 | \
                                                                     CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #else
        #define CapSense_CSD_SW_SHIELD_SEL_SCAN              (0x00000000uL)
    #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
               (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

    #define CapSense_CSD_SW_RES_INIT                        (CapSense_CSD_INIT_SWITCH_RES << CSD_SW_RES_RES_HCAV_Pos)
    #define CapSense_CSD_SW_RES_SCAN                        ((CapSense_CSD_SHIELD_SWITCH_RES << CSD_SW_RES_RES_HCBV_Pos) |\
                                                                     (CapSense_CSD_SHIELD_SWITCH_RES << CSD_SW_RES_RES_HCBG_Pos))

    #define CapSense_CSD_SHIELD_GPIO_DM                 (CY_GPIO_DM_STRONG_IN_OFF)
    #define CapSense_CSD_SENSOR_HSIOM_SEL               (CapSense_HSIOM_SEL_CSD_SENSE)
    #define CapSense_CSD_SHIELD_HSIOM_SEL               (CapSense_HSIOM_SEL_CSD_SHIELD)
    #define CapSense_CSD_CMOD_HSIOM_SEL                 (CapSense_HSIOM_SEL_AMUXA)

    #define CapSense_DEFAULT_IDACA_BALL_MODE            (CapSense_CSD_IDACA_BALL_MODE_FULL <<\
                                                                 CapSense_CSD_IDACA_BALL_MODE_POS)
    #define CapSense_DEFAULT_IDACB_BALL_MODE            (CapSense_CSD_IDACB_BALL_MODE_FULL <<\
                                                                 CapSense_CSD_IDACB_BALL_MODE_POS)

    #define CapSense_DEFAULT_SENSE_DUTY_SEL             (CapSense_CSD_SENSE_DUTY_SENSE_POL_PHI_HIGH)

#elif(CapSense_SENSING_LOW_EMI == CapSense_CSD_SENSING_METHOD)

    #if (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN)
        /* Enable CSDCMP */
        #define CapSense_CSD_CSDCMP_SCAN                (CapSense_CSD_CSDCMP_CSDCMP_EN_MSK  |\
                                                                 CapSense_CSD_CSDCMP_CMP_PHASE_PHI2 |\
                                                                 CapSense_CSD_CSDCMP_AZ_EN_MSK)
    #else
        /* Enable CSDCMP */
        #define CapSense_CSD_CSDCMP_SCAN                (CapSense_CSD_CSDCMP_CSDCMP_EN_MSK |\
                                                                 CapSense_CSD_CSDCMP_CMP_PHASE_PHI2)
    #endif /* (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN) */

    #if(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #define CapSense_CSD_SW_HS_P_SEL_SCAN               (CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE)
    #else
        #define CapSense_CSD_SW_HS_P_SEL_SCAN               (CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_OPEN)
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

    #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
        #define CapSense_CSD_SW_FW_MOD_SEL_SCAN             (CapSense_CSD_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE |\
                                                                     CapSense_CSD_SW_FW_MOD_SEL_SW_F1CA_PHI2)
        #define CapSense_CSD_SW_FW_TANK_SEL_SCAN            (0x00000000uL)
    #else
        #define CapSense_CSD_SW_FW_MOD_SEL_SCAN             (0x00000000uL)
        #define CapSense_CSD_SW_FW_TANK_SEL_SCAN            (CapSense_CSD_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE |\
                                                                     CapSense_CSD_SW_FW_TANK_SEL_SW_F2CA_PHI2)
    #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */

    #if(CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        #if(CapSense_IDAC_SINKING != CapSense_CSD_IDAC_CONFIG)
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN             (CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP | CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI1 | CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI1)
        #else
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN             (CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI1 | CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI2_HSCMP | CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI1)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #else
        #if(CapSense_IDAC_SINKING != CapSense_CSD_IDAC_CONFIG)
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN             (CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI1)
        #else
            #define CapSense_CSD_SW_SHIELD_SEL_SCAN             (CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI1)
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

    #define CapSense_CSD_SW_RES_INIT                    ((CapSense_CSD_F1PM_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_F1PM_Pos) |\
                                                                 (CapSense_CSD_F2PT_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_F2PT_Pos) |\
                                                                 (CapSense_CSD_HCAG_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_HCAG_Pos) |\
                                                                 (CapSense_CSD_HCAV_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_HCAV_Pos) |\
                                                                 (CapSense_CSD_HCBG_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_HCBG_Pos) |\
                                                                 (CapSense_CSD_HCBV_SWITCH_RES_LE_INIT << CSD_SW_RES_RES_HCBV_Pos))

    #define CapSense_CSD_SW_RES_SCAN                    ((CapSense_CSD_F1PM_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_F1PM_Pos) |\
                                                                 (CapSense_CSD_F2PT_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_F2PT_Pos) |\
                                                                 (CapSense_CSD_HCAG_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_HCAG_Pos) |\
                                                                 (CapSense_CSD_HCAV_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_HCAV_Pos) |\
                                                                 (CapSense_CSD_HCBG_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_HCBG_Pos) |\
                                                                 (CapSense_CSD_HCBV_SWITCH_RES_LE_SCAN << CSD_SW_RES_RES_HCBV_Pos))

    #define CapSense_CSD_SHIELD_GPIO_DM                 (CY_GPIO_DM_ANALOG)
    #define CapSense_CSD_SENSOR_HSIOM_SEL               (CapSense_HSIOM_SEL_AMUXA)
    #define CapSense_CSD_SHIELD_HSIOM_SEL               (CapSense_HSIOM_SEL_AMUXB)
    #define CapSense_CSD_CMOD_HSIOM_SEL                 (CapSense_HSIOM_SEL_GPIO)

    #define CapSense_DEFAULT_IDACA_BALL_MODE            (CapSense_CSD_IDACA_BALL_MODE_PHI2 <<\
                                                                 CapSense_CSD_IDACA_BALL_MODE_POS)
    #define CapSense_DEFAULT_IDACB_BALL_MODE            (CapSense_CSD_IDACB_BALL_MODE_FULL <<\
                                                                 CapSense_CSD_IDACB_BALL_MODE_POS)

    #if(CapSense_IDAC_SINKING != CapSense_CSD_IDAC_CONFIG)
        #define CapSense_DEFAULT_SENSE_DUTY_SEL             (0x00000000uL)
    #else
        #define CapSense_DEFAULT_SENSE_DUTY_SEL             (CapSense_CSD_SENSE_DUTY_SENSE_POL_MSK)
    #endif /* (CapSense_IDAC_SINKING != CapSense_CSD_IDAC_CONFIG) */

#elif(CapSense_SENSING_FULL_WAVE == CapSense_CSD_SENSING_METHOD)
#else
    #error "Not supported sensing method."
#endif /* (CapSense_SENSING_LEGACY == CapSense_CSD_SENSING_METHOD) */


/***************************************
* API Constants
***************************************/
/* Definition of time interval that is enough for charging 100nF capacitor */
#define CapSense_CSD_AVG_CYCLES_PER_LOOP                    (5u)
#define CapSense_CSD_MAX_CHARGE_TIME_US                     (100u)
#define CapSense_CSD_PRECHARGE_WATCHDOG_CYCLES_NUM          (((CapSense_CPU_CLOCK_FREQ_MHZ) * (CapSense_CSD_MAX_CHARGE_TIME_US)) /\
                                                                        (CapSense_CSD_AVG_CYCLES_PER_LOOP))

#define CapSense_CSD_IDAC_MOD_BITS_USED                     (7u)
#define CapSense_CAL_MIDDLE_BIT                             (1uL << (CapSense_CSD_IDAC_MOD_BITS_USED - 1u))
/* Increased scan time duration to cover coarse and fine init cycles */
#define CapSense_MAX_17_BITS_VALUE                          (0x0001FFFFLu)
#define CapSense_MAX_SCAN_TIME                              ((CapSense_MAX_17_BITS_VALUE * CapSense_CSD_SCANSPEED_DIVIDER) /\
                                                                        (CYDEV_CLK_PERICLK__MHZ))
#define CapSense_CALIBR_WATCHDOG_CYCLES_NUM                 (((CapSense_CPU_CLOCK_FREQ_MHZ) * (CapSense_MAX_SCAN_TIME)) /\
                                                                        (CapSense_CSD_AVG_CYCLES_PER_LOOP))

#define CapSense_DELAY_EXTRACYCLES_NUM                      (9u)

/* Calibration constants */
#define CapSense_IDAC_MOD_MAX_CALIB_ERROR                   (10u)

#if (CapSense_IDAC_GAIN_HIGH == CapSense_CSD_IDAC_GAIN)
    #define CapSense_CSD_IDAC_GAIN_VALUE_NA                 (2400u)
#elif (CapSense_IDAC_GAIN_MEDIUM == CapSense_CSD_IDAC_GAIN)
    #define CapSense_CSD_IDAC_GAIN_VALUE_NA                 (300u)
#else
    #define CapSense_CSD_IDAC_GAIN_VALUE_NA                 (37u)
#endif /* (CapSense_IDAC_GAIN_HIGH == CapSense_CSD_IDAC_GAIN) */

/* Defining the drive mode of pins depending on the Inactive sensor connection setting in the Component customizer. */
#if(CapSense_SNS_CONNECTION_GROUND == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_CSD_INACTIVE_SNS_GPIO_DM               (CY_GPIO_DM_STRONG)
#elif(CapSense_SNS_CONNECTION_HIGHZ == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_CSD_INACTIVE_SNS_GPIO_DM               (CY_GPIO_DM_ANALOG)
#elif(CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)
    #define CapSense_CSD_INACTIVE_SNS_GPIO_DM               (CY_GPIO_DM_STRONG_IN_OFF)
#else
    #error "Unsupported inactive connection for the inactive sensors."
#endif /* (CapSense_SNS_CONNECTION_GROUND == CapSense_CSD_INACTIVE_SNS_CONNECTION) */


/* Clock Source Mode */
#if (CapSense_CLK_SOURCE_DIRECT == CapSense_CSD_SNS_CLK_SOURCE)
    #define CapSense_DEFAULT_MODULATION_MODE                (CapSense_CLK_SOURCE_DIRECT)
#elif (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
    #define CapSense_DEFAULT_MODULATION_MODE                (CapSense_CLK_SOURCE_SSC2)
#elif ((CapSense_CLK_SOURCE_PRS8) == CapSense_CSD_SNS_CLK_SOURCE)
    #define CapSense_DEFAULT_MODULATION_MODE                (CapSense_CSD_SNS_CLK_SOURCE)
#elif ((CapSense_CLK_SOURCE_PRS12) == CapSense_CSD_SNS_CLK_SOURCE)
    #define CapSense_DEFAULT_MODULATION_MODE                (CapSense_CSD_SNS_CLK_SOURCE)
#else
    #define CapSense_DEFAULT_MODULATION_MODE                (CapSense_CSD_SNS_CLK_SOURCE)
#endif /* (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE) */

/* IDACs Ranges */
#if (CapSense_IDAC_GAIN_LOW == CapSense_CSD_IDAC_GAIN)
    #define CapSense_DEFAULT_IDACA_RANGE                    (CapSense_CSD_IDACA_RANGE_IDAC_LO << CapSense_CSD_IDACA_RANGE_POS)
    #define CapSense_DEFAULT_IDACB_RANGE                    (CapSense_CSD_IDACB_RANGE_IDAC_LO << CapSense_CSD_IDACB_RANGE_POS)
#elif (CapSense_IDAC_GAIN_MEDIUM == CapSense_CSD_IDAC_GAIN)
    #define CapSense_DEFAULT_IDACA_RANGE                    (CapSense_CSD_IDACA_RANGE_IDAC_MED << CapSense_CSD_IDACA_RANGE_POS)
    #define CapSense_DEFAULT_IDACB_RANGE                    (CapSense_CSD_IDACB_RANGE_IDAC_MED << CapSense_CSD_IDACB_RANGE_POS)
#else
    #define CapSense_DEFAULT_IDACA_RANGE                    (CapSense_CSD_IDACA_RANGE_IDAC_HI << CapSense_CSD_IDACA_RANGE_POS)
    #define CapSense_DEFAULT_IDACB_RANGE                    (CapSense_CSD_IDACB_RANGE_IDAC_HI << CapSense_CSD_IDACB_RANGE_POS)
#endif

/* IDACs Polarities */
#if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
    #define CapSense_DEFAULT_IDACA_POLARITY                 (CapSense_CSD_IDACA_POLARITY_VDDA_SNK << CapSense_CSD_IDACA_POLARITY_POS)
    #define CapSense_DEFAULT_IDACB_POLARITY                 (CapSense_CSD_IDACB_POLARITY_VDDA_SNK << CapSense_CSD_IDACB_POLARITY_POS)
#else
    #define CapSense_DEFAULT_IDACA_POLARITY                 (CapSense_CSD_IDACA_POLARITY_VSSA_SRC << CapSense_CSD_IDACA_POLARITY_POS)
    #define CapSense_DEFAULT_IDACB_POLARITY                 (CapSense_CSD_IDACB_POLARITY_VSSA_SRC << CapSense_CSD_IDACB_POLARITY_POS)
#endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */


#if(CapSense_VREF_SRSS != CapSense_CSD_VREF_SOURCE)
    #define CapSense_CSD_SW_REFGEN_VREF_SRC                 (CapSense_CSD_SW_REFGEN_SEL_SW_SGRP_MSK)
#else
    #define CapSense_CSD_SW_REFGEN_VREF_SRC                 (CapSense_CSD_SW_REFGEN_SEL_SW_SGR_MSK)
#endif /* (CapSense_VREF_SRSS != CapSense_CSD_VREF_SOURCE) */


/* IDAC legs configuration */
#if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
     (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        #define CapSense_DEFAULT_SW_REFGEN_SEL              (CapSense_CSD_SW_REFGEN_VREF_SRC | CapSense_CSD_SW_REFGEN_SEL_SW_IAIB_MSK)
        #define CapSense_DEFAULT_IDACB_LEG1_MODE            (CapSense_CSD_IDACB_LEG1_EN_MSK |\
                                                                    (CapSense_CSD_IDACB_LEG1_MODE_CSD_STATIC << CapSense_CSD_IDACB_LEG1_MODE_POS))
#else
        #define CapSense_DEFAULT_SW_REFGEN_SEL              (CapSense_CSD_SW_REFGEN_VREF_SRC)
        #define CapSense_DEFAULT_IDACB_LEG1_MODE            (CapSense_CSD_IDACB_LEG1_MODE_GP_STATIC << CapSense_CSD_IDACB_LEG1_MODE_POS)
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
           (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */


#if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
     (CapSense_DISABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        #define CapSense_DEFAULT_IDACB_LEG2_MODE            (CapSense_CSD_IDACA_LEG2_EN_MSK |\
                                                                    (CapSense_CSD_IDACA_LEG2_MODE_CSD_STATIC << CapSense_CSD_IDACA_LEG2_MODE_POS))
#else
        #define CapSense_DEFAULT_IDACB_LEG2_MODE            (CapSense_CSD_IDACA_LEG2_MODE_GP_STATIC << CapSense_CSD_IDACA_LEG2_MODE_POS)
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
        (CapSense_DISABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

/* IDACs register configuration is based on the Component configuration */
#define CapSense_IDAC_MOD_DEFAULT_CFG                       (CapSense_DEFAULT_IDACA_RANGE | \
                                                                     CapSense_DEFAULT_IDACA_POLARITY | \
                                                                     CapSense_DEFAULT_IDACA_BALL_MODE | \
                                                                    (CapSense_CSD_IDACA_LEG1_MODE_CSD << CapSense_CSD_IDACA_LEG1_MODE_POS) | \
                                                                     CapSense_CSD_IDACA_LEG1_EN_MSK | \
                                                                     CapSense_DEFAULT_IDACB_LEG2_MODE)

#define CapSense_IDAC_COMP_DEFAULT_CFG                      (CapSense_DEFAULT_IDACB_RANGE | \
                                                                     CapSense_DEFAULT_IDACB_POLARITY | \
                                                                     CapSense_DEFAULT_IDACB_BALL_MODE | \
                                                                     CapSense_DEFAULT_IDACB_LEG1_MODE | \
                                                                    (CapSense_CSD_IDACB_LEG2_MODE_GP_STATIC << CapSense_CSD_IDACB_LEG2_MODE_POS))

#define CapSense_IDAC_MOD_CALIBRATION_CFG                   (CapSense_DEFAULT_IDACA_RANGE | \
                                                                     CapSense_DEFAULT_IDACA_POLARITY | \
                                                                     CapSense_DEFAULT_IDACA_BALL_MODE | \
                                                                    (CapSense_CSD_IDACA_LEG1_MODE_CSD << CapSense_CSD_IDACA_LEG1_MODE_POS) | \
                                                                     CapSense_CSD_IDACA_LEG1_EN_MSK | \
                                                                    (CapSense_CSD_IDACA_LEG2_MODE_GP_STATIC << CapSense_CSD_IDACA_LEG2_MODE_POS))

#define CapSense_IDAC_COMP_CALIBRATION_CFG                  (CapSense_DEFAULT_IDACB_RANGE | \
                                                                     CapSense_DEFAULT_IDACB_POLARITY | \
                                                                     CapSense_DEFAULT_IDACB_BALL_MODE | \
                                                                    (CapSense_CSD_IDACB_LEG1_MODE_GP_STATIC << CapSense_CSD_IDACB_LEG1_MODE_POS) | \
                                                                    (CapSense_CSD_IDACB_LEG2_MODE_GP_STATIC << CapSense_CSD_IDACB_LEG2_MODE_POS))

/***************************************
* Global software variables
***************************************/

extern uint32 CapSense_configCsd;

#if (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN)
    extern uint8 CapSense_badConversionsNum;
#endif /* (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN) */

#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    /* Stores IDAC and raw count that corresponds to a sensor with maximum Cp within a widget */
    extern uint8 CapSense_calibratedIdac;
    extern uint16 CapSense_calibratedRawcount;
    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
        extern uint8 CapSense_calibratedIdacRow;
        extern uint16 CapSense_calibratedRawcountRow;
    #endif /*(CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

/***************************************
* Function Prototypes
**************************************/

/* Interrupt handler */
extern void CapSense_CSDPostSingleScan(void);
extern void CapSense_CSDPostMultiScan(void);

#if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
    extern void CapSense_CSDPostMultiScanGanged(void);
#endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */


/**
* \cond SECTION_CYSENSE_LOW_LEVEL
* \addtogroup group_cysense_low_level
* \{
*/

void CapSense_CSDSetupWidget(uint32 widgetId);
void CapSense_CSDSetupWidgetExt(uint32 widgetId, uint32 sensorId);
void CapSense_CSDScan(void);
void CapSense_CSDScanExt(void);
#if ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
     (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))
    cy_status CapSense_CSDCalibrateWidget(uint32 widgetId, uint32 target);
#endif /* ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
           (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)) */
void CapSense_CSDConnectSns(CapSense_FLASH_IO_STRUCT const *snsAddrPtr);
void CapSense_CSDDisconnectSns(CapSense_FLASH_IO_STRUCT const *snsAddrPtr);

/** \}
* \endcond */

/*****************************************************
* Function Prototypes - internal Low Level functions
*****************************************************/
/**
* \cond SECTION_CYSENSE_INTERNAL
* \addtogroup group_cysense_internal
* \{
*/

void CapSense_SsCSDInitialize(void);
void CapSense_SsCSDStartSample(void);
void CapSense_SsCSDSetUpIdacs(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt);
void CapSense_SsCSDConfigClock(void);
void CapSense_SsCSDElectrodeCheck(void);
#if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
     (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
    void CapSense_SsCSDDisableShieldElectrodes(void);
#endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
           (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */
uint32 CapSense_SsCSDGetNumberOfConversions(uint32 snsClkDivider, uint32 resolution, uint32 snsClkSrc);
void CapSense_SsCSDCalculateScanDuration(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt);
void CapSense_SsCSDConnectSensorExt(uint32 widgetId, uint32 sensorId);
void CapSense_SsCSDDisconnectSnsExt(uint32 widgetId, uint32 sensorId);

#if ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))
#endif /* ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
           (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
           (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)) */

/** \}
* \endcond */

/***************************************
* Global software variables
***************************************/
extern uint32 CapSense_configCsd;
/* Interrupt handler */
extern void CapSense_CSDPostSingleScan(void);
extern void CapSense_CSDPostMultiScan(void);
#if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
extern void CapSense_CSDPostMultiScanGanged(void);
#endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
#if (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN)
    extern uint8 CapSense_badConversionsNum;
#endif /* (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN) */

#endif /* End CY_SENSE_CapSense_SENSINGCSD_LL_H */


/* [] END OF FILE */
