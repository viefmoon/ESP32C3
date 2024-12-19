#include "DS18B20Sensor.h"

DS18B20Sensor::DS18B20Sensor() : 
    oneWire(ONE_WIRE_BUS),
    sensors(&oneWire),
    enabled(ENABLE_DS18B20) {
}

void DS18B20Sensor::searchDevices() {
    devices.clear();
    sensors.begin();
    
    // Buscar solo los sensores configurados
    for (const auto& config : DS18B20_CONFIGS) {
        if (sensors.isConnected(config.address)) {
            devices.emplace_back(config.address, &config);
            Serial.print("Sensor encontrado - ID: ");
            Serial.print(config.id);
            Serial.print(" - Nombre: ");
            Serial.println(config.name);
        } else {
            Serial.print("Sensor no conectado - ID: ");
            Serial.println(config.id);
        }
    }
}

bool DS18B20Sensor::begin() {
    if (!enabled) {
        Serial.println("Sensores DS18B20 desactivados");
        return true;
    }
    
    searchDevices();
    
    if (devices.empty()) {
        Serial.println("No se encontraron sensores DS18B20");
        return false;
    }
    
    setResolution(12);
    return true;
}

bool DS18B20Sensor::readSensor() {
    if (!enabled || devices.empty()) {
        return false;
    }
    
    sensors.requestTemperatures();
    unsigned long currentTime = millis();
    bool anyValidReading = false;
    
    for (auto& device : devices) {
        float tempC = sensors.getTempC(device.address);
        
        if (tempC != DEVICE_DISCONNECTED_C) {
            device.measurement = Measurement(
                device.config ? device.config->id : "DS18B20_UNKNOWN",
                device.config ? device.config->name : "Sensor Desconocido",
                SensorType::TEMPERATURE
            );
            device.measurement.setTimestamp(currentTime);
            device.measurement.addVariable("Temperatura", tempC, UNIT_CELSIUS);
            device.isValid = true;
            anyValidReading = true;
        } else {
            device.isValid = false;
            Serial.print("Error al leer el sensor ");
            Serial.println(device.config ? device.config->id : "desconocido");
        }
    }
    
    return anyValidReading;
}

void DS18B20Sensor::printMeasurements() {
    if (!enabled) {
        Serial.println("Sensores DS18B20 desactivados");
        return;
    }
    
    for (const auto& device : devices) {
        if (device.isValid) {
            device.measurement.print();
        }
    }
}

void DS18B20Sensor::setResolution(uint8_t resolution) {
    if (!enabled) return;
    
    if (resolution < 9) resolution = 9;
    if (resolution > 12) resolution = 12;
    
    for (const auto& device : devices) {
        sensors.setResolution(device.address, resolution);
    }
}

void DS18B20Sensor::printAddresses() {
    if (!enabled) {
        Serial.println("Sensores DS18B20 desactivados");
        return;
    }
    
    for (const auto& device : devices) {
        Serial.print("Sensor ");
        Serial.print(device.config ? device.config->id : "Desconocido");
        Serial.print(" (");
        Serial.print(device.config ? device.config->name : "Sin nombre");
        Serial.print(") - Dirección: ");
        
        for (uint8_t i = 0; i < 8; i++) {
            if (device.address[i] < 16) Serial.print("0");
            Serial.print(device.address[i], HEX);
        }
        Serial.println();
    }
}

const Measurement* DS18B20Sensor::getMeasurement(size_t index) const {
    if (!enabled || index >= devices.size() || !devices[index].isValid) {
        return nullptr;
    }
    return &devices[index].measurement;
}

const Measurement* DS18B20Sensor::getMeasurementByAddress(const DeviceAddress& address) const {
    if (!enabled) return nullptr;
    
    for (const auto& device : devices) {
        if (memcmp(device.address, address, sizeof(DeviceAddress)) == 0 && device.isValid) {
            return &device.measurement;
        }
    }
    return nullptr;
}

void DS18B20Sensor::scanBus() {
    Serial.println("\n=== Escaneando bus OneWire para sensores DS18B20 ===");
    
    sensors.begin();
    DeviceAddress tempAddress;
    uint8_t deviceCount = sensors.getDeviceCount();
    
    if (deviceCount == 0) {
        Serial.println("No se encontraron dispositivos en el bus");
        return;
    }
    
    Serial.print("Dispositivos encontrados: ");
    Serial.println(deviceCount);
    Serial.println("\nDirecciones encontradas:");
    
    for (uint8_t i = 0; i < deviceCount; i++) {
        if (sensors.getAddress(tempAddress, i)) {
            Serial.print(i);
            Serial.print(") ");
            // Imprime la dirección en formato hexadecimal
            for (uint8_t j = 0; j < 8; j++) {
                if (tempAddress[j] < 16) Serial.print("0");
                Serial.print(tempAddress[j], HEX);
            }
            
            // Imprime la temperatura actual del sensor
            float tempC = sensors.getTempC(tempAddress);
            Serial.print(" - Temperatura actual: ");
            Serial.print(tempC);
            Serial.println("°C");
        }
    }
    Serial.println("\nCopia estas direcciones para usarlas en tu configuración");
    Serial.println("=== Fin del escaneo ===\n");
} 