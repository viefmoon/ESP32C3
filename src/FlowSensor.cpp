#include "FlowSensor.h"

FlowSensor* FlowSensor::instance = nullptr;

void IRAM_ATTR FlowSensor::pulseCounter() {
    if (instance && instance->pulseCount < UINT32_MAX) {
        instance->pulseCount++;
    }
}

bool FlowSensor::begin() {
    if (!enabled) {
        Serial.println("Sensor de flujo desactivado");
        return true;
    }

    powerManager.power12VOn();
    pinMode(FLOW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseCounter, RISING);
    
    resetCounter();
    Serial.println("Sensor de flujo inicializado");
    return true;
}

void FlowSensor::setEnabled(bool state) {
    if (enabled == state) return;  // No hacer nada si el estado no cambia
    
    enabled = state;
    if (!enabled) {
        detachInterrupt(digitalPinToInterrupt(FLOW_PIN));
        powerManager.power12VOff();
    } else {
        begin();
    }
}

bool FlowSensor::calculateFlow(unsigned long currentTime) {
    unsigned long timeDiff = currentTime - lastMeasureTime;
    
    if (timeDiff == 0) {
        Serial.println("Error: Intervalo de tiempo demasiado corto");
        return false;
    }
    
    uint32_t currentCount = pulseCount;
    
    // Verificar desbordamiento
    if (currentCount < lastPulseCount) {
        Serial.println("Advertencia: Desbordamiento del contador de pulsos");
        lastPulseCount = 0;
    }
    
    uint32_t pulseDiff = currentCount - lastPulseCount;
    
    // Calcular flujo (mL/pulso * pulsos) * (60000 ms/min) / (ms transcurridos * 1000 mL/L) = L/min
    flowRate = ((float)pulseDiff * mlPerPulse * 60000.0) / (float)(timeDiff * 1000.0);
    
    // Validar resultado
    if (!SensorUtils::isValidFlow(flowRate)) {
        Serial.println("Error: Cálculo de flujo inválido");
        return false;
    }
    
    lastPulseCount = currentCount;
    lastMeasureTime = currentTime;
    return true;
}

bool FlowSensor::readSensor() {
    if (!enabled) return false;

    unsigned long currentTime = millis();
    
    if (!calculateFlow(currentTime)) {
        return false;
    }
    
    flowMeasurement = Measurement(FLOW_CONFIG.id, FLOW_CONFIG.name, SensorType::FLOW);
    flowMeasurement.setTimestamp(currentTime);
    
    if (!flowMeasurement.addVariable("Flujo", flowRate, "L/min") ||
        !flowMeasurement.addVariable("Pulsos", pulseCount, "pulsos")) {
        Serial.println("Error al agregar variables de medición");
        return false;
    }
    
    return true;
}

void FlowSensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensor de flujo desactivado");
        return;
    }
    flowMeasurement.print();
} 