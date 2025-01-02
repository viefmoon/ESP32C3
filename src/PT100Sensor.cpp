#include "PT100Sensor.h"

PT100Sensor::PT100Sensor(PCA9555& ioExpander) : 
    thermo(&ioExpander, P04, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN),
    measurement(PT100_CONFIG.id, PT100_CONFIG.name, SensorType::TEMPERATURE),
    enabled(SensorEnable::PT100) {
}

bool PT100Sensor::begin() {
    if (!enabled) {
        Serial.println("Sensor PT100 desactivado");
        return true;
    }

    if (!thermo.begin(MAX31865_4WIRE)) {
        Serial.println("Error al inicializar el sensor PT100");
        return false;
    }

    Serial.println("\n=== Información del Sensor PT100 ===");
    Serial.print("ID: ");
    Serial.print(PT100_CONFIG.id);
    Serial.print(" - Nombre: ");
    Serial.println(PT100_CONFIG.name);
    Serial.println("===================================");

    return true;
}

bool PT100Sensor::readSensor() {
    if (!enabled) return false;

    uint16_t rtd = thermo.readRTD();
    float ratio = rtd;
    ratio /= 32768;
    float resistance = R_REF * ratio;
    float temperature = thermo.temperature(R_NOMINAL, R_REF);

    // Verificar si hay errores
    uint8_t fault = readFault();
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
        clearFault();
        return false;
    }

    // Actualizar medición
    unsigned long currentTime = millis();
    measurement = Measurement(PT100_CONFIG.id, PT100_CONFIG.name, SensorType::TEMPERATURE);
    measurement.setTimestamp(currentTime);
    measurement.addVariable("Temperatura", temperature, UNIT_CELSIUS);
    measurement.addVariable("Resistencia", resistance, "Ohm");
    measurement.addVariable("RTD", rtd, "raw");
    measurement.addVariable("Ratio", ratio, "ratio");

    return true;
}

void PT100Sensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensor PT100 desactivado");
        return;
    }
    measurement.print();
}

void PT100Sensor::setWires(max31865_numwires_t wires) {
    if (!enabled) return;
    thermo.setWires(wires);
}

uint8_t PT100Sensor::readFault() {
    return thermo.readFault();
}

void PT100Sensor::clearFault() {
    thermo.clearFault();
} 