#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>
#include "Measurement.h"
#include "PowerManager.h"
#include "sensor_config.h"
#include "sensor_utils.h"

class FlowSensor {
private:
    static const uint8_t FLOW_PIN = 18;  // Pin fijo en la PCB
    const uint16_t measureInterval;
    const float mlPerPulse;
    
    PowerManager& powerManager;
    Measurement flowMeasurement;
    bool enabled;
    
    volatile uint32_t pulseCount;
    uint32_t lastPulseCount;
    unsigned long lastMeasureTime;
    float flowRate;  // en L/min

    static FlowSensor* instance;
    static void IRAM_ATTR pulseCounter();

    // Método privado para calcular el flujo
    bool calculateFlow(unsigned long currentTime);

public:
    FlowSensor(PowerManager& power) :
        measureInterval(FLOW_CONFIG.measureInterval),
        mlPerPulse(FLOW_CONFIG.mlPerPulse),
        powerManager(power),
        flowMeasurement(FLOW_CONFIG.id, FLOW_CONFIG.name, SensorType::FLOW),
        enabled(SensorEnable::FLOW),
        pulseCount(0),
        lastPulseCount(0),
        lastMeasureTime(0),
        flowRate(0.0) {
        instance = this;
    }
    
    ~FlowSensor() { 
        if (instance == this) {
            instance = nullptr;
            setEnabled(false);  // Asegurar que se deshabilita correctamente
        }
    }
    
    bool begin();
    bool readSensor();
    const Measurement& getFlowMeasurement() const { return flowMeasurement; }
    void printMeasurements();
    float getCurrentFlowRate() const { return flowRate; }
    
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state);

    bool isTimeToMeasure() const {
        return (millis() - lastMeasureTime) >= measureInterval;
    }

    // Método para reiniciar el contador
    void resetCounter() {
        pulseCount = 0;
        lastPulseCount = 0;
        lastMeasureTime = millis();
        flowRate = 0.0;
    }
};

#endif 