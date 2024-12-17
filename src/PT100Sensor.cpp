#include "PT100Sensor.h"

bool PT100Sensor::begin() {
    if (!enabled) {
        Serial.println("Sensor PT100 desactivado");
        return true;
    }

    ioExpander.pinMode(csPin, OUTPUT);
    ioExpander.digitalWrite(csPin, HIGH);

    if (!max31865.begin(MAX31865_4WIRE)) {
        Serial.println("No se pudo inicializar el MAX31865");
        enabled = false;
        return false;
    }

    Serial.println("MAX31865 inicializado correctamente para PT100");
    return true;
}

void PT100Sensor::checkFault() {
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

bool PT100Sensor::readSensor() {
    if (!enabled) return false;

       Serial.println("Iniciando lectura PT100...");
       
       float resistance = max31865.readRTD();
       Serial.printf("RTD raw: %f\n", resistance);
       
       float tempC = max31865.temperature(rNominal, rRef);
       Serial.printf("Valores usados - rNominal: %d, rRef: %d\n", rNominal, rRef);
       
       if (isnan(tempC)) {
           Serial.println("Error al leer temperatura del PT100");
           checkFault();
           return false;
       }

    unsigned long currentTime = millis();
    tempMeasurement = Measurement(PT100_CONFIG.id, PT100_CONFIG.name, SensorType::TEMPERATURE);
    tempMeasurement.setTimestamp(currentTime);
    tempMeasurement.addVariable("Temperatura", tempC, UNIT_CELSIUS);
    tempMeasurement.addVariable("Resistencia", resistance, "Ohm");

    return true;
}

void PT100Sensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensor PT100 desactivado");
        return;
    }
    tempMeasurement.print();
}