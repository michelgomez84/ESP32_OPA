#include "wifi_manager.h"
#include <WiFi.h>

bool wifiConnect(const char* ssid, const char* password)
{
    WiFi.begin(ssid, password);

    Serial.println("Connecting WiFi...");

    int retries = 0;

    while (WiFi.status() != WL_CONNECTED && retries < 20)
    {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi Connected.");
        Serial.println(WiFi.localIP());
        return true;
    }

    Serial.println("\nWiFi connection failed.");
    return false;
}

bool wifiIsConnected()
{
    return WiFi.status() == WL_CONNECTED;
}