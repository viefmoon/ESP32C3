#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <Arduino.h>
#include <string>
#include <vector>

// Enumeración para los tipos de sensores
enum class SensorType {
    TEMPERATURE,
    HUMIDITY,
    TIMESTAMP,
    PRESSURE,
    LIGHT,
    FLOW,
    UNKNOWN
};

// Estructura para una variable medida
struct MeasuredVariable {
    String name;
    float value;
    String unit;
    
    MeasuredVariable(const String& n, float v, const String& u) 
        : name(n), value(v), unit(u) {}
};

// Clase principal para mediciones
class Measurement {
private:
    String sensorId;
    String sensorName;
    SensorType type;
    unsigned long timestamp;
    std::vector<MeasuredVariable> variables;
    static const size_t MAX_VARIABLES = 5;

public:
    Measurement(const String& id, const String& name, SensorType t)
        : sensorId(id.length() > 0 ? id : "UNKNOWN"),
          sensorName(name.length() > 0 ? name : "Sensor Desconocido"),
          type(t),
          timestamp(0) {
        variables.reserve(MAX_VARIABLES);
    }
    
    // Constructor de copia
    Measurement(const Measurement& other)
        : sensorId(other.sensorId),
          sensorName(other.sensorName),
          type(other.type),
          timestamp(other.timestamp),
          variables(other.variables) {
    }
    
    // Operador de asignación
    Measurement& operator=(const Measurement& other) {
        if (this != &other) {
            sensorId = other.sensorId;
            sensorName = other.sensorName;
            type = other.type;
            timestamp = other.timestamp;
            variables = other.variables;
        }
        return *this;
    }
    
    // Getters
    String getSensorId() const { return sensorId; }
    String getSensorName() const { return sensorName; }
    SensorType getType() const { return type; }
    unsigned long getTimestamp() const { return timestamp; }
    const std::vector<MeasuredVariable>& getVariables() const { return variables; }
    
    // Setters y métodos de utilidad
    void setTimestamp(unsigned long ts) { timestamp = ts; }
    
    bool addVariable(const String& name, float value, const String& unit) {
        if (variables.size() >= MAX_VARIABLES) {
            Serial.println("Error: Número máximo de variables alcanzado");
            return false;
        }
        if (name.length() == 0 || unit.length() == 0) {
            Serial.println("Error: Nombre o unidad inválidos");
            return false;
        }
        variables.push_back(MeasuredVariable(name, value, unit));
        return true;
    }
    
    // Método para imprimir la medición
    void print() const {
        Serial.print("Sensor ID: ");
        Serial.println(sensorId);
        Serial.print("Nombre: ");
        Serial.println(sensorName);
        Serial.print("Timestamp: ");
        Serial.println(timestamp);
        
        for (const auto& var : variables) {
            Serial.print(var.name);
            Serial.print(": ");
            Serial.print(var.value);
            Serial.print(" ");
            Serial.println(var.unit);
        }
        Serial.println();
    }
};

#endif 