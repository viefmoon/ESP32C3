#ifndef SHT40_SENSOR_H
#define SHT40_SENSOR_H

#include <Adafruit_SHT4x.h>
#include "Measurement.h"
#include "config.h"
#include "sensor_config.h"

class SHT40Sensor {
private:
    Adafruit_SHT4x sht4;
    Measurement tempMeasurement;
    Measurement humMeasurement;
    bool enabled;
    
public:
    SHT40Sensor();
    
    bool begin();
    bool readSensor();
    const Measurement& getTemperatureMeasurement() const { return tempMeasurement; }
    const Measurement& getHumidityMeasurement() const { return humMeasurement; }
    void printMeasurements();
    
    // Control de activación
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state) { enabled = state; }
    
    // Configuración de precisión
    void setHighPrecision();
    void setMediumPrecision();
    void setLowPrecision();
};

#endif 