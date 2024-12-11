#ifndef PT1000_SENSOR_H
#define PT1000_SENSOR_H

#include <Adafruit_MAX31865.h>
#include "Measurement.h"
#include "config.h"
#include "clsPCA9555.h"
#include "sensor_config.h"

class PT1000Sensor {
private:
    const uint16_t rRef;
    const uint16_t rNominal;
    const uint8_t csPin;
    
    PCA9555& ioExpander;
    Adafruit_MAX31865 max31865;
    Measurement tempMeasurement;
    bool enabled;

    void checkFault();

public:
    PT1000Sensor(PCA9555& expander) :
        rRef(4300),
        rNominal(1000),
        csPin(P04),
        ioExpander(expander),
        max31865(10, 7, 6, csPin),
        tempMeasurement(PT1000_CONFIG.id, PT1000_CONFIG.name, SensorType::TEMPERATURE),
        enabled(SensorEnable::PT1000) {}
    
    bool begin();
    bool readSensor();
    const Measurement& getTemperatureMeasurement() const { return tempMeasurement; }
    void printMeasurements();
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state) { enabled = state; }
};

#endif 