#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <Arduino.h>
#include <RTClib.h>
#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>

class RTCManager {
private:
    RTC_DS3231 rtc;
    const char* wifiSSID;
    const char* wifiPassword;
    const char* ntpServer;
    const long gmtOffset;
    const int daylightOffset;
    
    bool connectToWiFi();
    void disconnectWiFi();

public:
    RTCManager(const char* ssid, const char* password, 
               const char* ntp = "pool.ntp.org",
               long gmtOffsetSec = -18000,
               int daylightOffsetSec = 0);
    
    bool begin();
    bool syncWithNTP();
    void setFallbackDateTime();
    DateTime getCurrentTime();
    void printDateTime();
    bool hasLostPower();
};

#endif 