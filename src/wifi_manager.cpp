#include "wifi_manager.h"
#include <WiFi.h>
#include <time.h>

bool Wifi_Manager::Connect(const char* ssid, const char* password)
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
        syncTime();   // sincroniza hora después de conectar
        return true;
    }

    Serial.println("\nWiFi connection failed.");
    return false;
}

bool Wifi_Manager::IsConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void Wifi_Manager::syncTime()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Synchronizing time");

    time_t now = time(nullptr);

    while (now < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    Serial.println("\nTime synchronized");
}