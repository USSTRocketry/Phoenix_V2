#include "Filter/LowPass.h"

void print_sensor_data(const SensorData* sensorData) {
    Serial.printf("----------------------------------------\n");
    Serial.printf("BMP280 - Temp: %.2f °C, Pressure: %.2f Pa, Altitude: %.2f m\n",
                sensorData->BMP280.Temperature,
                sensorData->BMP280.Pressure,
                sensorData->BMP280.Altitude);

    Serial.printf("Accel: %.2f, %.2f, %.2f\n",
                sensorData->AccelGyroData.Accel.x(),
                sensorData->AccelGyroData.Accel.y(),
                sensorData->AccelGyroData.Accel.z());

    Serial.printf("Gyro: %.2f, %.2f, %.2f\n",
                sensorData->AccelGyroData.Gyro.x(),
                sensorData->AccelGyroData.Gyro.y(),
                sensorData->AccelGyroData.Gyro.z());
               

    Serial.printf("Magnetic Field: %.2f, %.2f, %.2f\n",
                sensorData->Magnetic.x(),
                sensorData->Magnetic.y(),
                sensorData->Magnetic.z());


    Serial.printf("---\n\n");
}

namespace Filter
{
    SensorData LowPass::Filter(const SensorData& NewData) { 
        
        SensorData temp = m_History * m_Alpha + NewData * (1 - m_Alpha); 
        // Serial.println("Temp: ");
        // print_sensor_data(&temp);
        
        m_History = temp;
        return m_History;
    }
} // namespace Filter
