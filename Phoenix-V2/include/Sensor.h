#pragma once

#include "ArduinoEigen.h"

struct SensorData
{
    Eigen::Vector3f Magnetic {};

    struct BMP280
    {
        float Temperature;
        float Pressure;
        float Altitude;
    } BMP280 {};

    struct AccelGyroData
    {
        float Temperature;
        Eigen::Vector3f Accel;
        Eigen::Vector3f Gyro;
    } AccelGyroData {};
};

// sensor interface
class ISensor
{
public:
    // places the data into the param
    // true if successfully read and placed
    virtual bool CollectData(SensorData&) = 0;
};
