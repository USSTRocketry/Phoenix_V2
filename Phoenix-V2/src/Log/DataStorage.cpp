/**
 * @file DataStorage.cpp
 * @author USST Avionics
 * @brief This file has all the logic for saving telemetry data to the SD and flash
 * @version 0.1
 * @date 2025-06-28
 *
 * @copyright Copyright (c) 2025
 *
 */

// #include <Arduino.h>
#include "Log/DataStorage.h"
#include "SensorInterface.h"
#include <string>

#include <SerialFlash.h>
#include <SD.h>
#include <SPI.h>

const int SDchipSelect    = 4; // Audio Shield has SD card CS on pin 10
const int FlashChipSelect = 6; // digital pin for flash chip CS pin
// const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash

void InitSdCard()
{
    if (!SD.begin(SDchipSelect))
    {
        // error("Unable to access SD card");
    }
    if (!SerialFlash.begin(FlashChipSelect))
    {
        // error("Unable to access SPI Flash chip");
    }

    if (SD.exists("/FlightData.fdat")) { SD.remove("/FlightData.fdat"); }

    SerialFlash.create("/FlightData.fdat", sizeof("/FlightData.fdat"));
    File file = SD.open("/FlightData.fdat", O_CREAT & O_WRITE);

    file.write("BarVal,Thermoval,accel_x,accell_y,accell_z,Gyro_x,Gyro_y,Gyro_z,timestamp\n");

    file.close();
}

void InitDataStorage()
{
    Serial.printf("InitDataStorage()");
    StoreStringLine(
        "State,Altitude,Pressure,Temperature,Acceleration_X,Acceleration_Y,Acceleration_Z,gyroX,gyroY,gyroZ,\
    magneticX,magneticY,magneticX,Timestamp");
}

void StoreStringLine(std::string s)
{
    File file = SD.open("/FlightData.fdat", O_APPEND);

    file.write(s.c_str());

    file.close();
}

void TransferFileData(File to)
{
    File from           = SD.open("/FlightData.fdat", O_READ);
    unsigned long count = from.size();
    while (count > 0)
    {
        char buff[128];
        unsigned long n = count;
        if (n > 128) n = 128;
        from.read(buff, n);
        to.write(buff, n);
        count = count - n;
    }
    from.close();
}

std::string ConvertDataToCSVRow(FlightState State, const SensorData& SensorData)
{
    /* State,Altitude,Pressure,Temperature,Acceleration_X,Acceleration_Y,Acceleration_Z,gyroX,gyroY,gyroZ,
        magneticX,magneticY,magneticX,Timestamp
    */

    return std::to_string(State) + "," +

           std::to_string(SensorData.BMP280.Altitude) + "," + std::to_string(SensorData.BMP280.Pressure) + "," +
           std::to_string(SensorData.BMP280.Temperature) + "," +

           std::to_string(SensorData.AccelGyroData.Accel.x()) + "," +
           std::to_string(SensorData.AccelGyroData.Accel.y()) + "," +
           std::to_string(SensorData.AccelGyroData.Accel.z()) + "," +

           std::to_string(SensorData.AccelGyroData.Gyro.x()) + "," + std::to_string(SensorData.AccelGyroData.Gyro.z()) +
           "," + std::to_string(SensorData.AccelGyroData.Gyro.z()) + "," +

           std::to_string(SensorData.Magnetic.x()) + "," + std::to_string(SensorData.Magnetic.y()) + "," +
           std::to_string(SensorData.Magnetic.z()) + "," +

           std::to_string(millis()) + "\n";
}

void StoreData(FlightState S, const SensorData& SensorData) { StoreStringLine(ConvertDataToCSVRow(S, SensorData)); }
