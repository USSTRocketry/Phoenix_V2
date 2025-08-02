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
        Eigen::Vector3f Accel{};
        Eigen::Vector3f Gyro{};
    } AccelGyroData {};

public:
    SensorData operator*(float Factor) const
    {
        return {
            .Magnetic = Magnetic * Factor,

            .BMP280 {BMP280.Temperature * Factor,     BMP280.Pressure * Factor,    BMP280.Altitude * Factor                     },

            .AccelGyroData {
                     AccelGyroData.Temperature * Factor, AccelGyroData.Accel * Factor, AccelGyroData.Gyro * Factor}
        };
    }

    SensorData operator+(const SensorData& Other) const
    {
        return {
            .Magnetic = Magnetic + Other.Magnetic,

            .BMP280 {              BMP280.Temperature + Other.BMP280.Temperature,
                     BMP280.Pressure + Other.BMP280.Pressure,
                     BMP280.Altitude + Other.BMP280.Altitude      },

            .AccelGyroData {AccelGyroData.Temperature + Other.AccelGyroData.Temperature,
                     AccelGyroData.Accel + Other.AccelGyroData.Accel,
                     AccelGyroData.Gyro + Other.AccelGyroData.Gyro}
        };
    }
};

// sensor interface
template <typename SensorDataType>
class ISensor
{
public:
    // places the data into the param
    // true if successfully read and placed
    virtual bool CollectData(SensorDataType&) = 0;
};
