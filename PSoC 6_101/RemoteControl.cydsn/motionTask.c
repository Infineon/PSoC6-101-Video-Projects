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
#include <stdio.h>
#include "bmi160.h"
#include "bleTask.h"
#include "motionTask.h"

static struct bmi160_dev bmi160Dev;

static int8_t BMI160BurstWrite(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len)
{  
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context);
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context);
    for(int i = 0;i<len; i++)
    { 
        Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,data[i],0,&I2C_1_context);
    }
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context);
    
    return 0;
}

// This function supports the BMP180 library and read I2C Registers
static int8_t BMI160BurstRead(uint8_t dev_addr, uint8_t reg_addr,uint8_t *data, uint16_t len)
{   
    Cy_SCB_I2C_MasterSendStart(I2C_1_HW,dev_addr,CY_SCB_I2C_WRITE_XFER,0,&I2C_1_context);
    Cy_SCB_I2C_MasterWriteByte(I2C_1_HW,reg_addr,0,&I2C_1_context);
    Cy_SCB_I2C_MasterSendReStart(I2C_1_HW,dev_addr,CY_SCB_I2C_READ_XFER,0,&I2C_1_context);
    for(int i = 0;i<len-1; i++)
    {
        Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_ACK,&data[i],0,&I2C_1_context);
    }
    Cy_SCB_I2C_MasterReadByte(I2C_1_HW,CY_SCB_I2C_NAK,&data[len-1],0,&I2C_1_context);
    
    Cy_SCB_I2C_MasterSendStop(I2C_1_HW,0,&I2C_1_context);

    return 0;
}

static void bmi160Init(void)
{

    vTaskDelay(100);

  /* BMI160 */
    bmi160Dev.read = (bmi160_com_fptr_t)BMI160BurstRead;
    bmi160Dev.write = (bmi160_com_fptr_t)BMI160BurstWrite;
    bmi160Dev.delay_ms = (bmi160_delay_fptr_t)vTaskDelay;
    bmi160Dev.id = BMI160_I2C_ADDR;  // I2C device address

    bmi160_init(&bmi160Dev); // initialize the device
  
    bmi160Dev.gyro_cfg.odr = BMI160_GYRO_ODR_800HZ;
    bmi160Dev.gyro_cfg.range = BMI160_GYRO_RANGE_125_DPS;
    bmi160Dev.gyro_cfg.bw = BMI160_GYRO_BW_OSR4_MODE;

      /* Select the power mode of Gyroscope sensor */
    bmi160Dev.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    bmi160Dev.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
    bmi160Dev.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    bmi160Dev.accel_cfg.bw = BMI160_ACCEL_BW_OSR4_AVG1;
    bmi160Dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

      /* Set the sensor configuration */
    bmi160_set_sens_conf(&bmi160Dev);
    bmi160Dev.delay_ms(50);
}

// 32768 = 2g
#define MAXACCEL (32768/2)

void motionTask(void *arg)
{
    (void)arg;
    I2C_1_Start();
    bmi160Init();
    struct bmi160_sensor_data acc;
    
    TickType_t lastMovement = 0;
    
    double gx,gy;
    double m1,m2;
    
    while(1)
    {
        bmi160_get_sensor_data(BMI160_ACCEL_ONLY, &acc, NULL, &bmi160Dev);
 
        // Convert counts to G 
        gx = (double)acc.x/MAXACCEL;
        gy = (double)acc.y/MAXACCEL;
        
        // Cap the readings at 1.0g
        if(gx>1.0) gx=1.0;
        if(gx<-1.0) gx =-1.0;
        if(gy>1.0) gy=1.0;
        if(gy<-1.0) gy =-1.0;
        
        // Calculate a number between 0 and 100 based on the angle
        m1 = acos(gx)*360/(2*3.14*1.8);
        m2 = acos(gy)*360/(2*3.14*1.8);
 
        // If the board moves more than 3% of motor change then say it moved
        if( fabs(m1-50.0) > 3.0 || fabs(m2-50.0) > 3.0)
            lastMovement = xTaskGetTickCount();
        
        // if it has been more than a second then turn on capsense and LED
        if( (xTaskGetTickCount() - lastMovement) > 1000)
        {
            xEventGroupSetBits(systemInputMode,MODE_CAPSENSE);
            xEventGroupClearBits(systemInputMode,MODE_MOTION);  
            Cy_GPIO_Write(LED8_PORT,LED8_NUM,0);
        }
        else  
        {
            xEventGroupClearBits(systemInputMode,MODE_CAPSENSE);
            xEventGroupSetBits(systemInputMode,MODE_MOTION);
            Cy_GPIO_Write(LED8_PORT,LED8_NUM,1);
        }
      
        
        if(xEventGroupGetBits(systemInputMode) == MODE_MOTION)
        {
            writeMotorPosition(M1,ABSOLUTE,(int)m1);
            writeMotorPosition(M2,ABSOLUTE,(int)m2);
        }
    }
}
/* [] END OF FILE */
