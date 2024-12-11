#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "clsPCA9555.h"

class PowerManager {
private:
    PCA9555& ioExpander;
    static const uint8_t POWER_3V3_PIN = P00;  // Fuente de 3.3V para sensores en IO0_0
    static const uint8_t POWER_12V_PIN = P01;  // Fuente de 12V en IO0_1
    static const uint16_t POWER_STABILIZE_DELAY = 100; // ms

public:
    PowerManager(PCA9555& expander);
    bool begin();

    void power3V3On();
    void power3V3Off();
    void power12VOn();
    void power12VOff();
    void allPowerOff();
};

#endif 