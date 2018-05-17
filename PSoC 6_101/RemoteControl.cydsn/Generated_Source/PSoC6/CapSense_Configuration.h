/*******************************************************************************
* \file       CapSense_Configuration.h
* \version    2.0
*
* \brief
*   This file provides the customizer parameters definitions.
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

#if !defined(CY_SENSE_CapSense_CONFIGURATION_H)
#define CY_SENSE_CapSense_CONFIGURATION_H

#include "syslib/cy_syslib.h"
#include "cyfitter.h"


/*******************************************************************************
* Customizer-generated defines
*******************************************************************************/
#define CapSense_ENABLE                             (1u)
#define CapSense_DISABLE                            (0u)

/*******************************************************************************
* Creator-global defines
*******************************************************************************/
#define CapSense_2000_MV                            (2000u)

#ifdef CYDEV_VDDA_MV
    #define CapSense_CYDEV_VDDA_MV                  (CYDEV_VDDA_MV)
#else
    #ifdef CYDEV_VDD_MV
        #define CapSense_CYDEV_VDDA_MV              (CYDEV_VDD_MV)
    #endif
#endif

#define CapSense_BAD_CONVERSIONS_NUM                (1u)
#define CapSense_RESAMPLING_CYCLES_MAX_NUMBER       (1u)

/*******************************************************************************
* Enabled Scan Methods
*******************************************************************************/
#define CapSense_CSD_EN                   (1u)
#define CapSense_CSX_EN                   (1u)
#define CapSense_CSD_CSX_EN               (CapSense_CSD_EN && CapSense_CSX_EN)

/*******************************************************************************
* Definitions for number of widgets and sensors
*******************************************************************************/
#define CapSense_TOTAL_WIDGETS            (3u)
#define CapSense_TOTAL_CSD_WIDGETS        (1u)
#define CapSense_TOTAL_CSD_SENSORS        (5u)
#define CapSense_TOTAL_CSX_WIDGETS        (2u)
#define CapSense_TOTAL_CSX_NODES          (2u)

/*******************************************************************************
* Total number of CSD sensors + CSX nodes
*******************************************************************************/
#define CapSense_TOTAL_SENSORS            (CapSense_TOTAL_CSD_SENSORS + \
                                                   CapSense_TOTAL_CSX_NODES)

/*******************************************************************************
* Total number of scan slots (used only when dual-channel scan is enabled)
*******************************************************************************/
#define CapSense_TOTAL_SCAN_SLOTS         (9u)

/*******************************************************************************
* Defines widget IDs
*******************************************************************************/
#define CapSense_LINEARSLIDER0_WDGT_ID          (0u)
#define CapSense_BUTTON0_WDGT_ID                (1u)
#define CapSense_BUTTON1_WDGT_ID                (2u)

/*******************************************************************************
* Defines sensor IDs
*******************************************************************************/

/* LinearSlider0 sensor names */
#define CapSense_LINEARSLIDER0_SNS0_ID          (0u)
#define CapSense_LINEARSLIDER0_SNS1_ID          (1u)
#define CapSense_LINEARSLIDER0_SNS2_ID          (2u)
#define CapSense_LINEARSLIDER0_SNS3_ID          (3u)
#define CapSense_LINEARSLIDER0_SNS4_ID          (4u)

/* Button0 sensor names */
#define CapSense_BUTTON0_SNS0_ID                (0u)

/* Button0 node names */
#define CapSense_BUTTON0_RX0_TX0_ID             (0u)

/* Button0 sensor element IDs */
#define CapSense_BUTTON0_RX0_ID                 (0u)
#define CapSense_BUTTON0_TX0_ID                 (1u)

/* Button1 sensor names */
#define CapSense_BUTTON1_SNS0_ID                (0u)

/* Button1 node names */
#define CapSense_BUTTON1_RX0_TX0_ID             (0u)

/* Button1 sensor element IDs */
#define CapSense_BUTTON1_RX0_ID                 (0u)
#define CapSense_BUTTON1_TX0_ID                 (1u)



/*******************************************************************************
* Enabled widget types
*******************************************************************************/
#define CapSense_BUTTON_WIDGET_EN         (1u)
#define CapSense_SLIDER_WIDGET_EN         (1u)
#define CapSense_MATRIX_WIDGET_EN         (0u)
#define CapSense_PROXIMITY_WIDGET_EN      (0u)
#define CapSense_TOUCHPAD_WIDGET_EN       (0u)

#define CapSense_CSD_MATRIX_WIDGET_EN     (0u)
#define CapSense_CSD_TOUCHPAD_WIDGET_EN   (0u)

#define CapSense_CSX_MATRIX_WIDGET_EN     (0u)
#define CapSense_CSX_TOUCHPAD_WIDGET_EN   (0u)

/*******************************************************************************
* Centroid APIs
*******************************************************************************/
#define CapSense_CENTROID_EN              (1u)
#define CapSense_TOTAL_DIPLEXED_SLIDERS   (0u)
#define CapSense_TOTAL_LINEAR_SLIDERS     (1u)
#define CapSense_TOTAL_RADIAL_SLIDERS     (0u)
#define CapSense_TOTAL_TOUCHPADS          (0u)
#define CapSense_MAX_CENTROID_LENGTH      (5u)
#define CapSense_SLIDER_MULT_METHOD       (0u)
#define CapSense_TOUCHPAD_MULT_METHOD     (0u)

/*******************************************************************************
* Enabled sensor types
*******************************************************************************/
#define CapSense_REGULAR_SENSOR_EN        (1u)
#define CapSense_PROXIMITY_SENSOR_EN      (0u)

/*******************************************************************************
* Sensor ganging
*******************************************************************************/
#define CapSense_GANGED_SNS_EN            (0u)
#define CapSense_CSD_GANGED_SNS_EN        (0u)
#define CapSense_CSX_GANGED_SNS_EN        (0u)

/*******************************************************************************
* Max number of sensors used among all the widgets
*******************************************************************************/
#define CapSense_MAX_SENSORS_PER_WIDGET   (5u)
#define CapSense_MAX_SENSORS_PER_5X5_TOUCHPAD (1u)

/*******************************************************************************
* Total number of all used electrodes (NOT unique)
*******************************************************************************/
#define CapSense_TOTAL_ELECTRODES         (9u)
/* Obsolete */
#define CapSense_TOTAL_SENSOR_IOS         CapSense_TOTAL_ELECTRODES

/*******************************************************************************
* Total number of used physical IOs (unique)
*******************************************************************************/
#define CapSense_TOTAL_IO_CNT             (8u)

/*******************************************************************************
* Array length for widget status registers
*******************************************************************************/
#define CapSense_WDGT_STATUS_WORDS        \
                        (((uint8)((CapSense_TOTAL_WIDGETS - 1u) / 32u)) + 1u)


/*******************************************************************************
* Auto-tuning mode selection
*******************************************************************************/
#define CapSense_CSD_SS_DIS         (0x00ul)
#define CapSense_CSD_SS_HW_EN       (0x01ul)
#define CapSense_CSD_SS_TH_EN       (0x02ul)
#define CapSense_CSD_SS_HWTH_EN     (CapSense_CSD_SS_HW_EN | \
                                             CapSense_CSD_SS_TH_EN)

#define CapSense_CSD_AUTOTUNE       CapSense_CSD_SS_HWTH_EN


/*******************************************************************************
* General settings
*******************************************************************************/

#define CapSense_AUTO_RESET_METHOD_LEGACY (0u)
#define CapSense_AUTO_RESET_METHOD_SAMPLE (1u)

#define CapSense_MULTI_FREQ_SCAN_EN       (0u)
#define CapSense_SENSOR_AUTO_RESET_EN     (0u)
#define CapSense_SENSOR_AUTO_RESET_METHOD (0u)
#define CapSense_NUM_CENTROIDS            (1u)
#define CapSense_4PTS_LOCAL_MAX_EN        (0u)
#define CapSense_OFF_DEBOUNCE_EN          (0u)
#define CapSense_CUSTOM_DS_RAM_SIZE       (0u)

/* Defines power status of HW IP block after scanning */
#define CapSense_BLOCK_OFF_AFTER_SCAN_EN  (0u)

/* Defines number of scan frequencies */
#if (CapSense_DISABLE != CapSense_MULTI_FREQ_SCAN_EN)
    #define CapSense_NUM_SCAN_FREQS       (3u)
#else
    #define CapSense_NUM_SCAN_FREQS       (1u)
#endif /* #if (CapSense_DISABLE != CapSense_MULTI_FREQ_SCAN_EN) */

/* Data size for thresholds / low baseline reset */
#define CapSense_SIZE_8BITS               (8u)
#define CapSense_SIZE_16BITS              (16u)

#define CapSense_THRESHOLD_SIZE           CapSense_SIZE_16BITS
typedef uint16 CapSense_THRESHOLD_TYPE;

#if (CapSense_AUTO_RESET_METHOD_LEGACY == CapSense_SENSOR_AUTO_RESET_METHOD)
    #define CapSense_LOW_BSLN_RST_SIZE        CapSense_SIZE_8BITS
    typedef uint8 CapSense_LOW_BSLN_RST_TYPE;
#else
    #define CapSense_LOW_BSLN_RST_SIZE    (16u)
    typedef uint16 CapSense_LOW_BSLN_RST_TYPE;
#endif /* #if (CapSense_AUTO_RESET_METHOD_LEGACY == CapSense_SENSOR_AUTO_RESET_METHOD) */

/* Coefficient to define touch threshold for proximity sensors */
#define CapSense_PROX_TOUCH_COEFF         (300u)

/*******************************************************************************
* General Filter Constants
*******************************************************************************/

/* Baseline algorithm options */
#define CapSense_IIR_BASELINE                 (0u)
#define CapSense_BUCKET_BASELINE              (1u)

#define CapSense_BASELINE_TYPE                CapSense_IIR_BASELINE

/* IIR baseline filter algorithm for regular sensors*/
#define CapSense_REGULAR_IIR_BL_TYPE          CapSense_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for regular sensors */
#define CapSense_REGULAR_IIR_BL_N             (1u)
#define CapSense_REGULAR_IIR_BL_SHIFT         (8u)

/* IIR baseline filter algorithm for proximity sensors*/
#define CapSense_PROX_IIR_BL_TYPE             CapSense_IIR_FILTER_PERFORMANCE

/* IIR baseline coefficients for proximity sensors */
#define CapSense_PROX_IIR_BL_N                (1u)
#define CapSense_PROX_IIR_BL_SHIFT            (8u)


/* IIR filter constants */
#define CapSense_IIR_COEFFICIENT_K            (256u)

/* IIR filter type */
#define CapSense_IIR_FILTER_STANDARD          (1u)
#define CapSense_IIR_FILTER_PERFORMANCE       (2u)
#define CapSense_IIR_FILTER_MEMORY            (3u)

/* Raw count filters */
#define CapSense_RC_FILTER_EN                 (CapSense_REGULAR_RC_FILTER_EN || CapSense_PROX_RC_FILTER_EN)

/* Regular sensor raw count filters */
#define CapSense_REGULAR_RC_FILTER_EN         (0u)
#define CapSense_REGULAR_RC_IIR_FILTER_EN     (0u)
#define CapSense_REGULAR_RC_MEDIAN_FILTER_EN  (0u)
#define CapSense_REGULAR_RC_AVERAGE_FILTER_EN (0u)
#define CapSense_REGULAR_RC_CUSTOM_FILTER_EN  (0u)
#define CapSense_REGULAR_RC_ALP_FILTER_EN     (0u)

/* Proximity sensor raw count filters */
#define CapSense_PROX_RC_FILTER_EN            (0u)
#define CapSense_PROX_RC_IIR_FILTER_EN        (0u)
#define CapSense_PROX_RC_MEDIAN_FILTER_EN     (0u)
#define CapSense_PROX_RC_AVERAGE_FILTER_EN    (0u)
#define CapSense_PROX_RC_CUSTOM_FILTER_EN     (0u)
#define CapSense_PROX_RC_ALP_FILTER_EN        (0u)

#define CapSense_ALP_FILTER_EN                (0u)
#define CapSense_REGULAR_RC_ALP_FILTER_COEFF  (2u)
#define CapSense_PROX_RC_ALP_FILTER_COEFF     (2u)

/* IIR raw count filter algorithm for regular sensors */
#define CapSense_REGULAR_IIR_RC_TYPE          (CapSense_IIR_FILTER_STANDARD)

/* IIR raw count filter coefficients for regular sensors */
#define CapSense_REGULAR_IIR_RC_N             (128u)
#define CapSense_REGULAR_IIR_RC_SHIFT         (0u)

/* IIR raw count filter algorithm for proximity sensors*/
#define CapSense_PROX_IIR_RC_TYPE             (CapSense_IIR_FILTER_STANDARD)

/* IIR raw count filter coefficients for proximity sensors */
#define CapSense_PROX_IIR_RC_N                (128u)
#define CapSense_PROX_IIR_RC_SHIFT            (0u)

/* Median filter constants */

/* Order of regular sensor median filter */
#define CapSense_REGULAR_MEDIAN_LEN           (2u)

/* Order of proximity sensor median filter */
#define CapSense_PROX_MEDIAN_LEN              (2u)

/* Average filter constants*/
#define CapSense_AVERAGE_FILTER_LEN_2         (1u)
#define CapSense_AVERAGE_FILTER_LEN_4         (3u)

/* Order of regular sensor average filter */
#define CapSense_REGULAR_AVERAGE_LEN          (CapSense_AVERAGE_FILTER_LEN_4)

/* Order of proximity sensor average filter */
#define CapSense_PROX_AVERAGE_LEN             (CapSense_AVERAGE_FILTER_LEN_4)

/* Widget baseline coefficient enable */
#define CapSense_WD_BSLN_COEFF_EN             (0u)

/* Centroid position filters */
#define CapSense_POSITION_FILTER_EN           (0u)
#define CapSense_POS_MEDIAN_FILTER_EN         (0u)
#define CapSense_POS_IIR_FILTER_EN            (0u)
#define CapSense_POS_ADAPTIVE_IIR_FILTER_EN   (0u)
#define CapSense_POS_AVERAGE_FILTER_EN        (0u)
#define CapSense_POS_JITTER_FILTER_EN         (0u)
#define CapSense_BALLISTIC_MULTIPLIER_EN      (0u)
#define CapSense_CENTROID_3X3_CSD_EN          (0u)
#define CapSense_CENTROID_5X5_CSD_EN          (0u)
#define CapSense_CSD_5X5_MAX_FINGERS          (1u)

#define CapSense_POS_IIR_COEFF                (128u)
#define CapSense_POS_IIR_RESET_RADIAL_SLIDER  (35u)

#define CapSense_CSX_TOUCHPAD_UNDEFINED       (40u)

/* IDAC options */
#define CapSense_IDAC_GAIN_LOW                (0uL)
#define CapSense_IDAC_GAIN_MEDIUM             (1uL)
#define CapSense_IDAC_GAIN_HIGH               (2uL)

#define CapSense_IDAC_SOURCING                (0u)
#define CapSense_IDAC_SINKING                 (1u)

/* Shield tank capacitor precharge source */
#define CapSense_CSH_PRECHARGE_VREF           (0u)
#define CapSense_CSH_PRECHARGE_IO_BUF         (1u)

/* Shield electrode delay */
#define CapSense_NO_DELAY                     (0u)
#define CapSense_SH_DELAY_5NS                 (1u)
#define CapSense_SH_DELAY_10NS                (2u)
#define CapSense_SH_DELAY_20NS                (3u)

/* Inactive sensor connection options */
#define CapSense_SNS_CONNECTION_GROUND        (1u)
#define CapSense_SNS_CONNECTION_HIGHZ         (2u)
#define CapSense_SNS_CONNECTION_SHIELD        (3u)

/* Sense clock selection options */
#define CapSense_CLK_SOURCE_DIRECT            (0x00000000Lu)

#define CapSense_CLK_SOURCE_SSC1              (0x01u)
#define CapSense_CLK_SOURCE_SSC2              (0x02u)
#define CapSense_CLK_SOURCE_SSC3              (0x03u)
#define CapSense_CLK_SOURCE_SSC4              (0x04u)

#define CapSense_CLK_SOURCE_PRS8              (0x05u)
#define CapSense_CLK_SOURCE_PRS12             (0x06u)
#define CapSense_CLK_SOURCE_PRSAUTO           (0xFFu)

/* Defines scan resolutions */
#define CapSense_RES6BIT                      (6u)
#define CapSense_RES7BIT                      (7u)
#define CapSense_RES8BIT                      (8u)
#define CapSense_RES9BIT                      (9u)
#define CapSense_RES10BIT                     (10u)
#define CapSense_RES11BIT                     (11u)
#define CapSense_RES12BIT                     (12u)
#define CapSense_RES13BIT                     (13u)
#define CapSense_RES14BIT                     (14u)
#define CapSense_RES15BIT                     (15u)
#define CapSense_RES16BIT                     (16u)

/* Initialization switch resistance */
#define CapSense_INIT_SW_RES_LOW              (0x00000000Lu)
#define CapSense_INIT_SW_RES_MEDIUM           (0x00000001Lu)
#define CapSense_INIT_SW_RES_HIGH             (0x00000002Lu)

/* Initialization switch resistance */
#define CapSense_SCAN_SW_RES_LOW              (0x00000000Lu)
#define CapSense_SCAN_SW_RES_MEDIUM           (0x00000001Lu)
#define CapSense_SCAN_SW_RES_HIGH             (0x00000002Lu)

/* CSD shield switch resistance */
#define CapSense_SHIELD_SW_RES_LOW            (0x00000000Lu)
#define CapSense_SHIELD_SW_RES_MEDIUM         (0x00000001Lu)
#define CapSense_SHIELD_SW_RES_HIGH           (0x00000002Lu)
#define CapSense_SHIELD_SW_RES_LOW_EMI        (0x00000003Lu)

/* CSD shield switch resistance */
#define CapSense_INIT_SHIELD_SW_RES_LOW       (0x00000000Lu)
#define CapSense_INIT_SHIELD_SW_RES_MEDIUM    (0x00000001Lu)
#define CapSense_INIT_SHIELD_SW_RES_HIGH      (0x00000002Lu)
#define CapSense_INIT_SHIELD_SW_RES_LOW_EMI   (0x00000003Lu)

/* CSD shield switch resistance */
#define CapSense_SCAN_SHIELD_SW_RES_LOW       (0x00000000Lu)
#define CapSense_SCAN_SHIELD_SW_RES_MEDIUM    (0x00000001Lu)
#define CapSense_SCAN_SHIELD_SW_RES_HIGH      (0x00000002Lu)
#define CapSense_SCAN_SHIELD_SW_RES_LOW_EMI   (0x00000003Lu)

/* Vref source */
#define CapSense_VREF_SRSS                    (0x00000000Lu)
#define CapSense_VREF_PASS                    (0x00000001Lu)

/* Iref source */
#define CapSense_IREF_SRSS                    (0x00000000Lu)
#define CapSense_IREF_PASS                    (0x00000001Lu)

/* Sensing method */
#define CapSense_SENSING_LEGACY               (0x00000000Lu)
#define CapSense_SENSING_LOW_EMI              (0x00000001Lu)
#define CapSense_SENSING_FULL_WAVE            (0x00000002Lu)


/*******************************************************************************
* CSD/CSX Common settings
*******************************************************************************/

#define CapSense_CSD_VREF_SOURCE              (CapSense_VREF_SRSS)
#define CapSense_CSD_IREF_SOURCE              (CapSense_IREF_SRSS)

#define CapSense_LP_MODE_EN                   (0u)

#define CapSense_BLOCK_ANALOG_WAKEUP_DELAY_US (0u)

/*******************************************************************************
* CSD Specific settings
*******************************************************************************/

/* CSD scan method settings */
#define CapSense_CSD_IDAC_AUTOCAL_EN          (1u)
#define CapSense_CSD_IDAC_GAIN                (CapSense_IDAC_GAIN_HIGH)
#define CapSense_CSD_SHIELD_EN                (0u)
#define CapSense_CSD_SHIELD_TANK_EN           (0u)
#define CapSense_CSD_CSH_PRECHARGE_SRC        (CapSense_CSH_PRECHARGE_VREF)
#define CapSense_CSD_SHIELD_DELAY             (CapSense_NO_DELAY)
#define CapSense_CSD_TOTAL_SHIELD_COUNT       (0u)
#define CapSense_CSD_SCANSPEED_DIVIDER        (1u)
#define CapSense_CSD_COMMON_SNS_CLK_EN        (0u)
#define CapSense_CSD_SNS_CLK_SOURCE           (CapSense_CLK_SOURCE_PRSAUTO)
#define CapSense_CSD_SNS_CLK_DIVIDER          (16u)
#define CapSense_CSD_INACTIVE_SNS_CONNECTION  (CapSense_SNS_CONNECTION_GROUND)
#define CapSense_CSD_IDAC_COMP_EN             (1u)
#define CapSense_CSD_IDAC_CONFIG              (CapSense_IDAC_SOURCING)
#define CapSense_CSD_RAWCOUNT_CAL_LEVEL       (85u)
#define CapSense_CSD_DUALIDAC_LEVEL           (50u)
#define CapSense_CSD_PRESCAN_SETTLING_TIME    (5u)
#define CapSense_CSD_SNSCLK_R_CONST           (1000u)
#define CapSense_CSD_VREF_MV                  (2133u)

/* CSD settings */
#define CapSense_CSD_ANALOG_STARTUP_DELAY_US  (23u)
#define CapSense_CSD_FINE_INIT_TIME           (10u)
#define CapSense_CSD_DEDICATED_IDAC_COMP_EN   (1u)
#define CapSense_CSD_AUTO_ZERO_EN             (0u)
#define CapSense_CSD_AUTO_ZERO_TIME           (15Lu)
#define CapSense_CSD_NOISE_METRIC_EN          (0u)
#define CapSense_CSD_NOISE_METRIC_TH          (1Lu)
#define CapSense_CSD_INIT_SWITCH_RES          (CapSense_INIT_SW_RES_MEDIUM)
#define CapSense_CSD_SENSING_METHOD           (0)
#define CapSense_CSD_SHIELD_SWITCH_RES        (CapSense_SHIELD_SW_RES_MEDIUM)
#define CapSense_CSD_GAIN                     (11Lu)

#define CapSense_CSD_F1PM_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_F1PM_SWITCH_RES_LE_SCAN     (0)
#define CapSense_CSD_F2PT_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_F2PT_SWITCH_RES_LE_SCAN     (0)
#define CapSense_CSD_HCAG_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_HCAG_SWITCH_RES_LE_SCAN     (3)
#define CapSense_CSD_HCAV_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_HCAV_SWITCH_RES_LE_SCAN     (0)
#define CapSense_CSD_HCBG_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_HCBG_SWITCH_RES_LE_SCAN     (3)
#define CapSense_CSD_HCBV_SWITCH_RES_LE_INIT     (1)
#define CapSense_CSD_HCBV_SWITCH_RES_LE_SCAN     (3)

#define CapSense_CSD_SENSE_WIDTH_LE              (10)

#define CapSense_CSD_MFS_DIVIDER_OFFSET_F1       (1u)
#define CapSense_CSD_MFS_DIVIDER_OFFSET_F2       (2u)

/*******************************************************************************
* CSX Specific settings
*******************************************************************************/

/* CSX scan method settings */
#define CapSense_CSX_SCANSPEED_DIVIDER        (1u)
#define CapSense_CSX_COMMON_TX_CLK_EN         (0u)
#define CapSense_CSX_TX_CLK_SOURCE            (CapSense_CLK_SOURCE_PRSAUTO)
#define CapSense_CSX_TX_CLK_DIVIDER           (167u)
#define CapSense_CSX_MAX_FINGERS              (1u)
#define CapSense_CSX_MAX_LOCAL_PEAKS          (5u)
#define CapSense_CSX_IDAC_AUTOCAL_EN          (1u)
#define CapSense_CSX_IDAC_BITS_USED           (7u)
#define CapSense_CSX_RAWCOUNT_CAL_LEVEL       (40u)
#define CapSense_CSX_IDAC_GAIN                (CapSense_IDAC_GAIN_MEDIUM)
#define CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES (0u)
#define CapSense_CSX_MULTIPHASE_TX_EN         (0u)
#define CapSense_CSX_MAX_TX_PHASE_LENGTH      (0u)

/* CSX settings */
#define CapSense_CSX_ANALOG_STARTUP_DELAY_US  (23u)
#define CapSense_CSX_AUTO_ZERO_EN             (0u)
#define CapSense_CSX_AUTO_ZERO_TIME           (15u)
#define CapSense_CSX_FINE_INIT_TIME           (4u)
#define CapSense_CSX_NOISE_METRIC_EN          (0u)
#define CapSense_CSX_NOISE_METRIC_TH          (1u)
#define CapSense_CSX_INIT_SWITCH_RES          (CapSense_INIT_SW_RES_MEDIUM)
#define CapSense_CSX_SCAN_SWITCH_RES          (CapSense_SCAN_SW_RES_LOW)
#define CapSense_CSX_INIT_SHIELD_SWITCH_RES   (CapSense_INIT_SHIELD_SW_RES_HIGH)
#define CapSense_CSX_SCAN_SHIELD_SWITCH_RES   (CapSense_SCAN_SHIELD_SW_RES_LOW)

#define CapSense_CSX_MFS_DIVIDER_OFFSET_F1    (1u)
#define CapSense_CSX_MFS_DIVIDER_OFFSET_F2    (2u)

/* Gesture parameters */
#define CapSense_GES_GLOBAL_EN                (0u)


/*******************************************************************************
* Global Parameter Definitions
*******************************************************************************/

/* RAM Global Parameters Definitions */
#define CapSense_CONFIG_ID                      (0x523Au)
#define CapSense_DEVICE_ID                      (0x0900u)
#define CapSense_HW_CLOCK                       (0x186Au)
#define CapSense_CSD0_CONFIG                    (0x0008u)

/*******************************************************************************
* LinearSlider0 initialization values for FLASH data structure
*******************************************************************************/
#define CapSense_LINEARSLIDER0_STATIC_CONFIG    (10241u)
#define CapSense_LINEARSLIDER0_NUM_SENSORS      (5u)
#define CapSense_LINEARSLIDER0_X_RESOLUTION     (100u)
#define CapSense_LINEARSLIDER0_X_CENT_MULT      ((CapSense_LINEARSLIDER0_X_RESOLUTION * 256u) / \
                                                 (CapSense_LINEARSLIDER0_NUM_SENSORS - 1u))
#define CapSense_LINEARSLIDER0_IIR_FILTER_COEFF (128u)

/*******************************************************************************
* LinearSlider0 initialization values for RAM data structure
*******************************************************************************/
#define CapSense_LINEARSLIDER0_RESOLUTION       (CapSense_RES12BIT)
#define CapSense_LINEARSLIDER0_FINGER_TH        (100u)
#define CapSense_LINEARSLIDER0_NOISE_TH         (40u)
#define CapSense_LINEARSLIDER0_NNOISE_TH        (40u)
#define CapSense_LINEARSLIDER0_HYSTERESIS       (10u)
#define CapSense_LINEARSLIDER0_ON_DEBOUNCE      (3u)
#define CapSense_LINEARSLIDER0_LOW_BSLN_RST     (30u)
#define CapSense_LINEARSLIDER0_IDAC_MOD0        (32u)
#define CapSense_LINEARSLIDER0_SNS_CLK          (4u)
#define CapSense_LINEARSLIDER0_SNS_CLK_SOURCE   (0u)
#define CapSense_LINEARSLIDER0_FINGER_CAP       (160u)
#define CapSense_LINEARSLIDER0_SIGPFC           (0u)
#define CapSense_LINEARSLIDER0_POSITION         (0xFFFFu)

/*******************************************************************************
* Button0 initialization values for FLASH data structure
*******************************************************************************/
#define CapSense_BUTTON0_STATIC_CONFIG          (10249u)
#define CapSense_BUTTON0_NUM_SENSORS            (1u)

/*******************************************************************************
* Button0 initialization values for RAM data structure
*******************************************************************************/
#define CapSense_BUTTON0_RESOLUTION             (100u)
#define CapSense_BUTTON0_FINGER_TH              (100u)
#define CapSense_BUTTON0_NOISE_TH               (40u)
#define CapSense_BUTTON0_NNOISE_TH              (40u)
#define CapSense_BUTTON0_HYSTERESIS             (10u)
#define CapSense_BUTTON0_ON_DEBOUNCE            (3u)
#define CapSense_BUTTON0_LOW_BSLN_RST           (30u)
#define CapSense_BUTTON0_IDAC_MOD0              (32u)
#define CapSense_BUTTON0_SNS_CLK                (167u)
#define CapSense_BUTTON0_SNS_CLK_SOURCE         (0u)
#define CapSense_BUTTON0_FINGER_CAP             (160u)
#define CapSense_BUTTON0_SIGPFC                 (0u)

/*******************************************************************************
* Button1 initialization values for FLASH data structure
*******************************************************************************/
#define CapSense_BUTTON1_STATIC_CONFIG          (10241u)
#define CapSense_BUTTON1_NUM_SENSORS            (1u)

/*******************************************************************************
* Button1 initialization values for RAM data structure
*******************************************************************************/
#define CapSense_BUTTON1_RESOLUTION             (100u)
#define CapSense_BUTTON1_FINGER_TH              (100u)
#define CapSense_BUTTON1_NOISE_TH               (40u)
#define CapSense_BUTTON1_NNOISE_TH              (40u)
#define CapSense_BUTTON1_HYSTERESIS             (10u)
#define CapSense_BUTTON1_ON_DEBOUNCE            (3u)
#define CapSense_BUTTON1_LOW_BSLN_RST           (30u)
#define CapSense_BUTTON1_IDAC_MOD0              (32u)
#define CapSense_BUTTON1_SNS_CLK                (167u)
#define CapSense_BUTTON1_SNS_CLK_SOURCE         (0u)
#define CapSense_BUTTON1_FINGER_CAP             (160u)
#define CapSense_BUTTON1_SIGPFC                 (0u)

/* RAM Sensor Parameters Definitions */
#define CapSense_LINEARSLIDER0_SNS0_IDAC_COMP0  (32u)
#define CapSense_LINEARSLIDER0_SNS1_IDAC_COMP0  (32u)
#define CapSense_LINEARSLIDER0_SNS2_IDAC_COMP0  (32u)
#define CapSense_LINEARSLIDER0_SNS3_IDAC_COMP0  (32u)
#define CapSense_LINEARSLIDER0_SNS4_IDAC_COMP0  (32u)
#define CapSense_BUTTON0_RX0_IDAC_COMP0         (32u)
#define CapSense_BUTTON1_RX0_IDAC_COMP0         (32u)



/*******************************************************************************
* ADC Specific Macros
*******************************************************************************/
#define CapSense_ADC_RES8BIT                  (8u)
#define CapSense_ADC_RES10BIT                 (10u)

#define CapSense_ADC_FULLRANGE_MODE           (0u)
#define CapSense_ADC_VREF_MODE                (1u)

#define CapSense_ADC_MIN_CHANNELS             (1u)
#define CapSense_ADC_EN                       (0u)
#define CapSense_ADC_STANDALONE_EN            (0u)
#define CapSense_ADC_TOTAL_CHANNELS           (0u)
#define CapSense_ADC_RESOLUTION               (CapSense_ADC_RES10BIT)
#define CapSense_ADC_AMUXB_INPUT_EN           (0u)
#define CapSense_ADC_SELECT_AMUXB_CH          (0u)
#define CapSense_ADC_AZ_EN                    (1Lu)
#define CapSense_ADC_AZ_TIME                  (5u)
#define CapSense_ADC_VREF_MV                  (2133u)
#define CapSense_ADC_GAIN                     (11Lu)
#define CapSense_ADC_IDAC_DEFAULT             (31u)
#define CapSense_ADC_MODCLK_DIV_DEFAULT       (1u)
#define CapSense_ADC_MEASURE_MODE             (CapSense_ADC_FULLRANGE_MODE)
#define CapSense_ADC_ANALOG_STARTUP_DELAY_US  (23u)

/*******************************************************************************
* Built-In Self-Test Configuration
*******************************************************************************/
#define CapSense_SELF_TEST_EN                   (0Lu)
#define CapSense_TST_GLOBAL_CRC_EN              (0Lu)
#define CapSense_TST_WDGT_CRC_EN                (0Lu)
#define CapSense_TST_BSLN_DUPLICATION_EN        (0Lu)
#define CapSense_TST_BSLN_RAW_OUT_RANGE_EN      (0Lu)
#define CapSense_TST_SNS_SHORT_EN               (0Lu)
#define CapSense_TST_SNS2SNS_SHORT_EN           (0Lu)
#define CapSense_TST_SNS_CAP_EN                 (0Lu)
#define CapSense_TST_SH_CAP_EN                  (0Lu)
#define CapSense_TST_EXTERNAL_CAP_EN            (0Lu)
#define CapSense_TST_INTERNAL_CAP_EN            (0Lu)
#define CapSense_TST_VDDA_EN                    (0Lu)
#define CapSense_TST_FINE_INIT_TIME             (1Lu)


#define CapSense_TST_ANALOG_STARTUP_DELAY_US    (23u)

/*******************************************************************************
* Gesture Configuration
*******************************************************************************/
#define CapSense_TIMESTAMP_INTERVAL             (1Lu)
#define CapSense_GESTURE_EN_WIDGET_ID           (0Lu)
#define CapSense_BALLISTIC_EN_WIDGET_ID         (0Lu)


#endif /* CY_SENSE_CapSense_CONFIGURATION_H */


/* [] END OF FILE */
