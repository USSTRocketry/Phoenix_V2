#pragma once

#include "ArduinoEigen.h"

struct SensorData
{
    using MagnetometerData = Eigen::Vector3f;
    MagnetometerData Magnetic {Eigen::Vector3f::Zero()};

    struct BMP280Data
    {
        float Temperature;
        float Pressure;
        float Altitude;
    } BMP280 {};

    struct AccelGyroData
    {
        float Temperature;
        Eigen::Vector3f Accel {Eigen::Vector3f::Zero()};
        Eigen::Vector3f Gyro {Eigen::Vector3f::Zero()};
    } AccelGyro {};

public:
    SensorData operator*(float Factor) const
    {
        return {
            .Magnetic = Magnetic * Factor,

            .BMP280 {   BMP280.Temperature * Factor, BMP280.Pressure * Factor, BMP280.Altitude * Factor},

            .AccelGyro {AccelGyro.Temperature * Factor, AccelGyro.Accel * Factor,  AccelGyro.Gyro * Factor}
        };
    }

    SensorData operator+(const SensorData& Other) const
    {
        return {
            .Magnetic = Magnetic + Other.Magnetic,

            .BMP280 {      BMP280.Temperature + Other.BMP280.Temperature,
                     BMP280.Pressure + Other.BMP280.Pressure,
                     BMP280.Altitude + Other.BMP280.Altitude},

            .AccelGyro {AccelGyro.Temperature + Other.AccelGyro.Temperature,
                     AccelGyro.Accel + Other.AccelGyro.Accel,
                     AccelGyro.Gyro + Other.AccelGyro.Gyro  }
        };
    }
};
