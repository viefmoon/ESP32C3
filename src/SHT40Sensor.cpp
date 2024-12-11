#include "SHT40Sensor.h"

SHT40Sensor::SHT40Sensor() : 
    tempMeasurement(SHT40_CONFIG.temperature.id, 
                   SHT40_CONFIG.temperature.name, 
                   SensorType::TEMPERATURE),
    humMeasurement(SHT40_CONFIG.humidity.id,
                  SHT40_CONFIG.humidity.name,
                  SensorType::HUMIDITY),
    enabled(ENABLE_SHT40) {
}

bool SHT40Sensor::begin() {
    if (!enabled) {
        Serial.println("Sensor SHT40 desactivado");
        return true;
    }

    if (!sht4.begin()) {
        Serial.println("No se encontró el sensor SHT40");
        return false;
    }
    
    Serial.println("\n=== Información del Sensor SHT40 ===");
    Serial.print("Temperatura - ID: ");
    Serial.print(SHT40_CONFIG.temperature.id);
    Serial.print(" - Nombre: ");
    Serial.println(SHT40_CONFIG.temperature.name);
    
    Serial.print("Humedad - ID: ");
    Serial.print(SHT40_CONFIG.humidity.id);
    Serial.print(" - Nombre: ");
    Serial.println(SHT40_CONFIG.humidity.name);
    Serial.println("===================================");
    
    // Configuración inicial
    setHighPrecision();
    return true;
}

bool SHT40Sensor::readSensor() {
    if (!enabled) return false;

    sensors_event_t humidity_event, temp_event;
    
    if (!sht4.getEvent(&humidity_event, &temp_event)) {
        Serial.println("Error al leer el sensor SHT40");
        return false;
    }
    
    // Actualizar timestamp y mediciones
    unsigned long currentTime = millis();
    
    tempMeasurement = Measurement(SHT40_CONFIG.temperature.id, 
                                SHT40_CONFIG.temperature.name, 
                                SensorType::TEMPERATURE);
    tempMeasurement.setTimestamp(currentTime);
    tempMeasurement.addVariable("Temperatura", temp_event.temperature, UNIT_CELSIUS);
    
    humMeasurement = Measurement(SHT40_CONFIG.humidity.id,
                               SHT40_CONFIG.humidity.name,
                               SensorType::HUMIDITY);
    humMeasurement.setTimestamp(currentTime);
    humMeasurement.addVariable("Humedad", humidity_event.relative_humidity, UNIT_HUMIDITY);
    
    return true;
}

void SHT40Sensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensor SHT40 desactivado");
        return;
    }
    tempMeasurement.print();
    humMeasurement.print();
}

void SHT40Sensor::setHighPrecision() {
    if (!enabled) return;
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
}

void SHT40Sensor::setMediumPrecision() {
    if (!enabled) return;
    sht4.setPrecision(SHT4X_MED_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
}

void SHT40Sensor::setLowPrecision() {
    if (!enabled) return;
    sht4.setPrecision(SHT4X_LOW_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
} 