#ifndef ADS131M04_SENSOR_H
#define ADS131M04_SENSOR_H

#include "ADS131M04.h"
#include "Measurement.h"
#include "sensor_config.h"
#include "clsPCA9555.h"

class ADS131M04Sensor {
private:
    
    PCA9555& ioExpander;
    ADS131M04 adc1;
    ADS131M04 adc2;
    Measurement measurement;
    bool enabled;

public:
    ADS131M04Sensor(PCA9555& expander);
    
    bool begin();
    bool readSensor();
    void printMeasurements();
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state) { enabled = state; }
    const Measurement& getMeasurement() const { return measurement; }
};

#endif