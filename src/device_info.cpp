#include "device_info.h"
#include <WiFi.h>

#define FIRMWARE_VERSION "1.0.0"
#define APP_NAME "sunbox"
#define DEVICE_MODEL "ESP32"

#define WIFI_SSID ".TigoWiFi-420920068/0";
#define WIFI_PASSWORD "WiFi-96629472";

String getDeviceId()
{
    uint64_t chipid = ESP.getEfuseMac();
    return String((uint32_t)(chipid >> 32), HEX) +
           String((uint32_t)chipid, HEX);
}

String getFirmwareVersion()
{
    return FIRMWARE_VERSION;
}

uint32_t getFreeHeap()
{
    return ESP.getFreeHeap();
}

String getAppName()
{
    return APP_NAME;
}

String getDeviceModel()
{
    return DEVICE_MODEL;
}
