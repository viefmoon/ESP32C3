#include "ConfigMode.h"
#include <EEPROM.h>

// Dirección en EEPROM para guardar configuración
#define CONFIG_ADDR_SENSOR_ENABLE 0
#define CONFIG_ADDR_FLOW         10
#define CONFIG_ADDR_DS18B20      20

ConfigMode::ConfigMode(PCA9555& expander) : 
    ioExpander(expander),
    isInConfigMode(false),
    configStartTime(0),
    lastLedToggle(0),
    ledState(false),
    pServer(nullptr),
    deviceConnected(false),
    pSensorEnableCharacteristic(nullptr),
    pFlowConfigCharacteristic(nullptr),
    pDS18B20ConfigCharacteristic(nullptr) {
}

void ConfigMode::begin() {
    pinMode(CONFIG_PIN, INPUT_PULLUP);
    ioExpander.pinMode(LED_PIN, OUTPUT);
    ioExpander.digitalWrite(LED_PIN, LOW);
    EEPROM.begin(512);  // Inicializar EEPROM con 512 bytes
}

void ConfigMode::setupBLECharacteristics(BLEService* pService) {
    // Característica para habilitar/deshabilitar sensores
    pSensorEnableCharacteristic = pService->createCharacteristic(
        SENSOR_ENABLE_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pSensorEnableCharacteristic->setCallbacks(this);

    // Característica para configuración del sensor de flujo
    pFlowConfigCharacteristic = pService->createCharacteristic(
        FLOW_CONFIG_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pFlowConfigCharacteristic->setCallbacks(this);

    // Característica para configuración de DS18B20
    pDS18B20ConfigCharacteristic = pService->createCharacteristic(
        DS18B20_CONFIG_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pDS18B20ConfigCharacteristic->setCallbacks(this);
}

void ConfigMode::startBLE() {
    BLEDevice::init("ESP32-Config");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);
    
    BLEService *pService = pServer->createService(SERVICE_UUID);
    setupBLECharacteristics(pService);
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    BLEDevice::startAdvertising();
    
    Serial.println("Modo configuración BLE iniciado");
}

void ConfigMode::handleSensorEnableConfig(const uint8_t* data, size_t length) {
    if (length != 4) return;  // Esperamos 4 bytes (uno por sensor)
    
    // Guardar en EEPROM
    EEPROM.write(CONFIG_ADDR_SENSOR_ENABLE, data[0]);     // DS18B20
    EEPROM.write(CONFIG_ADDR_SENSOR_ENABLE + 1, data[1]); // SHT40
    EEPROM.write(CONFIG_ADDR_SENSOR_ENABLE + 2, data[2]); // PT1000
    EEPROM.write(CONFIG_ADDR_SENSOR_ENABLE + 3, data[3]); // Flow
    EEPROM.commit();
    
    Serial.println("Configuración de sensores actualizada");
}

void ConfigMode::handleFlowConfig(const uint8_t* data, size_t length) {
    if (length != sizeof(float)) return;  // Esperamos 4 bytes (float)
    
    float mlPerPulse;
    memcpy(&mlPerPulse, data, sizeof(float));
    
    // Guardar en EEPROM
    EEPROM.writeFloat(CONFIG_ADDR_FLOW, mlPerPulse);
    EEPROM.commit();
    
    Serial.printf("Configuración de flujo actualizada: %.2f mL/pulso\n", mlPerPulse);
}

void ConfigMode::handleDS18B20Config(const uint8_t* data, size_t length) {
    if (length != sizeof(DeviceAddress)) return;
    
    // Guardar en EEPROM
    for (size_t i = 0; i < sizeof(DeviceAddress); i++) {
        EEPROM.write(CONFIG_ADDR_DS18B20 + i, data[i]);
    }
    EEPROM.commit();
    
    Serial.println("Dirección DS18B20 actualizada");
}

void ConfigMode::onWrite(BLECharacteristic* pCharacteristic) {
    std::string uuid = pCharacteristic->getUUID().toString();
    std::string value = pCharacteristic->getValue();
    
    if (uuid == SENSOR_ENABLE_UUID) {
        handleSensorEnableConfig((uint8_t*)value.data(), value.length());
    }
    else if (uuid == FLOW_CONFIG_UUID) {
        handleFlowConfig((uint8_t*)value.data(), value.length());
    }
    else if (uuid == DS18B20_CONFIG_UUID) {
        handleDS18B20Config((uint8_t*)value.data(), value.length());
    }
}

void ConfigMode::toggleLed() {
    ledState = !ledState;
    ioExpander.digitalWrite(LED_PIN, ledState);
}

void ConfigMode::stopBLE() {
    if (pServer) {
        BLEDevice::deinit(true);
        pServer = nullptr;
    }
    isInConfigMode = false;
    ioExpander.digitalWrite(LED_PIN, LOW);
    Serial.println("Modo configuración BLE terminado");
}

void ConfigMode::check() {
    static bool lastPinState = true;
    static unsigned long pressStartTime = 0;
    bool currentPinState = digitalRead(CONFIG_PIN);
    unsigned long currentTime = millis();

    // Detectar entrada en modo configuración
    if (!isInConfigMode) {
        if (currentPinState == LOW) {
            if (lastPinState == HIGH) {
                pressStartTime = currentTime;
            } else if (currentTime - pressStartTime >= TRIGGER_TIME) {
                isInConfigMode = true;
                configStartTime = currentTime;
                startBLE();
            }
        }
    }
    // Manejar modo configuración
    else {
        // Parpadear LED
        if (currentTime - lastLedToggle >= LED_BLINK_INTERVAL) {
            toggleLed();
            lastLedToggle = currentTime;
        }

        // Verificar timeout si no hay dispositivo conectado
        if (!deviceConnected && currentTime - configStartTime >= TIMEOUT) {
            stopBLE();
        }
    }

    lastPinState = currentPinState;
}

void ConfigMode::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Dispositivo BLE conectado");
}

void ConfigMode::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Dispositivo BLE desconectado");
    stopBLE();
} 