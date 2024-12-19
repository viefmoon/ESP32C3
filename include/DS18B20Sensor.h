#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include "Measurement.h"
#include "config.h"
#include "sensor_config.h"
#include "sensor_utils.h"

struct DS18B20Device {
    DeviceAddress address;
    const DS18B20Config* config;
    Measurement measurement;
    bool isValid;
    
    DS18B20Device(const DeviceAddress& addr, const DS18B20Config* cfg) : 
        config(cfg),
        measurement(cfg ? cfg->id : "DS18B20_UNKNOWN",
                   cfg ? cfg->name : "Sensor Desconocido",
                   SensorType::TEMPERATURE),
        isValid(true) {
        memcpy(address, addr, sizeof(DeviceAddress));
    }
};

class DS18B20Sensor {
private:
    OneWire oneWire;
    DallasTemperature sensors;
    std::vector<DS18B20Device> devices;
    bool enabled;
    
    void searchDevices();
    
public:
    DS18B20Sensor();
    
    bool begin();
    bool readSensor();
    void printMeasurements();
    
    // Getters
    size_t getDeviceCount() const { return devices.size(); }
    const std::vector<DS18B20Device>& getDevices() const { return devices; }
    bool isEnabled() const { return enabled; }
    
    // Configuración
    void setEnabled(bool state) { enabled = state; }
    void setResolution(uint8_t resolution);
    void printAddresses();
    
    // Obtener medición específica
    const Measurement* getMeasurement(size_t index) const;
    const Measurement* getMeasurementByAddress(const DeviceAddress& address) const;
    
    // Nuevo método para escaneo
    void scanBus();
};

#endif 