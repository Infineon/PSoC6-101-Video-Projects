/***************************************************************************//**
* \file cy_ble_hal_pvt.c
* \version 2.0
*
* \brief
*  This file contains the source code for the HAL section of the BLE Middleware.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_ble_event_handler.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cy_ble_hal_pvt.h"
#include "ipc/cy_ipc_sema.h"
   
#if (CY_BLE_HOST_CORE)
cy_ble_intr_callback_t Cy_BLE_InterruptCallback;    
#endif /* (CY_BLE_HOST_CORE) */

#if (CY_BLE_HOST_CONTR_CORE)

#if (CY_BLE_STACK_MODE_HOST_UART)


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_Start
****************************************************************************//**
*
*  Enables the platform UART TX and RX interrupts and then enables the UART.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_Start(void)
{
    /* Setup ISR */
    Cy_SysInt_Init(cy_ble_configPtr->uartHostIsrConfig, &Cy_BLE_HAL_HOST_UART_Interrupt);
    NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->uartHostIsrConfig->intrSrc);

    (void)Cy_SCB_UART_Init(cy_ble_configPtr->uartHostHw, cy_ble_configPtr->uartHostConfig, NULL);
    /* Configure interrupt sources. */
    Cy_SCB_SetRxInterruptMask(cy_ble_configPtr->uartHostHw, CY_SCB_RX_INTR_NOT_EMPTY | CY_SCB_RX_INTR_OVERFLOW |
                              CY_SCB_RX_INTR_UNDERFLOW);
    Cy_SCB_SetTxInterruptMask(cy_ble_configPtr->uartHostHw, CY_SCB_TX_INTR_OVERFLOW | CY_SCB_TX_INTR_UART_DONE);

    Cy_SCB_UART_Enable(cy_ble_configPtr->uartHostHw);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_Stop
****************************************************************************//**
*
*  Disables the UART, clears all pending interrupts and disables the UART TX
*  and RX interrupts. This will also empty out the FIFOs.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_Stop(void)
{
    /* Stop interrupt and UART */
    NVIC_DisableIRQ((IRQn_Type)cy_ble_configPtr->uartHostIsrConfig->intrSrc);

    Cy_SCB_UART_Disable(cy_ble_configPtr->uartHostHw, NULL);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_Transmit
****************************************************************************//**
*
*  Sends the data to UART TX FIFO. The function handles data length up to the
*  supported TX FIFO length of the UART hardware module.
*
*  \param data: Pointer to the data to send through the UART
*  \param length: the length of data to transmit in bytes
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_Transmit(const uint8_t *dataBuf,
                                   uint8_t length)
{
    Cy_SCB_UART_PutArrayBlocking(cy_ble_configPtr->uartHostHw, (uint8_t*)dataBuf, (uint32_t)length);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_IsrEnable
****************************************************************************//**
*
*   Enables the UART interrupt to the interrupt controller.
*   Do not call this function unless Cy_BLE_HAL_HOST_UART_Start() has been called or the
*   functionality of the Cy_SysInt_Init() function, which sets the vector and the
*   priority, has been called.
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_IsrEnable(void)
{
    NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->uartHostIsrConfig->intrSrc);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_IsrDisable
****************************************************************************//**
*
*   Disables the UART Interrupt in the interrupt controller.
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_IsrDisable(void)
{
    NVIC_DisableIRQ((IRQn_Type)cy_ble_configPtr->uartHostIsrConfig->intrSrc);
}
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */


#if(CY_BLE_STACK_MODE_CONTR_UART)

/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_Interrupt
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the UART.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_UART_Interrupt(void)
{
    uint8_t uartTxStatus = CY_BLE_INTR_TX_SUCCESS;
    uint32_t srcInterrupt;

    uint8_t length = 0u;
    uint8_t srcCount = 0u;
    uint8_t uartRxStatus = CY_BLE_INTR_RX_SUCCESS;
    uint8_t receivedData;

    /* Evaluate RX interrupt event */
    srcInterrupt = Cy_SCB_GetRxInterruptStatusMasked(cy_ble_configPtr->uartHw);

    if(0u != (srcInterrupt & CY_SCB_RX_INTR_NOT_EMPTY))
    {
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_PARITY_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_PARITY_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_FRAME_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_FRAME_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_OVERFLOW))
        {
            uartRxStatus |= CY_BLE_INTR_RX_OVERFLOW;
        }
        if(uartRxStatus == CY_BLE_INTR_RX_SUCCESS)
        {
            length = (uint8_t)Cy_SCB_GetNumInRxFifo(cy_ble_configPtr->uartHw);
            for(srcCount = 0u; srcCount < length; srcCount++)
            {
                receivedData = (uint8_t)Cy_SCB_ReadRxFifo(cy_ble_configPtr->uartHw);
                Cy_BLE_HAL_UartRxDataHandler(receivedData);
            }
        }
        else
        {
            Cy_SCB_ClearRxFifo(cy_ble_configPtr->uartHw);
        }
        Cy_SCB_ClearRxInterrupt(cy_ble_configPtr->uartHw, srcInterrupt);
    }
    else
    {
        /* No RX interrupt. Do nothing. */
    }

    /* Evaluate TX interrupt event in sequence */
    srcInterrupt = Cy_SCB_GetTxInterruptStatusMasked(cy_ble_configPtr->uartHw);

    /* BLE Stack manager TX UART complete */
    if(0u != (srcInterrupt & CY_SCB_TX_INTR_UART_DONE))
    {
        if(0u != (srcInterrupt & CY_SCB_TX_INTR_OVERFLOW))
        {
            /*Stack manager TX UART error */
            uartTxStatus |= CY_BLE_INTR_TX_OVERFLOW;
        }
        Cy_BLE_HAL_UartTxCompltHandler();
        Cy_SCB_ClearTxInterrupt(cy_ble_configPtr->uartHw, srcInterrupt);
    }
    else
    {
        /* No TX interrupt. Do nothing. */
    }
}

/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_Start
****************************************************************************//**
*
*  Enables the platform UART TX and RX interrupts and then enables the UART.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_UART_Start(void)
{
    /* Setup ISR */
    Cy_SysInt_Init(cy_ble_configPtr->uartIsrConfig, &Cy_BLE_HAL_UART_Interrupt);
    NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->uartIsrConfig->intrSrc);

    (void)Cy_SCB_UART_Init(cy_ble_configPtr->uartHw, cy_ble_configPtr->uartConfig, NULL);
    /* Configure interrupt sources. */
    Cy_SCB_SetRxInterruptMask(cy_ble_configPtr->uartHw,
                              CY_SCB_RX_INTR_NOT_EMPTY | CY_SCB_RX_INTR_OVERFLOW | CY_SCB_RX_INTR_UNDERFLOW);
    Cy_SCB_SetTxInterruptMask(cy_ble_configPtr->uartHw, CY_SCB_TX_INTR_OVERFLOW | CY_SCB_TX_INTR_UART_DONE);

    Cy_SCB_UART_Enable(cy_ble_configPtr->uartHw);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_Stop
****************************************************************************//**
*
*  Disables the UART, clears all pending interrupts and disables the UART TX
*  and RX interrupts. This will also empty out the FIFOs.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_UART_Stop(void)
{
    /* Stop interrupt and UART */
    NVIC_DisableIRQ((IRQn_Type)cy_ble_configPtr->uartIsrConfig->intrSrc);

    Cy_SCB_UART_Disable(cy_ble_configPtr->uartHw, NULL);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_Transmit
****************************************************************************//**
*
*  Sends the data to UART TX FIFO. The function handles data length up to the
*  supported TX FIFO length of the UART hardware module.
*
*  \param data: Pointer to the data to send through the UART
*  \param length: the length of data to transmit in bytes
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_UART_Transmit(uint8_t *dataBuf,
                              uint8_t length)
{
    Cy_SCB_UART_PutArrayBlocking(cy_ble_configPtr->uartHw, dataBuf, (uint32_t)length);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_IsrEnable
****************************************************************************//**
*
*   Enables the UART interrupt to the interrupt controller.
*   Do not call this function unless Cy_BLE_HAL_UART_Start() has been called or the
*   functionality of the Cy_SysInt_Init() function, which sets the vector and the
*   priority, has been called.
*
*******************************************************************************/
void Cy_BLE_HAL_UART_IsrEnable(void)
{
    NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->uartIsrConfig->intrSrc);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_UART_IsrDisable
****************************************************************************//**
*
*   Disables the UART Interrupt in the interrupt controller.
*
*******************************************************************************/
void Cy_BLE_HAL_UART_IsrDisable(void)
{
    NVIC_DisableIRQ((IRQn_Type)cy_ble_configPtr->uartIsrConfig->intrSrc);
}

#endif /* (CY_BLE_CONFIG_MODE_CONTR_UART) */

#if(CY_BLE_STACK_CONTR_CORE)
/*******************************************************************************
* Function Name: Cy_BLE_HAL_RadioCalibrationWrite
****************************************************************************//**
*
*  Write radio calibration data to SFLASH
*
*  \param data:   Pointer to the buffer containing the data to be stored.
*  \param length: The length of the data in bytes.
*
*  \return
*   CY_BLE_SUCCESS                     A successful write
*   CY_BLE_ERROR_INVALID_PARAMETER     At least one of the input parameters is invalid
*   CY_BLE_ERROR_FLASH_WRITE           Error in flash Write
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_HAL_RadioCalibrationWrite(const void *data, uint32_t length)
{
    cy_en_ble_api_result_t rc;
    
    if((data != NULL) && 
       (length <= (CY_FLASH_SIZEOF_ROW - sizeof(cy_stc_ble_gap_bd_addr_t))))
    {
        rc = Cy_BLE_HAL_NvramWrite(data, (uint8_t *)CY_BLE_SFLASH_BLE_RADIO_CALL_ADDRESS, length);
    }
    else
    {
        rc = CY_BLE_ERROR_INVALID_PARAMETER;
    }
    return(rc);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_RadioCalibrationRead
****************************************************************************//**
*
*  Read radio calibration data from SFLASH
*
*  \param data:   Pointer to the buffer containing the data to be read from SFASH.
*  \param length: The length of the data in bytes.
*
*  \return
*   CY_BLE_SUCCESS                     A successful read
*   CY_BLE_ERROR_INVALID_PARAMETER     At least one of the input parameters is invalid
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_HAL_RadioCalibrationRead(void *data, uint32_t length)
{
    cy_en_ble_api_result_t rc = CY_BLE_SUCCESS;
    
    if((data != NULL) && 
       (length <= (CY_FLASH_SIZEOF_ROW - sizeof(cy_stc_ble_gap_bd_addr_t))))
    {
        (void) memcpy(data, (void *)CY_BLE_SFLASH_BLE_RADIO_CALL_ADDRESS, length);
    }
    else
    {
        rc = CY_BLE_ERROR_INVALID_PARAMETER;
    }
    return(rc);
}

#endif /* CY_BLE_STACK_CONTR_CORE */


/*******************************************************************************
* Function Name: Cy_BLE_HAL_NvramWrite
****************************************************************************//**
*
*  This function writes the data to the NVRAM store. It will check the
*  appropriate alignment of a start address and also perform an address range
*  check based on the length before performing the write operation.
*  This function performs memory compare and writes only row where there are new
*  data to write.
*
*  \param buffer:   Pointer to the buffer containing the data to be stored.
*  \param varFlash: Pointer to the array or variable in the flash.
*  \param length:    The length of the data in bytes.
*
* \return
*  CY_BLE_SUCCESS                     A successful write
*  CY_BLE_ERROR_INVALID_PARAMETER     At least one of the input parameters is invalid
*  CY_BLE_ERROR_FLASH_WRITE           Error in flash Write
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_HAL_NvramWrite(const uint8_t buffer[],
                                             const uint8_t varFlash[],
                                             uint32_t length)
{
    
    cy_en_ble_api_result_t bleReturn;
    cy_en_flashdrv_status_t rc = CY_FLASH_DRV_SUCCESS;

    uint32_t writeBuffer[CY_FLASH_SIZEOF_ROW / sizeof(uint32_t)];
    uint32_t rowId;
    uint32_t dstIndex;
    uint32_t srcIndex = 0u;
    uint32_t eeOffset;
    uint32_t byteOffset;
    uint32_t rowsNotEqual;
    uint8_t *writeBufferPointer;  

    eeOffset = (uint32_t)varFlash;
    writeBufferPointer = (uint8_t*)writeBuffer;

    /* Make sure, that varFlash[] points to Flash or Emulated EEPROM */
    if(((eeOffset >= CY_FLASH_BASE) &&
        ((eeOffset + length) <= (CY_FLASH_BASE + CY_FLASH_SIZE))) ||
       ((eeOffset >= CY_EM_EEPROM_BASE) &&
        ((eeOffset + length) <= (CY_EM_EEPROM_BASE + CY_EM_EEPROM_SIZE))) ||
       ((eeOffset >= (uint32_t)SFLASH->BLE_DEVICE_ADDRESS) &&
        ((eeOffset + length) <= ((uint32_t)SFLASH->BLE_DEVICE_ADDRESS + CY_FLASH_SIZEOF_ROW))))
    {
        cy_en_syspm_simo_buck_voltage1_t simoVoltage = CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_1_1V;
        cy_en_syspm_ldo_voltage_t ldoVoltage = CY_SYSPM_LDO_VOLTAGE_1_1V;
        bool writeFlag = false;
        
        if(Cy_SysPm_SimoBuckIsEnabled())
        {
            simoVoltage = Cy_SysPm_SimoBuckGetVoltage1();
            if(simoVoltage == CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V)
            {   /* Increase core voltage for write to flash operation */
                Cy_SysPm_SimoBuckSetVoltage1(CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_1_1V);
            }
        }
        if(Cy_SysPm_LdoIsEnabled())
        {
            ldoVoltage = Cy_SysPm_LdoGetVoltage();
            if(ldoVoltage == CY_SYSPM_LDO_VOLTAGE_0_9V)
            {   /* Increase core voltage for write to flash operation */
                 Cy_SysPm_LdoSetVoltage(CY_SYSPM_LDO_VOLTAGE_1_1V);
            }
        }
        
        eeOffset -= CY_FLASH_BASE;
        rowId = eeOffset / CY_FLASH_SIZEOF_ROW;
        byteOffset = CY_FLASH_SIZEOF_ROW * rowId;

        /* Inform Controller core about write operation */
        while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Set(CY_BLE_SEMA, false))
        {
        }
        
        while((rc == CY_FLASH_DRV_SUCCESS) && (srcIndex < length))
        {
            rowsNotEqual = 0u;
            /* Copy data to the write buffer either from the source buffer or from the flash */
            for(dstIndex = 0u; dstIndex < CY_FLASH_SIZEOF_ROW; dstIndex++)
            {
                if((byteOffset >= eeOffset) && (srcIndex < length))
                {
                    writeBufferPointer[dstIndex] = buffer[srcIndex];
                    /* Detect that row programming is required */
                    if((rowsNotEqual == 0u) && (CY_GET_REG8(CY_FLASH_BASE + byteOffset) != buffer[srcIndex]))
                    {
                        rowsNotEqual = 1u;
                    }
                    srcIndex++;
                }
                else
                {
                    writeBufferPointer[dstIndex] = CY_GET_REG8(CY_FLASH_BASE + byteOffset);
                }
                byteOffset++;
            }

            if(rowsNotEqual != 0u)
            {
                uint32_t oprTimeout = CY_BLE_FLASH_OPR_TIMEOUT; /* us */
                
            #ifdef CY_BLE_FLASH_OPR_START_WRITE
                /* Program flash row */
                rc = Cy_Flash_StartWrite((rowId * CY_FLASH_SIZEOF_ROW) + CY_FLASH_BASE, writeBuffer); 
                if(rc == CY_FLASH_DRV_OPERATION_STARTED)
                {
                    /* Polls whether the Flash operation is performed */
                    do
                    {
                        rc = Cy_Flash_IsOperationComplete();
                    }
                    while (rc == CY_FLASH_DRV_OPCODE_BUSY);
                }
            #else
                /* Erase flash row */
                do 
                {
                    rc = Cy_Flash_StartErase((rowId * CY_FLASH_SIZEOF_ROW) + CY_FLASH_BASE); 
                    
                    /* Timeout if flash operation return error */
                    oprTimeout--;
                    Cy_SysLib_DelayUs(1u);
                }while(((rc & CY_FLASH_ID_ERROR) == CY_FLASH_ID_ERROR) && (oprTimeout != 0u));
                
                if(rc == CY_FLASH_DRV_OPERATION_STARTED)
                {
                    /* Polls whether the Flash operation is performed */
                    do
                    {
                        rc = Cy_Flash_IsEraseComplete();
                    }
                    while (rc == CY_FLASH_DRV_OPCODE_BUSY);
                }
                
                /* Program flash row */
                if(rc == CY_FLASH_DRV_SUCCESS)
                {
                    oprTimeout = CY_BLE_FLASH_OPR_TIMEOUT; /* us */
                    
                    /* Program flash row */
                    do
                    {
                        rc = Cy_Flash_StartProgram((rowId * CY_FLASH_SIZEOF_ROW) + CY_FLASH_BASE, writeBuffer); 
                        
                        /* Timeout if flash operation return error */
                        oprTimeout--;
                        Cy_SysLib_DelayUs(1u);
                    }while(((rc & CY_FLASH_ID_ERROR) == CY_FLASH_ID_ERROR) && (oprTimeout != 0u));
                
                    if(rc == CY_FLASH_DRV_OPERATION_STARTED)
                    {
                        /* Polls whether the Flash operation is performed */
                        do
                        {
                            rc = Cy_Flash_IsOperationComplete();
                        }
                        while (rc == CY_FLASH_DRV_OPCODE_BUSY);
                    }
                } 
            #endif /* FLASH_OPERATION_START_WRITE */
                writeFlag = true;
            }

            /* Go to the next row */
            rowId++;
        }
        
        /* Clear Flash Cache and Buffer after write operation */
        if(writeFlag == true)
        {
            Cy_SysLib_ClearFlashCacheAndBuffer();
        }
        
        /* Inform BLE syspm callback about complete of write operation */
        while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Clear(CY_BLE_SEMA, false))
        {
        } 
              
        if((Cy_SysPm_SimoBuckIsEnabled()) && (simoVoltage == CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V))
        {   /* Return core voltage */
            Cy_SysPm_SimoBuckSetVoltage1(CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V);
        }
        if((Cy_SysPm_LdoIsEnabled()) && (ldoVoltage == CY_SYSPM_LDO_VOLTAGE_0_9V))
        {   /* Return core voltage */
            Cy_SysPm_LdoSetVoltage(CY_SYSPM_LDO_VOLTAGE_0_9V);
        }
    }
    else
    {
        rc = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
    }

    /* Return BLE error code */
    switch(rc)
    {
        case CY_FLASH_DRV_SUCCESS:
            bleReturn = CY_BLE_SUCCESS;
            break;

        case CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
        case CY_FLASH_DRV_INVALID_FLASH_ADDR:
            bleReturn = CY_BLE_ERROR_INVALID_PARAMETER;
            break;

        default:
            bleReturn = CY_BLE_ERROR_FLASH_WRITE;
            break;
    }

    return(bleReturn);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_NvramNonBlockingRowWrite
****************************************************************************//**
*
*  This function writes one row to the NVRAM store. It will check the
*  appropriate alignment of a start address and also perform an address range
*  check based on the length before performing the write operation.
*  This function performs memory compare and writes only row where there are new
*  data to write.
*
*  If function returns CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS
*  it means that writing is no complete yet and require to invoke again to get finish. 
*
*  This function is designated for internal usage.
*
* \param buffer:   Pointer to the buffer containing the data to be stored.
* \param varFlash: Pointer to the array or variable in the flash.
* \param length:   The length of the data in bytes.
*
* \return
*  CY_BLE_SUCCESS                       A successful write
*  CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS  Row writing in progress
*  CY_BLE_ERROR_INVALID_PARAMETER       At least one of the input parameters is invalid
*  CY_BLE_ERROR_FLASH_WRITE             Error in flash Write
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_HAL_NvramNonBlockingRowWrite(const uint8_t buffer[],
                                                           const uint8_t varFlash[],
                                                           uint32_t length)
{
    static cy_en_ble_api_result_t nvramState = CY_BLE_SUCCESS;
    static cy_en_syspm_simo_buck_voltage1_t simoVoltage = CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_1_1V;
    static cy_en_syspm_ldo_voltage_t ldoVoltage = CY_SYSPM_LDO_VOLTAGE_1_1V;
    static uint32_t writeBuffer[CY_FLASH_SIZEOF_ROW / sizeof(uint32_t)] = {0x00};
    
    cy_en_flashdrv_status_t rc = CY_FLASH_DRV_SUCCESS;
       
    uint32_t rowId;
    uint32_t dstIndex;
    uint32_t srcIndex = 0u;
    uint32_t eeOffset = (uint32_t)varFlash;
    uint32_t byteOffset;
    uint8_t *writeBufferPointer;  

    bool rowNotEqual = false;
    bool restoreCoreVoltage = false;
     
    writeBufferPointer = (uint8_t*)writeBuffer;

    /* Make sure, that varFlash[] points to Flash or Emulated EEPROM */
    if((((eeOffset >= CY_FLASH_BASE) && ((eeOffset + length) <= (CY_FLASH_BASE + CY_FLASH_SIZE))) ||
       ((eeOffset >= CY_EM_EEPROM_BASE) && ((eeOffset + length) <= (CY_EM_EEPROM_BASE + CY_EM_EEPROM_SIZE)))) &&
        (length <= CY_FLASH_SIZEOF_ROW)
    )
    {
        eeOffset -= CY_FLASH_BASE;
        rowId = eeOffset / CY_FLASH_SIZEOF_ROW;
        byteOffset = CY_FLASH_SIZEOF_ROW * rowId;
        
        switch (nvramState)
        {
            /* Start write row */
            case CY_BLE_SUCCESS:
            case CY_BLE_ERROR_FLASH_WRITE:
            case CY_BLE_ERROR_INVALID_PARAMETER:
            
                /* Copy data to the write buffer either from the source buffer or from the flash 
                   and check if row programming is required */
                for(dstIndex = 0u; dstIndex < CY_FLASH_SIZEOF_ROW; dstIndex++)
                {
                    if((byteOffset >= eeOffset) && (srcIndex < length))
                    {
                        writeBufferPointer[dstIndex] = buffer[srcIndex];
                        
                        /* Detect that row programming is required */
                        if((rowNotEqual == false) && (CY_GET_REG8(CY_FLASH_BASE + byteOffset) != buffer[srcIndex]))
                        {
                            rowNotEqual = true;
                        }
                        srcIndex++;
                    }
                    else
                    {
                        writeBufferPointer[dstIndex] = CY_GET_REG8(CY_FLASH_BASE + byteOffset);
                    }
                    byteOffset++;
                }

                /* Start a new write */
                if(rowNotEqual != false)
                {                     
                    /* Inform BLE syspm callback about write operation */
                    while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Set(CY_BLE_SEMA, false))
                    {
                    } 
                    
                    /* Increase core voltage for write to flash operation */
                    if(Cy_SysPm_SimoBuckIsEnabled())
                    {
                        simoVoltage = Cy_SysPm_SimoBuckGetVoltage1();
                        if(simoVoltage == CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V)
                        {   /* Increase core voltage for write to flash operation */
                            Cy_SysPm_SimoBuckSetVoltage1(CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_1_1V);
                        }
                    }  
                    if(Cy_SysPm_LdoIsEnabled())
                    {
                        ldoVoltage = Cy_SysPm_LdoGetVoltage();
                        if(ldoVoltage == CY_SYSPM_LDO_VOLTAGE_0_9V)
                        {   /* Increase core voltage for write to flash operation */
                            Cy_SysPm_LdoSetVoltage(CY_SYSPM_LDO_VOLTAGE_1_1V);
                        }
                    }
                                  
                    /* Write flash row */
                    rc = Cy_Flash_StartWrite((rowId * CY_FLASH_SIZEOF_ROW) + CY_FLASH_BASE, writeBuffer); 

                    if (rc != CY_FLASH_DRV_OPERATION_STARTED)
                    {      
                        /* We have error during start write, returns CY_BLE_ERROR_FLASH_WRITE and stop writing */
                        nvramState = CY_BLE_ERROR_FLASH_WRITE;
                        
                        /* Set flag for restore core voltage */
                        restoreCoreVoltage = true;
                    }
                    else
                    {
                        nvramState = CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS;
                    }
                }
                else
                {
                    nvramState = CY_BLE_SUCCESS;
                }
                break;
                 
            case CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS:
                
                /* Check the status of the Flash operation */
                rc = Cy_Flash_IsOperationComplete();
                
                if(rc == CY_FLASH_DRV_SUCCESS)
                {
                    nvramState = CY_BLE_SUCCESS;
                    restoreCoreVoltage = true;
                }
                else if ((rc & CY_FLASH_ID_ERROR) == CY_FLASH_ID_ERROR)
                {
                    nvramState = CY_BLE_ERROR_FLASH_WRITE;
                    restoreCoreVoltage = true;
                    
                }
                else if((rc & CY_FLASH_ID_INFO) == CY_FLASH_ID_INFO)
                {
                    nvramState = CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS;
                }
                else 
                {
                    /* empty else */
                }
                break;

            default:
                break;
        }
        
        /* Restore core voltage */
        if(restoreCoreVoltage == true)
        {  
            if((Cy_SysPm_SimoBuckIsEnabled()) && (simoVoltage == CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V))
            {   /* Return core voltage */   
                Cy_SysPm_SimoBuckSetVoltage1(CY_SYSPM_SIMO_BUCK_OUT1_VOLTAGE_0_9V);
            }
            if((Cy_SysPm_LdoIsEnabled()) && (ldoVoltage == CY_SYSPM_LDO_VOLTAGE_0_9V))
            {   /* Return core voltage */
                Cy_SysPm_LdoSetVoltage(CY_SYSPM_LDO_VOLTAGE_0_9V);
            }  
                
            /* Inform BLE syspm callback about complete of write operation */
            while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Clear(CY_BLE_SEMA, false))
            {
            } 
            /* Clear Flash Cache and Buffer after write operation */
            Cy_SysLib_ClearFlashCacheAndBuffer();
  
        }
    }
    else
    {
        nvramState = CY_BLE_ERROR_INVALID_PARAMETER;
    }

    return(nvramState);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_StackNvramWrite
****************************************************************************//**
*
*  This function is used by stack to perform flash write
*
*  This function is designated for internal usage.
*
* \param buffer:   Pointer to the buffer containing the data to be stored.
* \param varFlash: Pointer to the array or variable in the flash.
* \param length:   The length of the data in bytes.
*
* \return
*  CY_BLE_SUCCESS                       A successful write
*  CY_BLE_INFO_FLASH_WRITE_IN_PROGRESS  Row writing in progress
*  CY_BLE_ERROR_INVALID_PARAMETER       At least one of the input parameters is invalid
*  CY_BLE_ERROR_FLASH_WRITE             Error in flash Write
*
*******************************************************************************/
cy_en_ble_api_result_t Cy_BLE_HAL_StackNvramWrite(const uint8_t buffer[],
                                                  const uint8_t varFlash[],
                                                  uint32_t length)
{
    return(Cy_BLE_HAL_NvramWrite(buffer, varFlash, length));
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_NvramWriteIsBusy
****************************************************************************//**
*
* Reports the flash operation in the BLE host core is active
*
* \return
*   true  - flash operation is active
*   false - no flash activity
*
*******************************************************************************/
bool Cy_BLE_HAL_NvramWriteIsBusy(void)
{
    uint32_t ipcChanNum;
    bool retVal = true;
    
    /* Select CY_IPC_CHAN_SYSCAL regarding to host core */
    #if (CY_BLE_HOST_CORE == CY_CPU_CORTEX_M4)
        ipcChanNum = CY_IPC_CHAN_SYSCALL_CM4;
    #else
        ipcChanNum = CY_IPC_CHAN_SYSCALL_CM0;
    #endif  /* (CY_CPU_CORTEX_M0P) */
    
    /* Checks if the IPC structure is not locked */
    if (Cy_IPC_Drv_IsLockAcquired((IPC_STRUCT_Type*) &IPC->STRUCT[ipcChanNum]) == false)
    {
       retVal = false;
    }

    return (retVal);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_CalcCRC8
****************************************************************************//**
*
* Implements CRC-8 calculation
*
* \param data 
* The data to be used to calculate the CRC.
*
* \param length 
* The length of data.
*
* \return 
* The calculated CRC value.
*
*******************************************************************************/
uint8_t Cy_BLE_HAL_CalcCRC8(uint8_t data[], uint32_t length)
{
    return (Cy_BLE_HAL_BlockCalcCRC8(data, length, 0u));
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_BlockCalcCRC8
****************************************************************************//**
*
* Implements CRC-8 calculation
*
* \param data 
* The data to be used to calculate the CRC.
*
* \param length 
* The length of data.
*
* \param crcSeed 
* Init value of CRC. Use in case if we need calculate large buffer size by block
*
* \return 
* The calculated CRC value.
*
*******************************************************************************/
uint8_t Cy_BLE_HAL_BlockCalcCRC8(uint8_t data[], uint32_t length, uint8_t crcSeed)
{
    const uint8_t Crc8Table[256] = {
    0x00u, 0x31u, 0x62u, 0x53u, 0xC4u, 0xF5u, 0xA6u, 0x97u,
    0xB9u, 0x88u, 0xDBu, 0xEAu, 0x7Du, 0x4Cu, 0x1Fu, 0x2Eu,
    0x43u, 0x72u, 0x21u, 0x10u, 0x87u, 0xB6u, 0xE5u, 0xD4u,
    0xFAu, 0xCBu, 0x98u, 0xA9u, 0x3Eu, 0x0Fu, 0x5Cu, 0x6Du,
    0x86u, 0xB7u, 0xE4u, 0xD5u, 0x42u, 0x73u, 0x20u, 0x11u,
    0x3Fu, 0x0Eu, 0x5Du, 0x6Cu, 0xFBu, 0xCAu, 0x99u, 0xA8u,
    0xC5u, 0xF4u, 0xA7u, 0x96u, 0x01u, 0x30u, 0x63u, 0x52u,
    0x7Cu, 0x4Du, 0x1Eu, 0x2Fu, 0xB8u, 0x89u, 0xDAu, 0xEBu,
    0x3Du, 0x0Cu, 0x5Fu, 0x6Eu, 0xF9u, 0xC8u, 0x9Bu, 0xAAu,
    0x84u, 0xB5u, 0xE6u, 0xD7u, 0x40u, 0x71u, 0x22u, 0x13u,
    0x7Eu, 0x4Fu, 0x1Cu, 0x2Du, 0xBAu, 0x8Bu, 0xD8u, 0xE9u,
    0xC7u, 0xF6u, 0xA5u, 0x94u, 0x03u, 0x32u, 0x61u, 0x50u,
    0xBBu, 0x8Au, 0xD9u, 0xE8u, 0x7Fu, 0x4Eu, 0x1Du, 0x2Cu,
    0x02u, 0x33u, 0x60u, 0x51u, 0xC6u, 0xF7u, 0xA4u, 0x95u,
    0xF8u, 0xC9u, 0x9Au, 0xABu, 0x3Cu, 0x0Du, 0x5Eu, 0x6Fu,
    0x41u, 0x70u, 0x23u, 0x12u, 0x85u, 0xB4u, 0xE7u, 0xD6u,
    0x7Au, 0x4Bu, 0x18u, 0x29u, 0xBEu, 0x8Fu, 0xDCu, 0xEDu,
    0xC3u, 0xF2u, 0xA1u, 0x90u, 0x07u, 0x36u, 0x65u, 0x54u,
    0x39u, 0x08u, 0x5Bu, 0x6Au, 0xFDu, 0xCCu, 0x9Fu, 0xAEu,
    0x80u, 0xB1u, 0xE2u, 0xD3u, 0x44u, 0x75u, 0x26u, 0x17u,
    0xFCu, 0xCDu, 0x9Eu, 0xAFu, 0x38u, 0x09u, 0x5Au, 0x6Bu,
    0x45u, 0x74u, 0x27u, 0x16u, 0x81u, 0xB0u, 0xE3u, 0xD2u,
    0xBFu, 0x8Eu, 0xDDu, 0xECu, 0x7Bu, 0x4Au, 0x19u, 0x28u,
    0x06u, 0x37u, 0x64u, 0x55u, 0xC2u, 0xF3u, 0xA0u, 0x91u,
    0x47u, 0x76u, 0x25u, 0x14u, 0x83u, 0xB2u, 0xE1u, 0xD0u,
    0xFEu, 0xCFu, 0x9Cu, 0xADu, 0x3Au, 0x0Bu, 0x58u, 0x69u,
    0x04u, 0x35u, 0x66u, 0x57u, 0xC0u, 0xF1u, 0xA2u, 0x93u,
    0xBDu, 0x8Cu, 0xDFu, 0xEEu, 0x79u, 0x48u, 0x1Bu, 0x2Au,
    0xC1u, 0xF0u, 0xA3u, 0x92u, 0x05u, 0x34u, 0x67u, 0x56u,
    0x78u, 0x49u, 0x1Au, 0x2Bu, 0xBCu, 0x8Du, 0xDEu, 0xEFu,
    0x82u, 0xB3u, 0xE0u, 0xD1u, 0x46u, 0x77u, 0x24u, 0x15u,
    0x3Bu, 0x0Au, 0x59u, 0x68u, 0xFFu, 0xCEu, 0x9Du, 0xACu };
        
    uint8_t crc = crcSeed;
    
    while (length > 0u)
    {
        crc = Crc8Table[crc ^ *data];
        data++;
        length--;

    }

    return(crc);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_GetIcPackageType
****************************************************************************//**
*
*  This function returns package type
*
* return:
*  0 - BGA package
*  1 - CSP package
*  
*******************************************************************************/
uint32_t Cy_BLE_HAL_GetIcPackageType(void)
{
    return((CY_GPIO_PACKAGE_TYPE == CY_GPIO_PACKAGE_CSP) ? 1u : 0u);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_IsDefinedTx5dbm
****************************************************************************//**
*
*  This function returns true if Tx 5dbm mode is defined
*
* return:
*  true  - Tx 5dbm mode is defined
*  false - Tx 5dbm mode is not defined
*  
*******************************************************************************/
bool Cy_BLE_HAL_IsDefinedTx5dbm(void)
{
    bool ret = false;
    
    #if (defined (CY_BLE_ENABLE_TX_5DBM) && (CY_BLE_ENABLE_TX_5DBM != 0u))
        ret = true;
    #endif /* ((defined (CY_BLE_ENABLE_TX_5DBM)) && (CY_BLE_ENABLE_TX_5DBM != 0u)) */
    
    return(ret);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_DelayUs
****************************************************************************//**
*
*  Interface to Cy_SysLib_DelayUs function.
*
*******************************************************************************/
void Cy_BLE_HAL_DelayUs(uint16_t delayVal)
{
    Cy_SysLib_DelayUs(delayVal);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_DelayMs
****************************************************************************//**
*
*  Interface to Cy_SysLib_Delay function.
*
*******************************************************************************/
void Cy_BLE_HAL_DelayMs(uint32_t delayVal)
{
    Cy_SysLib_Delay(delayVal);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_EnableGlobalInterrupts
****************************************************************************//**
*
*  This function enables all interrupt used by the BLE Middleware.
*
*******************************************************************************/
void Cy_BLE_HAL_EnableGlobalInterrupts(void)
{
#if (CY_BLE_STACK_MODE_HOST_UART)
    Cy_BLE_HAL_HOST_UART_IsrEnable();
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */

#if (CY_BLE_STACK_MODE_CONTR_UART)
    Cy_BLE_HAL_UART_IsrEnable();
#endif /* (CY_BLE_STACK_MODE_CONTR_UART) */

/* Enable IPC interrupt */
#if (CY_BLE_STACK_MODE_IPC)
    Cy_BLE_Ipc_Cypipe_Isr_Enable();
#endif  /* (CY_BLE_STACK_MODE_IPC) */

#if (CY_BLE_STACK_CONTR_CORE)
    NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->blessIsrConfig->intrSrc);
#endif  /* (CY_BLE_STACK_CONTR_CORE) */
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_DisableGlobalInterrupts
****************************************************************************//**
*
*  This disables all interrupt used by the BLE Middleware.
*
*******************************************************************************/
void Cy_BLE_HAL_DisableGlobalInterrupts(void)
{
#if (CY_BLE_STACK_MODE_HOST_UART)
    Cy_BLE_HAL_HOST_UART_IsrDisable();
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */

#if (CY_BLE_STACK_MODE_CONTR_UART)
    Cy_BLE_HAL_UART_IsrDisable();
#endif /* (CY_BLE_STACK_MODE_CONTR_UART) */

/* Disable IPC interrupt */
#if (CY_BLE_STACK_MODE_IPC)
    Cy_BLE_Ipc_Cypipe_Isr_Disable();
#endif /* (CY_BLE_STACK_MODE_IPC) */

#if (CY_BLE_STACK_CONTR_CORE)
    NVIC_DisableIRQ((IRQn_Type)cy_ble_configPtr->blessIsrConfig->intrSrc);
#endif /* (CY_BLE_STACK_CONTR_CORE) */
}


/** 
 * BLE Stack Interface to clock 
 */

/*******************************************************************************
* Function Name: Cy_BLE_HAL_LfClkGetSource
****************************************************************************//**
*
*  Interface to Cy_SysClk_LfClkGetSource function.
*
*******************************************************************************/
cy_en_clklf_in_sources_t Cy_BLE_HAL_LfClkGetSource(void)
{
    return(Cy_SysClk_ClkLfGetSource());
}

/*******************************************************************************
* Function Name: Cy_BLE_HAL_ClkMeasurementCountersDone
****************************************************************************//**
*
*  Interface to Cy_SysClk_ClkMeasurementCountersDone function.
*
*******************************************************************************/
int32_t Cy_BLE_HAL_ClkMeasurementCountersDone(void)
{
    return((int32_t)Cy_SysClk_ClkMeasurementCountersDone());
}

/*******************************************************************************
* Function Name: Cy_BLE_HAL_ClkMeasurementCountersGetFreq
****************************************************************************//**
*
*  Interface to Cy_SysClk_ClkMeasurementCountersGetFreq function.
*
*******************************************************************************/
uint32_t Cy_BLE_HAL_ClkMeasurementCountersGetFreq(bool measuredClock,
                                                  uint32_t refClkFreq)
{
    return(Cy_SysClk_ClkMeasurementCountersGetFreq(measuredClock, refClkFreq));
}

/*******************************************************************************
* Function Name: Cy_BLE_HAL_StartClkMeasurementCounters
****************************************************************************//**
*
*  Interface to Cy_SysClk_StartClkMeasurementCounters function.
*
*******************************************************************************/
uint32_t Cy_BLE_HAL_StartClkMeasurementCounters(cy_en_meas_clks_t clock1,
                                                uint32_t count1,
                                                cy_en_meas_clks_t clock2)
{
    return((uint32_t)Cy_SysClk_StartClkMeasurementCounters(clock1, count1, clock2));
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_PiloTrim
****************************************************************************//**
*
*  Interface to Cy_SysClk_PiloTrim function.
*
*******************************************************************************/
int32_t Cy_BLE_HAL_PiloTrim(uint32_t piloFreq)
{
    return(Cy_SysClk_PiloTrim(piloFreq));
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_BlessStart
****************************************************************************//**
*
*  Starts Interrupt Controller.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_BlessStart(void)
{
    Cy_BLE_HAL_Init();

    /* Configures external power amplifier outputs. */
    Cy_BLE_ConfigureExtPA(BLE_BLESS_EXT_PA_LNA_CTRL_ENABLE_EXT_PA_LNA_Msk |
                          (CY_BLE_CONFIG_EXT_PA_ENABLED ? BLE_BLESS_EXT_PA_LNA_CTRL_OUT_EN_DRIVE_VAL_Msk : 0u));

    /* Setup BLESS ISR */
    #if (CY_BLE_STACK_CONTR_CORE)
        (void)Cy_SysInt_Init(cy_ble_configPtr->blessIsrConfig, &Cy_BLE_BlessInterrupt);
        NVIC_EnableIRQ((IRQn_Type)cy_ble_configPtr->blessIsrConfig->intrSrc);
    #endif /* (CY_BLE_STACK_CONTR_CORE) */
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_GetIpBlockVersion
****************************************************************************//**
*
* This function returns the version of m0s8bless ip block.
*
* \return
* uint32_t bits:
* 7:0 - ip version ( 1 - BLE_ver1, 2 - BLE_ver2, 3 - BLE_ver3, 4 - BLE_ver3 (*A) )
* 31:8 - reserved for future usage
*
*******************************************************************************/
uint32_t Cy_BLE_HAL_GetIpBlockVersion(void)
{
    return(CY_BLE_M0S8BLESS_VERSION);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_EnterCriticalSection
****************************************************************************//**
*
*  Interface to Cy_SysLib_EnterCriticalSection function.
*
*******************************************************************************/
uint32_t Cy_BLE_HAL_EnterCriticalSection(void)
{
    return(Cy_SysLib_EnterCriticalSection());
}

/*******************************************************************************
* Function Name: Cy_BLE_HAL_ExitCriticalSection
****************************************************************************//**
*
*  Interface to Cy_SysLib_ExitCriticalSection function.
*
*******************************************************************************/
void Cy_BLE_HAL_ExitCriticalSection(uint32_t interruptState)
{
    Cy_SysLib_ExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_SysPmSleep
****************************************************************************//**
*
*  Interface to Cy_SysPm_Sleep function.
*
*******************************************************************************/
cy_en_syspm_status_t Cy_BLE_HAL_SysPmSleep(cy_en_syspm_waitfor_t enWaitFor)
{
    return(Cy_SysPm_Sleep(enWaitFor));
}


/*******************************************************************************
* Function Name: Cy_BLE_HAL_SimoBuckSetVoltage2
****************************************************************************//**
*
*  Interface to Cy_SysPm_SimoBuckSetVoltage2 function with disable the 200 uS
*  delay after setting a higher voltage.
*
*******************************************************************************/
void Cy_BLE_HAL_SimoBuckSetVoltage2(cy_en_syspm_simo_buck_voltage2_t voltage)
{
    Cy_SysPm_SimoBuckSetVoltage2(voltage, false);
}


#if (CY_BLE_STACK_MODE_IPC)
/**
 *   
 * BLE Stack Interface to IPC 
 *
 */

/*******************************************************************************
* Function Name: Cy_BLE_Ipc_Cypipe_Isr_Enable
****************************************************************************//**
*
*   Enables the IPC pipe interrupt to the interrupt controller.
*
*******************************************************************************/
void Cy_BLE_Ipc_Cypipe_Isr_Enable(void)
{
#if (CY_BLE_STACK_CONTR_CORE)
    (void)Cy_IPC_Pipe_EndpointResume(CY_BLE_IPC_CONTROLLER_ADDR);
#endif  /* (CY_BLE_STACK_CONTR_CORE) */
#if (CY_BLE_HOST_CORE)
    (void)Cy_IPC_Pipe_EndpointResume(CY_BLE_IPC_HOST_ADDR);
#endif /* (CY_BLE_HOST_CORE) */
}


/*******************************************************************************
* Function Name: Cy_BLE_Ipc_Cypipe_Isr_Disable
****************************************************************************//**
*
*   Disables the IPC pipe Interrupt in the interrupt controller.
*
*******************************************************************************/
void Cy_BLE_Ipc_Cypipe_Isr_Disable(void)
{
#if (CY_BLE_STACK_CONTR_CORE)
    (void)Cy_IPC_Pipe_EndpointPause(CY_BLE_IPC_CONTROLLER_ADDR);
#endif  /* (CY_BLE_STACK_CONTR_CORE) */
#if (CY_BLE_HOST_CORE)
    (void)Cy_IPC_Pipe_EndpointPause(CY_BLE_IPC_HOST_ADDR);
#endif /* (CY_BLE_HOST_CORE) */
}

#if (CY_BLE_STACK_CONTR_CORE)
static cy_ipc_pipe_callback_ptr_t Cy_BLE_IPC_CtrlMsgFlushRecvStackHandle;
static void Cy_BLE_IPC_CtrlMsgFlushRecvCallBack(uint32_t * msgPtr);

/*******************************************************************************
* Function Name: Cy_BLE_IPC_CtrlMsgFlushRecvCallBack
****************************************************************************//**
*
* This a callback to be called when a message is received on a pipe.
*
* \param msgPtr
* Pointer to the callback message.
*
*******************************************************************************/    
static void Cy_BLE_IPC_CtrlMsgFlushRecvCallBack(uint32_t * msgPtr)
{
    /* Call BLE Stack IPC handler */
    Cy_BLE_IPC_CtrlMsgFlushRecvStackHandle(msgPtr);
}

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED)
/*******************************************************************************
* Function Name: Cy_BLE_IPC_ContrIntrFeatureCallBack
****************************************************************************//**
*
* This a callback to be called when a message is received on a pipe.
*
* \param msgPtr
* Pointer to the callback message.
*
*******************************************************************************/
static void Cy_BLE_IPC_ContrIntrFeatureCallBack(uint32_t * msgPtr);
static void Cy_BLE_IPC_ContrIntrFeatureCallBack(uint32_t * msgPtr)
{
    if((msgPtr != NULL) && (((cy_stc_ble_ipc_msg_t *)msgPtr)->pktType == 0xffu))
    {
        /* Set up Interrupt Notification Feature mask  */
        intrNotifyPtr = (cy_stc_ble_intr_notify_t *)msgPtr;
        intrNotifyPtr->configFlag = true;
    }   
}
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) */    
    
/*******************************************************************************
* Function Name: Cy_BLE_IPC_ControllerRegisterClientCallbacks
****************************************************************************//**
*
* This function registers a callbacks to be called when a message is received on a pipe.
*
* \param ctrlMsgRecvCallBack
* Pointer to the callback to be called when the handle has received a message.
*
* \param ctrlMsgFlushRecvCallBack
* Pointer to the callback to be called when the handle has received a flush message.
*
*  \return
*    CY_IPC_PIPE_SUCCESS          - Callback registered successfully
*    CY_IPC_PIPE_ERROR_BAD_CLIENT - Client ID out of range, callback not registered.
*
*******************************************************************************/
cy_en_ipc_pipe_status_t Cy_BLE_IPC_ControllerRegisterClientCallbacks(cy_ipc_pipe_callback_ptr_t ctrlMsgRecvCallBack,
                                                                     cy_ipc_pipe_callback_ptr_t ctrlMsgFlushRecvCallBack)
{
    cy_en_ipc_pipe_status_t returnStatus;

    Cy_BLE_IPC_CtrlMsgFlushRecvStackHandle = ctrlMsgFlushRecvCallBack;
    
    returnStatus = Cy_IPC_Pipe_RegisterCallback(CY_BLE_IPC_CONTROLLER_ADDR, ctrlMsgRecvCallBack,
                                                CY_BLE_CYPIPE_MSG_SEND_ID);

    if(returnStatus == CY_IPC_PIPE_SUCCESS)
    {
        returnStatus = Cy_IPC_Pipe_RegisterCallback(CY_BLE_IPC_CONTROLLER_ADDR, &Cy_BLE_IPC_CtrlMsgFlushRecvCallBack,
                                                    CY_BLE_CYPIPE_MSG_COMPLETE_ID);
    }

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED)
    if(returnStatus == CY_IPC_PIPE_SUCCESS)
    {
        returnStatus = Cy_IPC_Pipe_RegisterCallback(CY_BLE_IPC_CONTROLLER_ADDR, &Cy_BLE_IPC_ContrIntrFeatureCallBack,
                                                    CY_BLE_CYPIPE_MSG_INTR_NOTIFY_ID);                                      
    }
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) */
    return(returnStatus);
}


/*******************************************************************************
* Function Name: Cy_BLE_IPC_SendMessageToHost
****************************************************************************//**
*
* This function sends a message from the Controller to Host.
*
* \param msg
* Pointer to the message structure to be sent.
*
* \param controllerIpcRelCallBack
* Pointer to the Release callback function.
*
* \param controllerPollCallBack
* Pointer to the callback to be called when the handle has received a message.
*
*  \return
*    CY_IPC_PIPE_SUCCESS    - message was sent to the other end of the pipe
*    CY_IPC_PIPE_BAD_HANDLE - the handle provided for the pipe was not valid
*    CY_IPC_PIPE_SEND_BUSY  - the pipe is already busy sending a message
*    CY_IPC_PIPE_DIR_ERROR  - tried to send on the "to" end of a unidirectional pipe
*
*******************************************************************************/
cy_en_ipc_pipe_status_t Cy_BLE_IPC_SendMessageToHost(uint32_t *msg,
                                                     cy_ipc_pipe_relcallback_ptr_t controllerIpcRelCallBack,
                                                     cy_ipc_pipe_relcallback_ptr_t controllerPollCallBack)
{
    cy_en_ipc_pipe_status_t returnStatus;
    
    returnStatus = Cy_IPC_Pipe_SendMessage(CY_BLE_IPC_HOST_ADDR, CY_BLE_IPC_CONTROLLER_ADDR, msg,
                                           controllerIpcRelCallBack);

    if(returnStatus != CY_IPC_PIPE_SUCCESS)
    {
        Cy_IPC_Pipe_RegisterCallbackRel(CY_BLE_IPC_CONTROLLER_ADDR, controllerPollCallBack);
    }

    return(returnStatus);
}

#endif /* (CY_BLE_STACK_CONTR_CORE) */

#if ((CY_BLE_MODE_PROFILE) && (CY_BLE_HOST_CORE))

/* BLE Stack IPC Host handler */
static cy_ipc_pipe_callback_ptr_t Cy_BLE_IPC_HostMsgRecvStackHandle;
static cy_ipc_pipe_callback_ptr_t Cy_BLE_IPC_HostMsgFlushRecvStackHandle;
    
/* BLE Middleware IPC Host handler */
static void Cy_BLE_IPC_HostMsgFlushRecvCallBack(uint32_t * msgPtr);
static void Cy_BLE_IPC_HostMsgRecvCallBack(uint32_t * msgPtr);   


/*******************************************************************************
* Function Name: Cy_BLE_IPC_HostMsgFlushRecvCallBack
****************************************************************************//**
*
* This a callback to be called when the handle has received a flush message.
*
* \param msgPtr
* Pointer to the callback message.
*
*******************************************************************************/    
static void Cy_BLE_IPC_HostMsgFlushRecvCallBack(uint32_t *msgPtr)
{
    /* Call the BLE Stack IPC handler */
    Cy_BLE_IPC_HostMsgFlushRecvStackHandle(msgPtr);

    /* Call the application notification handler */
    if(Cy_BLE_HostRegisteredCallback != NULL)
    {
        Cy_BLE_HostRegisteredCallback();
    }
}


/*******************************************************************************
* Function Name: Cy_BLE_IPC_HostMsgRecvCallBack
****************************************************************************//**
*
* This a callback to be called when a message is received on a pipe.
*
* \param msgPtr
* Pointer to the callback message.
*
*******************************************************************************/    
static void Cy_BLE_IPC_HostMsgRecvCallBack(uint32_t *msgPtr)
{
    if(msgPtr != NULL)
    {
        /* Call the BLE Stack IPC handler */
        Cy_BLE_IPC_HostMsgRecvStackHandle(msgPtr);
    }
    
    /* Call the application IPC notification handler */
    if(Cy_BLE_HostRegisteredCallback != NULL)
    {
        Cy_BLE_HostRegisteredCallback();
    }
}


/*******************************************************************************
* Function Name: Cy_BLE_IPC_HostRegisterClientCallbacks
****************************************************************************//**
*
* This function registers a callbacks to be called when a message is received on a pipe.
*
* \param hostMsgRecvCallBack
* Pointer to the callback to be called when the handle has received a message.
*
* \param hostMsgFlushRecvCallBack
* Pointer to the callback to be called when the handle has received a flush message.
*
*  \return
*    CY_IPC_PIPE_SUCCESS          - Callback registered successfully
*    CY_IPC_PIPE_ERROR_BAD_CLIENT - Client ID out of range, callback not registered.
*
*******************************************************************************/
cy_en_ipc_pipe_status_t Cy_BLE_IPC_HostRegisterClientCallbacks(cy_ipc_pipe_callback_ptr_t hostMsgRecvCallBack,
                                                               cy_ipc_pipe_callback_ptr_t hostMsgFlushRecvCallBack)
{
    cy_en_ipc_pipe_status_t returnStatus;
    
    Cy_BLE_IPC_HostMsgRecvStackHandle      = hostMsgRecvCallBack;
    Cy_BLE_IPC_HostMsgFlushRecvStackHandle = hostMsgFlushRecvCallBack;
    
    returnStatus = Cy_IPC_Pipe_RegisterCallback(CY_BLE_IPC_HOST_ADDR, &Cy_BLE_IPC_HostMsgRecvCallBack,
                                                CY_BLE_CYPIPE_MSG_SEND_ID);

    if(returnStatus == CY_IPC_PIPE_SUCCESS)
    {
        returnStatus = Cy_IPC_Pipe_RegisterCallback(CY_BLE_IPC_HOST_ADDR, &Cy_BLE_IPC_HostMsgFlushRecvCallBack,
                                                    CY_BLE_CYPIPE_MSG_COMPLETE_ID);
    }

    return(returnStatus);
}


/*******************************************************************************
* Function Name: Cy_BLE_IPC_SendMessageToController
****************************************************************************//**
*
* This function sends a message from the Host to Controller.
*
* \param msg
* Pointer to the message structure to be sent.
*
* \param hostIpcRelCallBack
* Pointer to the Release callback function.
*
* \param hostPollCallBack
* Pointer to the callback to be called when the handle has received a message.
*
*  \return
*    CY_IPC_PIPE_SUCCESS - message was sent to the other end of the pipe
*    CY_IPC_PIPE_BAD_HANDLE - the handle provided for the pipe was not valid
*    CY_IPC_PIPE_SEND_BUSY - the pipe is already busy sending a message
*    CY_IPC_PIPE_DIR_ERROR - tried to send on the "to" end of a unidirectional pipe
*
*******************************************************************************/
cy_en_ipc_pipe_status_t Cy_BLE_IPC_SendMessageToController(uint32_t *msg,
                                                           cy_ipc_pipe_relcallback_ptr_t hostIpcRelCallBack,
                                                           cy_ipc_pipe_relcallback_ptr_t hostPollCallBack)
{
    cy_en_ipc_pipe_status_t returnStatus;

    returnStatus = Cy_IPC_Pipe_SendMessage(CY_BLE_IPC_CONTROLLER_ADDR, CY_BLE_IPC_HOST_ADDR, msg, hostIpcRelCallBack);

    if(returnStatus != CY_IPC_PIPE_SUCCESS)
    {
        Cy_IPC_Pipe_RegisterCallbackRel(CY_BLE_IPC_HOST_ADDR, hostPollCallBack);
    }

    return(returnStatus);
}
#endif /* (CY_BLE_HOST_CORE) */

#endif /* (CY_BLE_STACK_MODE_IPC) */

/* Mapping functions for stack size optimization */
#if (CY_BLE_MODE_PROFILE)

#if (CY_BLE_SECURE_CONN_FEATURE_ENABLED)

void Cy_BLE_HAL_EccHeapInit(uint8_t *heapMem, uint8_t numOfConn)
{
    Cy_BLE_HAL_MappingEccHeapInit(heapMem, numOfConn);
}

void Cy_BLE_HAL_EccHeapDeInit(void)
{
    Cy_BLE_HAL_MappingEccHeapDeInit();
}

uint16_t Cy_BLE_HAL_EccGetFeatureHeapReq(uint8_t numOfConn)
{
    return(Cy_BLE_HAL_MappingEccGetFeatureHeapReq(numOfConn));
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccGenerateSecurityKeypair(uint8_t p_publicKey[], 
                                                             uint8_t p_privateKey[], 
                                                             uint8_t random[])
{
    return(Cy_BLE_HAL_MappingEccGenerateSecurityKeypair(p_publicKey, p_privateKey, random));
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccGenerateDHKey(const uint8_t p_publicKey[], 
                                                   const uint8_t p_privateKey[], 
                                                   uint8_t p_secret[], uint8_t ci)
{
    return(Cy_BLE_HAL_MappingEccGenerateDHKey(p_publicKey, p_privateKey, p_secret, ci));
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccValidPublicKey(const uint8_t p_publicKey[])
{
    return(Cy_BLE_HAL_MappingEccValidPublicKey(p_publicKey));
}
    
cy_en_ble_api_result_t Cy_BLE_HAL_PairingLocalPublicKeyHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingLocalPublicKeyHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingRemoteKeyHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingRemoteKeyHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingDhkeyHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingDhkeyHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingDhkeyCheckHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingDhkeyCheckHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingKeypressNotificationHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingKeypressNotificationHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingRandHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingRandHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingConfirmHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingConfirmHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingLrConfirmingHandler(void *param)
{
    return(Cy_BLE_HAL_MappingPairingLrConfirmingHandler(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxDhkeyGenerateComplete(void *param)
{
    Cy_BLE_HAL_MappingTbxDhkeyGenerateComplete(param);

    return(CY_BLE_SUCCESS);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxLocalPubkeyGenerateComplete(void *param)
{
    Cy_BLE_HAL_MappingTbxLocalPubkeyGenerateComplete(param);

    return(CY_BLE_SUCCESS);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxGenerateLocalP256PublicKey(uint8_t param)
{
    return(Cy_BLE_HAL_MappingTbxGenerateLocalP256PublicKey(param));
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxGenerateDHkey(void  *param1,
                                                            void  *param2,
                                                            uint8_t param3)
{
    return(Cy_BLE_HAL_MappingTbxGenerateDHkey(param1, param2, param3));
}

void Cy_BLE_HAL_SmpScCmacComplete(uint8_t param)
{
    Cy_BLE_HAL_MappingSmpScCmacComplete(param);
}

cy_en_ble_api_result_t Cy_BLE_HAL_SeSmpScUserPasskeyHandler(void *param1,
                                                            void *param2)
{
    return(Cy_BLE_HAL_MappingSeSmpScUserPasskeyHandler(param1, param2));
}

void Cy_BLE_HAL_EccPointMult(uint8_t param)
{
    Cy_BLE_HAL_MappingEccPointMult(param);
}

#else     /* If feature is not required, return error. */

void Cy_BLE_HAL_EccHeapInit(uint8_t *heapMem CY_UNUSED, uint8_t numOfConn CY_UNUSED)
{
 
}

void Cy_BLE_HAL_EccHeapDeInit(void)
{
 
}

uint16_t Cy_BLE_HAL_EccGetFeatureHeapReq(uint8_t numOfConn CY_UNUSED)
{
    return(0u);
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccGenerateSecurityKeypair(uint8_t p_publicKey[] CY_UNUSED, 
                                                             uint8_t p_privateKey[] CY_UNUSED, 
                                                             uint8_t random[] CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccGenerateDHKey(const uint8_t p_publicKey[] CY_UNUSED, 
                                                   const uint8_t p_privateKey[] CY_UNUSED, 
                                                   uint8_t p_secret[] CY_UNUSED, uint8_t ci CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_EccValidPublicKey(const uint8_t p_publicKey[] CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}    
    
    
cy_en_ble_api_result_t Cy_BLE_HAL_PairingLocalPublicKeyHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingRemoteKeyHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingDhkeyHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingDhkeyCheckHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingKeypressNotificationHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingRandHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingConfirmHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingLrConfirmingHandler(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxDhkeyGenerateComplete(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxLocalPubkeyGenerateComplete(void *param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxGenerateLocalP256PublicKey(uint8_t param CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

cy_en_ble_api_result_t Cy_BLE_HAL_PairingScTbxGenerateDHkey(void  *param1 CY_UNUSED,
                                                            void  *param2 CY_UNUSED,
                                                            uint8_t param3 CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

void Cy_BLE_HAL_SmpScCmacComplete(uint8_t param CY_UNUSED)
{
}

cy_en_ble_api_result_t Cy_BLE_HAL_SeSmpScUserPasskeyHandler(void  *param1 CY_UNUSED,
                                                            void  *param2 CY_UNUSED)
{
    return(CY_BLE_ERROR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
}

void Cy_BLE_HAL_EccPointMult(uint8_t param CY_UNUSED)
{
}

#endif /* (CY_BLE_SECURE_CONN_FEATURE_ENABLED) */

#endif /* CY_BLE_MODE_PROFILE */

#endif /* CY_BLE_HOST_CONTR_CORE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* [] END OF FILE */
