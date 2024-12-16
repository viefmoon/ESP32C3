#ifndef CONFIG_MODE_H
#define CONFIG_MODE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "clsPCA9555.h"
#include "sensor_config.h"

// UUIDs para el servicio y características
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define SENSOR_ENABLE_UUID  "4fafc202-1fb5-459e-8fcc-c5c9c331914b"
#define FLOW_CONFIG_UUID    "4fafc203-1fb5-459e-8fcc-c5c9c331914b"
#define DS18B20_CONFIG_UUID "4fafc204-1fb5-459e-8fcc-c5c9c331914b"

class ConfigMode : public BLEServerCallbacks, public BLECharacteristicCallbacks {
private:
    static const uint8_t CONFIG_PIN = 9;         // Pin para entrar en modo config
    static const uint8_t LED_PIN = P13;          // LED en IO1_3 del PCA9555
    static const uint32_t TRIGGER_TIME = 5000;   // 5 segundos para activar
    static const uint32_t TIMEOUT = 30000;       // 30 segundos de timeout
    static const uint32_t LED_BLINK_INTERVAL = 500; // 500ms intervalo de parpadeo

    PCA9555& ioExpander;
    bool isInConfigMode;
    unsigned long configStartTime;
    unsigned long lastLedToggle;
    bool ledState;
    BLEServer* pServer;
    bool deviceConnected;

    // Características BLE
    BLECharacteristic* pSensorEnableCharacteristic;
    BLECharacteristic* pFlowConfigCharacteristic;
    BLECharacteristic* pDS18B20ConfigCharacteristic;

    void startBLE();
    void stopBLE();
    void toggleLed();
    void setupBLECharacteristics(BLEService* pService);
    void handleSensorEnableConfig(const uint8_t* data, size_t length);
    void handleFlowConfig(const uint8_t* data, size_t length);
    void handleDS18B20Config(const uint8_t* data, size_t length);

public:
    ConfigMode(PCA9555& expander);
    
    void begin();
    void check();
    bool isActive() const { return isInConfigMode; }
    
    // Callbacks de BLE
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    void onWrite(BLECharacteristic* pCharacteristic) override;
};

#endif 