#include "mpu6050.h"

MPU6050::MPU6050() {}

void MPU6050::mpu6050Init()
{
    this->salveAddr = MPU6050_ADDR;
    this->i2cWriteByte(MPU6050_PWR_MGMT_1,0x01);
    this->i2cWriteByte(MPU6050_PWR_MGMT_2,0x00);
    this->i2cWriteByte(MPU6050_SMPLRT_DIV,0x09);
    this->i2cWriteByte(MPU6050_CONFIG,0x06);
    this->i2cWriteByte(MPU6050_SMPLRT_DIV,0x09);
    this->i2cWriteByte(MPU6050_GYRO_CONFIG,0x18);
    this->i2cWriteByte(MPU6050_ACCEL_CONFIG,0x18);
}
void MPU6050::mpu6050GetData()
{
    uint8_t Data_H,Data_L;
    Data_H=this->i2cReadByte(MPU6050_ACCEL_XOUT_H);
    Data_L=this->i2cReadByte(MPU6050_ACCEL_XOUT_L);
    this->accXRawData=(Data_H<<8)|Data_L;

    Data_H=this->i2cReadByte(MPU6050_ACCEL_YOUT_H);
    Data_L=this->i2cReadByte(MPU6050_ACCEL_YOUT_L);
    this->accYRawData=(Data_H<<8)|Data_L;

    Data_H=this->i2cReadByte(MPU6050_ACCEL_ZOUT_H);
    Data_L=this->i2cReadByte(MPU6050_ACCEL_ZOUT_L);
    this->accZRawData=(Data_H<<8)|Data_L;

    Data_H=this->i2cReadByte(MPU6050_GYRO_XOUT_H);
    Data_L=this->i2cReadByte(MPU6050_GYRO_XOUT_L);
    this->gyroXRawData=(Data_H<<8)|Data_L;

    Data_H=this->i2cReadByte(MPU6050_GYRO_YOUT_H);
    Data_L=this->i2cReadByte(MPU6050_GYRO_YOUT_L);
    this->gyroYRawData=(Data_H<<8)|Data_L;

    Data_H=this->i2cReadByte(MPU6050_GYRO_ZOUT_H);
    Data_L=this->i2cReadByte(MPU6050_GYRO_ZOUT_L);
    this->gyroZRawData=(Data_H<<8)|Data_L;
}

