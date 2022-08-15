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
volatile unsigned long int runTime = 0;

// Async Webserver
AsyncWebServer server(80);

// Websockets Setup
AsyncWebSocket ws("/ws");

// HW Timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Web socket messages
void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        const uint8_t size = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        const int speed = json["speed"];
        motor.setSpeed(speed);
    }
}

void onEvent(AsyncWebSocket* server,
             AsyncWebSocketClient* client,
             AwsEventType type,
             void* arg,
             uint8_t* data,
             size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

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

    AsyncCallbackJsonWebHandler* speedControlHandler = new AsyncCallbackJsonWebHandler("/speed-control", [](AsyncWebServerRequest* request, JsonVariant& json) {
        // 0. Convert data into json object
        StaticJsonDocument<1024> data;
        if (json.is<JsonArray>()) {
            data = json.as<JsonArray>();
        } else if (json.is<JsonObject>()) {
            data = json.as<JsonObject>();
        }

        // 1. Set the new state
        int speed = data["speed"].as<int>();
        motor.setSpeed(speed);

        // 2. Return the new state
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        const size_t capacity = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<capacity> doc;
        JsonObject responsePayload = doc.to<JsonObject>();
        responsePayload["status"] = "success";
        serializeJson(responsePayload, *response);
        request->send(response);
    });

    server.addHandler(speedControlHandler);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/style.css", "text/css");
    });

    server.on("/motor.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/motor.js", "text/javascript");
    });

    server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/main.js", "text/javascript");
    });

    server.on("/actions.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/actions.js", "text/javascript");
    });

    server.on("/component.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/component.js", "text/javascript");
    });

    server.on("/info.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/info.js", "text/javascript");
    });

    server.on("/motor_control_button.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/motor_control_button.js", "text/javascript");
    });

    server.on("/connection_label.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/connection_label.js", "text/javascript");
    });

    server.on("/motor_status_label.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/motor_status_label.js", "text/javascript");
    });

    server.on("/spinner.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/spinner.js", "text/javascript");
    });

    server.on("/requests.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/requests.js", "text/javascript");
    });

    server.on("/timer.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/timer.js", "text/javascript");
    });

    server.on("/slider.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/slider.js", "text/javascript");
    });

    server.on("/state", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        const size_t capacity = JSON_OBJECT_SIZE(4);
        StaticJsonDocument<capacity> doc;
        JsonObject responsePayload = doc.to<JsonObject>();
        responsePayload["status"] = "success";
        responsePayload["motorMode"] = Motor::motor::getStringFromState(motor.getMotorState());
        responsePayload["speed"] = motor.getSpeed();
        portENTER_CRITICAL_ISR(&timerMux);
        responsePayload["runTime"] = runTime;
        portEXIT_CRITICAL_ISR(&timerMux);
        serializeJson(responsePayload, *response);
        request->send(response);
    });

    // Attach Websockets to server
    ws.onEvent(onEvent);
    server.addHandler(&ws);

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

// Loop timer
unsigned long prevTimes = millis();
bool timePassed(int timems) {
    unsigned int currentTime = millis();
    if ((currentTime - prevTimes) > timems) {
        prevTimes = millis();
        return true;
    }
    return false;
}

void loop() {
    // Clean up websockets
    if (timePassed(1000)) {
        ws.cleanupClients();
    }
}