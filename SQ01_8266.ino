#include <ESP8266WiFi.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"

int8_t volume_value = -79;
int8_t subwoofer_value = -79; 

#define led 12
#define mod 14

AsyncWebServer server(80);
AsyncWebSocket socket("/ws");

WiFiEventHandler wifi_disconnected_handler;

void setup(void)
{
    Serial.begin(115200);
    pinMode(led, OUTPUT);
    pinMode(mod, INPUT);
    
    EEPROM.begin(50);
    LittleFS.begin();
    Wire.begin();

    delay(500);
    digitalWrite(led,HIGH);

    create_path_handler();
    setup_processor();

    if (digitalRead(mod) == LOW)
    {
        String ssid = read_string(15);
        String pass = read_string(35);

        wifi_disconnected_handler = WiFi.onStationModeDisconnected(wifi_disconnected);
    
        WiFi.mode(WIFI_STA);
        WiFi.hostname("SQ01 Processor");
        WiFi.begin(ssid.c_str(), pass.c_str());
        
        while (WiFi.status() != WL_CONNECTED)
        {
            digitalWrite(led, LOW);
            delay(250);
            digitalWrite(led, HIGH);
            delay(250);
        }
    }
    else
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("SQ01 Processor", "SQ01@DIYDSP");
    }

    socket.onEvent(socket_event);
    server.addHandler(&socket);
    server.begin();
}

void wifi_disconnected(const WiFiEventStationModeDisconnected &event)
{
    digitalWrite(led,!digitalRead(led));

    String ssid = read_string(15);
    String pass = read_string(35);

    WiFi.disconnect();
    WiFi.begin(ssid.c_str(), pass.c_str());
}

void loop(void) 
{ 
    socket.cleanupClients();
    delay(100);
}

