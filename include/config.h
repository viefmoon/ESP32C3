#ifndef CONFIG_H
#define CONFIG_H

// Configuración de pines
#define ONE_WIRE_BUS     2     // GPIO2 para OneWire
#define I2C_SDA_PIN 1
#define I2C_SCL_PIN 0

// Configuración WiFi
#define WIFI_SSID       "INFINITUM0439"
#define WIFI_PASSWORD   "3XhdAUXEuP"

// Configuración NTP
#define NTP_SERVER      "pool.ntp.org"
#define GMT_OFFSET      -18000     // GMT-5 para México (ajusta según tu zona horaria)
#define DAYLIGHT_OFFSET 0          // Ajuste para horario de verano

// Otras configuraciones
#define SERIAL_BAUD     115200       // Velocidad del puerto serial
#define SYNC_INTERVAL   3600000    // Intervalo de sincronización en milisegundos (1 hora)

// Configuración Deep Sleep
#define uS_TO_S_FACTOR  1000000ULL // Factor de conversión de microsegundos a segundos
#define TIME_TO_SLEEP   300        // Tiempo de sueño en segundos (5 minutos = 300 segundos)

// Activación de sensores
#define ENABLE_SHT40    false       // Activar/desactivar sensor SHT40
#define ENABLE_DS18B20  false       // Activar/desactivar sensores DS18B20
#define MAX_DS18B20     4          // Número máximo de sensores DS18B20 a buscar

// Banderas de control
#define SHOULD_SYNC     true       // Determina si se debe sincronizar con NTP

// Definición de IDs base para sensores
#define SENSOR_SHT40_TEMP_ID    "SHT40_TEMP_01"
#define SENSOR_SHT40_HUM_ID     "SHT40_HUM_01"
#define SENSOR_RTC_ID           "RTC_01"
#define SENSOR_DS18B20_BASE_ID  "DS18B20_"     // Se añadirá número al final

// Nombres descriptivos de sensores
#define SENSOR_SHT40_TEMP_NAME  "Sensor de Temperatura SHT40"
#define SENSOR_SHT40_HUM_NAME   "Sensor de Humedad SHT40"
#define SENSOR_RTC_NAME         "Reloj RTC DS3231"
#define SENSOR_DS18B20_NAME     "Sensor de Temperatura DS18B20 #"  // Se añadirá número al final

// Unidades de medida
#define UNIT_CELSIUS           "°C"
#define UNIT_HUMIDITY          "%RH"
#define UNIT_TIMESTAMP         "UTC"

// Definiciones de pines SPI
#define SPI_SCK_PIN     10    // Pin de reloj SPI
#define SPI_MISO_PIN    6     // Pin MISO (Master In Slave Out)
#define SPI_MOSI_PIN    7     // Pin MOSI (Master Out Slave In)
#define SPI_CS_PIN      -1    // Pin CS (Chip Select) - No usado globalmente

#endif // CONFIG_H 