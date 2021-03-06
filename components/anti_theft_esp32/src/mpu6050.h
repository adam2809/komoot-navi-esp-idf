#pragma once

#include "i2c_manager.h"

#define SIGNAL_PATH_RESET  0x68
#define INT_PIN_CFG      0x37
#define ACCEL_CONFIG       0x1C
#define MOT_THR            0x1F  // Motion detection threshold bits [7:0]
#define MOT_DUR            0x20  // This seems wrong // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MOT_DETECT_CTRL    0x69
#define INT_ENABLE         0x38
#define PWR_MGMT_1          0x6B //SLEEPY TIME
#define INT_STATUS 0x3A
#define MPU6050_ADDRESS 0x68 //AD0 is 0
#define PWR_MGMT_2       0x6C

#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MOTION_DETECTION_SENSITIVITY 10

void configure_mpu();