#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include <mutex>

#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "SPIFFS.h"
#include "motor_state.h"

// Method Definitions
void updateTime();

// Credentials
const char* ssid = "The Mill";
const char* password = "finepowder";

// WiFi Setup
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 255);
IPAddress subnet(255, 255, 255, 0);
const char* hostname = "mill";

// Motor
Motor::motor motor;

// Running time (s)
volatile unsigned long int runTime;

// Async Webserver
AsyncWebServer server(80);

// HW Timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    motor.begin();

    // 0. Setup timer
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &updateTime, true);
    timerAlarmWrite(timer, 1e6, true);
    timerAlarmEnable(timer);

    // 1. Setup wifi
    Serial.println(WiFi.mode(WIFI_AP) ? "Success: Set mode to AP" : "Failure: Could not set AP Mode");
    Serial.println(WiFi.softAP(ssid, password) ? "Success: Wifi Init" : "Failure: Wifi init failed");
    delay(2000);
    Serial.println(WiFi.softAPConfig(local_ip, gateway, subnet) ? "Success: Wifi Config set" : "Failure: Wifi Config not set");
    Serial.println(WiFi.softAPsetHostname(hostname) ? "Success: Hostname set" : "Failure: Hostname not set");

    Serial.print("Local IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("Hostname: ");
    Serial.println(WiFi.softAPgetHostname());

    // 2. Setup DNS discovery
    Serial.println(MDNS.begin(hostname) ? "Success: DNS hostname set" : "Failure: DNS discovery won't work");

    // 3. Routes for discovery
    AsyncCallbackJsonWebHandler* motorControlHandler = new AsyncCallbackJsonWebHandler("/motor-control", [](AsyncWebServerRequest* request, JsonVariant& json) {
        // 0. Convert data into json object
        StaticJsonDocument<1024> data;
        if (json.is<JsonArray>()) {
            data = json.as<JsonArray>();
        } else if (json.is<JsonObject>()) {
            data = json.as<JsonObject>();
        }

        // 1. Set the new state
        Motor::states requestedState = Motor::motor::getStateFromString(data["mode"].as<const char*>());
        motor.run(requestedState);

        // 2. Return the new state
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        const size_t capacity = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> doc;
        JsonObject responsePayload = doc.to<JsonObject>();
        responsePayload["status"] = "success";
        responsePayload["motorMode"] = Motor::motor::getStringFromState(requestedState);
        serializeJson(responsePayload, *response);
        request->send(response);
    });

    server.addHandler(motorControlHandler);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/style.css", "text/css");
    });

    server.on("/actions.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/actions.js", "text/javascript");
    });

    server.on("/boardstate.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/boardstate.js", "text/javascript");
    });

    server.on("/timer.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/timer.js", "text/javascript");
    });

    server.on("/state", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        const size_t capacity = JSON_OBJECT_SIZE(3);
        StaticJsonDocument<capacity> doc;
        JsonObject responsePayload = doc.to<JsonObject>();
        responsePayload["status"] = "success";
        responsePayload["motorMode"] = Motor::motor::getStringFromState(motor.getMotorState());
        portENTER_CRITICAL_ISR(&timerMux);
        responsePayload["runTime"] = runTime;
        portEXIT_CRITICAL_ISR(&timerMux);
        serializeJson(responsePayload, *response);
        request->send(response);
    });

    server.begin();
}

void IRAM_ATTR updateTime() {
    portENTER_CRITICAL_ISR(&timerMux);
    if (!motor.isOn()) {
        runTime = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        return;
    }
    runTime++;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void loop() {
}