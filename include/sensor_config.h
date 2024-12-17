#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#include <DallasTemperature.h>

// Constantes comunes para sensores
namespace SensorConstants {
    constexpr uint16_t DEFAULT_MEASURE_INTERVAL = 2000;  // ms
    constexpr uint8_t MAX_DS18B20_SENSORS = 4;
    constexpr float MIN_VALID_TEMP = -55.0f;
    constexpr float MAX_VALID_TEMP = 125.0f;
    constexpr float MIN_VALID_HUMIDITY = 0.0f;
    constexpr float MAX_VALID_HUMIDITY = 100.0f;
}

// Estructuras de configuración para cada tipo de sensor
struct DS18B20Config {
    const char* id;
    const char* name;
    DeviceAddress address;
};

struct SHT40Config {
    struct {
        const char* id;
        const char* name;
    } temperature;
    struct {
        const char* id;
        const char* name;
    } humidity;
};

struct PT100_Config {
    const char* id;
    const char* name;
    uint16_t rRef;      // Resistencia de referencia
    uint16_t rNominal;  // Resistencia nominal a 0°C
};

struct FlowConfig {
    const char* id;
    const char* name;
    float mlPerPulse;
    uint16_t measureInterval;  // ms
};

// Habilitación de sensores
namespace SensorEnable {
    constexpr bool DS18B20 = true;
    constexpr bool SHT40 = true;
    constexpr bool PT100 = true;
    constexpr bool FLOW = false;
}

// Configuración de los sensores DS18B20
const DS18B20Config DS18B20_CONFIGS[] = {
    {
        "DS18B20_01",
        "Sensor Temperatura 1",
        {0x28, 0xFF, 0x64, 0x1E, 0x15, 0x21, 0x4B, 0x7C}
    },
    {
        "DS18B20_02",
        "Sensor Temperatura 2",
        {0x28, 0xFF, 0x64, 0x1E, 0x15, 0x21, 0x4B, 0x7D}
    }
};

// Configuración del sensor SHT40
const SHT40Config SHT40_CONFIG = {
    {
        "SHT40_TEMP",
        "Sensor SHT40 Temperatura"
    },
    {
        "SHT40_HUM",
        "Sensor SHT40 Humedad"
    }
};

// Configuración del sensor PT100
const PT100_Config PT100_CONFIG = {
    "PT100_01",
    "Sensor de Temperatura PT100",
    430,   // Resistencia de referencia
    100     // Resistencia nominal
};

// Configuración del sensor de flujo
const FlowConfig FLOW_CONFIG = {
    "FLOW_01",
    "Sensor de Flujo",
    1.0f,    // 1.0 mL por pulso
    2000     // Intervalo de medición en ms
};

#endif // SENSOR_CONFIG_H 