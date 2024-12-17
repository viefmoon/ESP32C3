#ifndef PT100_SENSOR_H
#define PT100_SENSOR_H

#include <Adafruit_MAX31865.h>
#include "Measurement.h"
#include "config.h"
#include "clsPCA9555.h"
#include "sensor_config.h"

class PT100Sensor {
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
    PT100Sensor(PCA9555& expander) :
        rRef(PT100_CONFIG.rRef),
        rNominal(PT100_CONFIG.rNominal),
        csPin(P04),
        ioExpander(expander),
        max31865(expander, csPin),
        tempMeasurement(PT100_CONFIG.id, PT100_CONFIG.name, SensorType::TEMPERATURE),
        enabled(SensorEnable::PT100) {}
    
    bool begin();
    bool readSensor();
    const Measurement& getTemperatureMeasurement() const { return tempMeasurement; }
    void printMeasurements();
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state) { enabled = state; }
};

#endif 