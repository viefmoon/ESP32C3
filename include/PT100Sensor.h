#ifndef PT100_SENSOR_H
#define PT100_SENSOR_H

#include <Adafruit_MAX31865.h>
#include "Measurement.h"
#include "config.h"
#include "sensor_config.h"
#include "clsPCA9555.h"

class PT100Sensor {
private:
    static constexpr float R_REF = 430.0f;      // Resistencia de referencia fija
    static constexpr float R_NOMINAL = 100.0f;   // Resistencia nominal fija a 0°C
    
    Adafruit_MAX31865 thermo;
    Measurement measurement;
    bool enabled;
    
public:
    PT100Sensor(PCA9555& ioExpander);
    
    bool begin();
    bool readSensor();
    void printMeasurements();
    
    // Getters
    const Measurement& getMeasurement() const { return measurement; }
    bool isEnabled() const { return enabled; }
    
    // Configuración
    void setEnabled(bool state) { enabled = state; }
    void setWires(max31865_numwires_t wires);
    uint8_t readFault();
    void clearFault();
};

#endif // PT100_SENSOR_H 