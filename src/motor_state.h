#ifndef motor_state_h
#define motor_state_h

#include <Arduino.h>
#include <string.h>
#define MOTOR_ON "on"
#define MOTOR_OFF "off"
#define MOTOR_PIN GPIO_NUM_33

namespace Motor {
enum states { on,
              off };

class motor {
   public:
    states motorState;

    motor() {}

    void begin() {
        motorState = states::off;
        pinMode(MOTOR_PIN, OUTPUT);
        digitalWrite(MOTOR_PIN, LOW);
        Serial.println("Status: Motor is ready");
    }

    void turnOn() {
        setMotorState(states::on);
        digitalWrite(MOTOR_PIN, HIGH);
        Serial.println("Status: Motor on");
    }

    void turnOff() {
        setMotorState(states::off);
        digitalWrite(MOTOR_PIN, LOW);
        Serial.println("Status: Motor off");
    }

    void run(states state) {
        if (state == states::on) {
            turnOn();
        }
        if (state == states::off) {
            turnOff();
        }
    }

    void setMotorState(states newState) {
        motorState = newState;
    }

    states getMotorState() {
        return motorState;
    }

    bool isOn() {
        if (motorState == states::on) {
            return true;
        }
        return false;
    }

    static states getStateFromString(const char* mode) {
        if (!strcmp(mode, MOTOR_ON))
            return states::on;
        else {
            return states::off;
        }
    }

    static const char* getStringFromState(states mode) {
        if (mode == states::on)
            return MOTOR_ON;
        else {
            return MOTOR_OFF;
        }
    }

    static states flipState(states mode) {
        int state = mode ^ mode;
        return static_cast<states>(state);
    }

    static const char* flipState(const char* mode) {
        states s = getStateFromString(mode);
        states flippedMode = flipState(s);
        return getStringFromState(flippedMode);
    }
};
}  // namespace Motor

#endif