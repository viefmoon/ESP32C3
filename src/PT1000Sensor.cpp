#include "PT1000Sensor.h"

bool PT1000Sensor::begin() {
    if (!enabled) {
        Serial.println("Sensor PT1000 desactivado");
        return true;
    }

    ioExpander.pinMode(csPin, OUTPUT);
    ioExpander.digitalWrite(csPin, HIGH);

    if (!max31865.begin(MAX31865_3WIRE)) {
        Serial.println("No se pudo inicializar el MAX31865");
        enabled = false;
        return false;
    }

    Serial.println("MAX31865 inicializado correctamente");
    return true;
}

void PT1000Sensor::checkFault() {
    uint8_t fault = max31865.readFault();
    if (fault) {
        Serial.print("Fault 0x"); Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
            Serial.println("RTD High Threshold");
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
            Serial.println("RTD Low Threshold");
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
            Serial.println("REFIN- > 0.85 x Bias");
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
            Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
            Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_OVUV) {
            Serial.println("Under/Over voltage");
        }
        max31865.clearFault();
    }
}

bool PT1000Sensor::readSensor() {
    if (!enabled) return false;

    float tempC = max31865.temperature(rNominal, rRef);
    
    if (isnan(tempC)) {
        Serial.println("Error al leer temperatura del PT1000");
        checkFault();
        return false;
    }

    unsigned long currentTime = millis();
    tempMeasurement = Measurement(PT1000_CONFIG.id, PT1000_CONFIG.name, SensorType::TEMPERATURE);
    tempMeasurement.setTimestamp(currentTime);
    tempMeasurement.addVariable("Temperatura", tempC, UNIT_CELSIUS);
    
    float resistance = max31865.readRTD();
    tempMeasurement.addVariable("Resistencia", resistance, "Ohm");

    return true;
}

void PT1000Sensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensor PT1000 desactivado");
        return;
    }
    tempMeasurement.print();
} 