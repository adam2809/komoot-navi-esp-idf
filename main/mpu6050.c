#include "mpu6050.h"

const uint8_t data[] = {
    0x00,
    0x07,
    0x01,
    40,
    0x15,
    140,
    0x40,
    8,
    7
};
void configure_mpu(int sens){
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_1 ,&data[0],1);
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,SIGNAL_PATH_RESET ,&data[1],1);//Reset all internal signal paths in the MPU-6050 by writing 0x07 to register 0x68;
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,ACCEL_CONFIG ,&data[2],1);//Write register 28 (==0x1C) to set the Digital High Pass Filter, bits 3:0. For example set it to 0x01 for 5Hz. (These 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.)
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_THR ,&sens,1);  //Write the desired Motion threshold to register 0x1F (For example, write decimal 20).
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_DUR ,&data[3],1 );  //Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,MOT_DETECT_CTRL ,&data[4],1); //to register 0x69, write the motion detection decrement and a few other settings (for example write 0x15 to set both free-fall and motion decrements to 1 and accelerometer start-up delay to 5ms total by adding 1ms. )
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,I2C_SLV0_ADDR ,&data[5],1 ); // now INT pin is active low
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,INT_ENABLE ,&data[6],1 ); //write register 0x38, bit 6 (0x40), to enable motion detection interrupt.
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_1 ,&data[7],1); // 101000 - Cycle & disable TEMP SENSOR
    i2c_manager_write(I2C_NUM_0, MPU6050_ADDRESS ,PWR_MGMT_2 ,&data[8],1);
}