#ifndef motor_state_h
#define motor_state_h

#include <Arduino.h>
#include <string.h>
#define MOTOR_ON "on"
#define MOTOR_OFF "off"
#define MOTOR_PIN GPIO_NUM_4
#define PWM_CHANNEL 0
#define PWM_FREQ 10000
#define PWM_RESOLUTION 8

namespace Motor {
enum states { on,
              off };

class motor {
   public:
    states motorState;
    int speed;

    motor() {
    }

    void begin() {
        motorState = states::off;
        speed = 0;
        pinMode(MOTOR_PIN, OUTPUT);
        ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
        ledcAttachPin(MOTOR_PIN, PWM_CHANNEL);
        ledcWrite(PWM_CHANNEL, 0);

        Serial.println("Status: Motor is ready");
    }

    void turnOn() {
        setMotorState(states::on);
        Serial.println("Status: Motor on");
    }

    void turnOff() {
        setMotorState(states::off);
        setSpeed(0);
        Serial.println("Status: Motor off");
    }

    void setSpeed(int speed) {
        if (this->isOn()) {
            ledcWrite(PWM_CHANNEL, speed);
            this->speed = speed;
            return;
        }
        ledcWrite(PWM_CHANNEL, 0);
    }

    int getSpeed() {
        return speed;
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