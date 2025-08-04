#include "Sensors/LSM6.h"

bool LSM6::Init() { return m_LSM6.begin(); }

bool LSM6::CollectData(SensorData& Data)
{
    AccelGyroData* D = m_LSM6.read();
    if (!D) { return false; }

    Data.AccelGyroData.Temperature = D->temperature;

    Data.AccelGyroData.Accel       = {.X = D->accelX, .Y = D->accelY, .Z = D->accelZ};
    Data.AccelGyroData.Gyro        = {.X = D->gyroX, .Y = D->gyroY, .Z = D->gyroZ};

    // Serial.println("\nAccelGyro Temperature: ");
    // Serial.println(Data.AccelGyroData.Temperature);

    // Serial.println("Accel X: ");
    // Serial.println(Data.AccelGyroData.Accel.x());  
    // Serial.println("Accel Y: ");
    // Serial.println(Data.AccelGyroData.Accel.y());  
    // Serial.println("Accel Z: ");
    // Serial.println(Data.AccelGyroData.Accel.z());

    // Serial.println("Gyro X: ");
    // Serial.println(Data.AccelGyroData.Gyro.x());   
    // Serial.println("Gyro Y: ");
    // Serial.println(Data.AccelGyroData.Gyro.y());  
    // Serial.println("Gyro Z: ");
    // Serial.println(Data.AccelGyroData.Gyro.z());   

    return true;
}

