#include <Arduino.h>
#include <WiFi.h>

#include "SPIFFS.h"

void initWifi();
void sendFile(String fn, WiFiClient c);

String ssid = "The Mill";
String password = "blackpowder";
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
    // Serial Monitor
    Serial.begin(9600);

    SPIFFS.begin();

    // Setup wifi access point
    initWifi();
}

void loop() {
    // put your main code here, to run repeatedly:
    WiFiClient client = server.available();

    if (client) {
        Serial.print("Client connected: ");
        Serial.println(client.localIP());

        sendFile("/index.html", client);
        sleep(100);

        // while (client.connected()) {
        //     if (client.available()) {

        //         sendFile("/index.html", client);
        //     }
        // }
    }

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println();
    sleep(10);
}

void initWifi() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());

    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address:");
    Serial.print(ip);

    server.begin();
}

void sendHTTPHeader(WiFiClient client, String contentType) {
    client.println("HTTP/1.1 200 OK");
    client.println(contentType);
    client.println("Connection: keep-alive");
    client.println();
}

void sendFile(String filename, WiFiClient client) {
    File file = SPIFFS.open(filename);

    // 1. Check file exists
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    if (strstr(filename.c_str(), ".html")) {
        sendHTTPHeader(client, "Content-type:text/html");
    }

    // 2. Read and send file
    while (file.available()) {
        client.println(file.readString());
    }

    // 3. Close handler
    file.close();
    client.println();
}

// String getClientData(WiFiClient* client) {
// }