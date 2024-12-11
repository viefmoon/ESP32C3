#include "PowerManager.h"

PowerManager::PowerManager(PCA9555& expander) : ioExpander(expander) {
}

bool PowerManager::begin() {
    // Configurar pines como salidas
    ioExpander.pinMode(POWER_3V3_PIN, OUTPUT);
    ioExpander.pinMode(POWER_12V_PIN, OUTPUT);
    
    // Asegurar que todas las fuentes están apagadas al inicio
    allPowerOff();
    return true;
}

void PowerManager::power3V3On() {
    ioExpander.digitalWrite(POWER_3V3_PIN, HIGH);
    Serial.println("Fuente de 3.3V encendida");
    delay(POWER_STABILIZE_DELAY);
}

void PowerManager::power3V3Off() {
    ioExpander.digitalWrite(POWER_3V3_PIN, LOW);
    Serial.println("Fuente de 3.3V apagada");
}

void PowerManager::power12VOn() {
    ioExpander.digitalWrite(POWER_12V_PIN, HIGH);
    Serial.println("Fuente de 12V encendida");
    delay(POWER_STABILIZE_DELAY);
}

void PowerManager::power12VOff() {
    ioExpander.digitalWrite(POWER_12V_PIN, LOW);
    Serial.println("Fuente de 12V apagada");
}

void PowerManager::allPowerOff() {
    power3V3Off();
    power12VOff();
    Serial.println("Todas las fuentes apagadas");
} 