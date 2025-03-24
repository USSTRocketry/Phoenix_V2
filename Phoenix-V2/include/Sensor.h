#pragma once

struct SensorData
{
    template <typename T>
    struct XYZ
    {
        T X;
        T Y;
        T Z;
    };

    XYZ<float> Magnetic;

    struct BMP280
    {
        float Temperature;
        float Pressure;
        float Altitude;
    } BMP280;
    struct AccelGyroData
    {
        float Temperature;
        XYZ<float> Accel;
        XYZ<float> Gyro;
    } AccelGyroData;
};

// sensor interface
class Sensor
{
public:
    // places the data into the param
    // true if successfully read and placed
    virtual bool CollectData(SensorData&) = 0;
};
