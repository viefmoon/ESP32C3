#include <Arduino.h>
#include "RTCManager.h"
#include "SHT40Sensor.h"
#include "DS18B20Sensor.h"
#include "PT1000Sensor.h"
#include "FlowSensor.h"
#include "PowerManager.h"
#include "config.h"
#include "sensor_config.h"

// Create instances
RTCManager rtcManager(WIFI_SSID, WIFI_PASSWORD, NTP_SERVER, GMT_OFFSET, DAYLIGHT_OFFSET);
SHT40Sensor sht40;
DS18B20Sensor ds18b20;
PCA9555 ioExpander(0x20);
PowerManager powerManager(ioExpander);
PT1000Sensor pt1000(ioExpander);
FlowSensor flowSensor(powerManager);

void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch(wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Despertar causado por timer");
            break;
        default:
            Serial.println("Inicio del sistema");
            break;
    }
}

void performMeasurements() {
    // Encender fuente de 3.3V para los sensores
    powerManager.power3V3On();
    
    // Mostrar fecha y hora
    rtcManager.printDateTime();
    
    bool anyMeasurementSuccess = false;
    
    // Leer y mostrar datos del SHT40 si está activado
    if (SensorEnable::SHT40 && sht40.isEnabled()) {
        if (sht40.readSensor()) {
            sht40.printMeasurements();
            anyMeasurementSuccess = true;
        } else {
            Serial.println("Error en la lectura del SHT40");
        }
    }
    
    // Leer y mostrar datos de los DS18B20 si están activados
    if (SensorEnable::DS18B20 && ds18b20.isEnabled()) {
        if (ds18b20.readSensor()) {
            ds18b20.printMeasurements();
            anyMeasurementSuccess = true;
        } else {
            Serial.println("Error en la lectura de DS18B20");
        }
    }

    // Leer y mostrar datos del PT1000 si está activado
    if (SensorEnable::PT1000 && pt1000.isEnabled()) {
        if (pt1000.readSensor()) {
            pt1000.printMeasurements();
            anyMeasurementSuccess = true;
        } else {
            Serial.println("Error en la lectura del PT1000");
        }
    }

    // Verificar si hubo al menos una medición exitosa
    if (!anyMeasurementSuccess) {
        Serial.println("¡Advertencia! Ninguna medición fue exitosa");
    }

    // Apagar fuente después de las mediciones
    powerManager.power3V3Off();
}

void goToSleep() {
    // Solo ir a dormir si el sensor de flujo está desactivado
    if (!flowSensor.isEnabled()) {
        Serial.println("Entrando en modo Deep Sleep por " + String(TIME_TO_SLEEP) + " segundos");
        Serial.flush();
        
        // Asegurar que todas las fuentes están apagadas antes de dormir
        powerManager.allPowerOff();
        
        // Configurar el timer para despertar
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
        
        // Entrar en deep sleep
        esp_deep_sleep_start();
    }
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    
    // Verificar memoria disponible
    uint32_t freeHeap = ESP.getFreeHeap();
    Serial.printf("Memoria libre: %u bytes\n", freeHeap);
    
    print_wakeup_reason();
    
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    SPI.begin(10, 6, 7, -1);
    
    bool setupSuccess = true;
    
    if (!rtcManager.begin()) {
        Serial.println("No se pudo encontrar el RTC");
        setupSuccess = false;
    }

    if (!ioExpander.begin() || !powerManager.begin()) {
        Serial.println("Error al inicializar PCA9555 o PowerManager");
        setupSuccess = false;
    }

    // Inicializar sensor de flujo si está habilitado
    if (SensorEnable::FLOW) {
        if (!flowSensor.begin()) {
            Serial.println("Error al inicializar sensor de flujo");
            flowSensor.setEnabled(false);
            setupSuccess = false;
        }
    } else {
        flowSensor.setEnabled(false);
    }

    // Inicializar PT1000 si está habilitado
    if (SensorEnable::PT1000) {
        if (!pt1000.begin()) {
            Serial.println("Error al inicializar PT1000");
            pt1000.setEnabled(false);
            setupSuccess = false;
        }
    } else {
        pt1000.setEnabled(false);
    }

    // Inicializar SHT40 si está habilitado
    if (SensorEnable::SHT40) {
        if (!sht40.begin()) {
            Serial.println("No se pudo encontrar el sensor SHT40");
            sht40.setEnabled(false);
            setupSuccess = false;
        }
    } else {
        sht40.setEnabled(false);
    }
    
    // Inicializar DS18B20 si están habilitados
    if (SensorEnable::DS18B20) {
        if (!ds18b20.begin()) {
            Serial.println("No se pudieron encontrar sensores DS18B20");
            ds18b20.setEnabled(false);
            setupSuccess = false;
        } else {
            ds18b20.printAddresses();
        }
    } else {
        ds18b20.setEnabled(false);
    }

    if (rtcManager.hasLostPower()) {
        Serial.println("¡RTC perdió energía!");
        if (SHOULD_SYNC) {
            rtcManager.syncWithNTP();
        } else {
            rtcManager.setFallbackDateTime();
        }
    }

    if (!setupSuccess) {
        Serial.println("Errores críticos en la inicialización");
        goToSleep();
    }

    performMeasurements();
    
    if (!flowSensor.isEnabled()) {
        goToSleep();
    }
}

void loop() {
    // Verificar memoria periódicamente
    static unsigned long lastMemCheck = 0;
    if (millis() - lastMemCheck > 60000) { // Cada minuto
        uint32_t freeHeap = ESP.getFreeHeap();
        Serial.printf("Memoria libre: %u bytes\n", freeHeap);
        lastMemCheck = millis();
    }

    if (flowSensor.isEnabled() && flowSensor.isTimeToMeasure()) {
        if (flowSensor.readSensor()) {
            flowSensor.printMeasurements();
            performMeasurements();
        } else {
            Serial.println("Error en la lectura del sensor de flujo");
        }
    }
    delay(10);
}