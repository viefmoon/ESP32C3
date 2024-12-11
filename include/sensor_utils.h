#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <DallasTemperature.h>
#include "sensor_config.h"

namespace SensorUtils {
    // Convierte una dirección de sensor a string hexadecimal
    inline String addressToString(const DeviceAddress& addr) {
        String str;
        for (uint8_t i = 0; i < 8; i++) {
            if (addr[i] < 16) str += "0";
            str += String(addr[i], HEX);
        }
        return str;
    }

    // Busca la configuración de un DS18B20 por dirección
    inline const DS18B20Config* findDS18B20Config(const DeviceAddress& searchAddress) {
        for (const auto& config : DS18B20_CONFIGS) {
            if (memcmp(config.address, searchAddress, sizeof(DeviceAddress)) == 0) {
                return &config;
            }
        }
        return nullptr;
    }

    // Verifica si un valor de temperatura es válido
    inline bool isValidTemperature(float temp) {
        return !isnan(temp) && !isinf(temp) && temp > -55.0f && temp < 125.0f;
    }

    // Verifica si un valor de humedad es válido
    inline bool isValidHumidity(float humidity) {
        return !isnan(humidity) && !isinf(humidity) && humidity >= 0.0f && humidity <= 100.0f;
    }

    // Verifica si un valor de flujo es válido
    inline bool isValidFlow(float flow) {
        return !isnan(flow) && !isinf(flow) && flow >= 0.0f;
    }
}

#endif // SENSOR_UTILS_H 