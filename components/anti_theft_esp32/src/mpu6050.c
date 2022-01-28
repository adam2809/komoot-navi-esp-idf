#include "mpu6050.h"

void configure_mpu(){
    uint8_t data = 0x00;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_1 ,&data,1);
    data = 0x07;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,SIGNAL_PATH_RESET ,&data,1);//Reset all internal signal paths in the MPU-6050 by writing 0x07 to register 0x68;
    data = 0x01;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,ACCEL_CONFIG ,&data,1);//Write register 28 (==0x1C) to set the Digital High Pass Filter, bits 3:0. For example set it to 0x01 for 5Hz. (These 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.)
    data = MOTION_DETECTION_SENSITIVITY;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_THR ,&data,1);  //Write the desired Motion threshold to register 0x1F (For example, write decimal 20).
    data = 0x28;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_DUR ,&data,1);  //Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate
    data = 0x15;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_DETECT_CTRL ,&data,1); //to register 0x69, write the motion detection decrement and a few other settings (for example write 0x15 to set both free-fall and motion decrements to 1 and accelerometer start-up delay to 5ms total by adding 1ms. )
    data = 0x0C;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,INT_PIN_CFG ,&data,1); // now INT pin is active low
    data = 0x40;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,INT_ENABLE ,&data,1); //write register 0x38, bit 6 (0x40), to enable motion detection interrupt.
    data = 0x08;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_1 ,&data,1); // 101000 - Cycle & disable TEMP SENSOR
    data = 0x08;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_2 ,&data,1);
}


