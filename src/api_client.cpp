#include "api_client.h"
#include "device_info.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* baseUrl = "https://sac.rodelag.com";
//"https://sac.rodelag.com/api/firmware/check";
static const char* rootCACert = R"EOF(
-----BEGIN CERTIFICATE-----
TU_CERTIFICADO_CA_AQUI
-----END CERTIFICATE-----
)EOF";

bool apiGetFirmwareInfo(FirmwareInfo& info)
{
    WiFiClientSecure client;
    client.setInsecure();
    //client.setCACert(rootCACert);

    HTTPClient https;

    String url = String(baseUrl) + "/api/DeviceVersion/GetVersionAsync?appname=" + getAppName() + "&devicemodel=" + getDeviceModel();
    if (!https.begin(client, url))
        return false;

    https.addHeader("Content-Type", "application/json");
    int httpCode = https.GET();
    
    if (httpCode != 200)
    {
        https.end();
        return false;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error =
        deserializeJson(doc, https.getString());

    if (error)
    {
        https.end();
        return false;
    }

    info.version = doc["version"].as<String>();
    info.firmwareUrl = doc["firmware"].as<String>();

    https.end();
    return true;
}