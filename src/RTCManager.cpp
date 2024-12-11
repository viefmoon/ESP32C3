#include "RTCManager.h"

RTCManager::RTCManager(const char* ssid, const char* password, 
                     const char* ntp, long gmtOffsetSec, 
                     int daylightOffsetSec) : 
    wifiSSID(ssid),
    wifiPassword(password),
    ntpServer(ntp),
    gmtOffset(gmtOffsetSec),
    daylightOffset(daylightOffsetSec) {
}

bool RTCManager::begin() {
    return rtc.begin();
}

bool RTCManager::connectToWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(wifiSSID, wifiPassword);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado");
        return true;
    }
    Serial.println("\nError de conexión WiFi");
    return false;
}

void RTCManager::disconnectWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

bool RTCManager::syncWithNTP() {
    if (!connectToWiFi()) return false;

    configTime(gmtOffset, daylightOffset, ntpServer);
    
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)) {
        Serial.println("Error al obtener la hora de Internet");
        disconnectWiFi();
        return false;
    }
    
    rtc.adjust(DateTime(
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec
    ));
    
    Serial.println("RTC sincronizado con éxito");
    disconnectWiFi();
    return true;
}

void RTCManager::setFallbackDateTime() {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC configurado con hora de compilación");
}

DateTime RTCManager::getCurrentTime() {
    return rtc.now();
}

void RTCManager::printDateTime() {
    DateTime currentTime = getCurrentTime();
    
    Serial.print(currentTime.year(), DEC);
    Serial.print('/');
    Serial.print(currentTime.month(), DEC);
    Serial.print('/');
    Serial.print(currentTime.day(), DEC);
    Serial.print(" ");
    Serial.print(currentTime.hour(), DEC);
    Serial.print(':');
    Serial.print(currentTime.minute(), DEC);
    Serial.print(':');
    Serial.println(currentTime.second(), DEC);
}

bool RTCManager::hasLostPower() {
    return rtc.lostPower();
} 