#include <Arduino.h>
#include "wifi_manager.h"
#include "wifi_provisioning.h"
#include "ota_manager.h"
#include "device_info.h"
#include "config_manager.h"
#include "wifi_portal.h"

const char* ssid = ".TigoWiFi-420920068/0";
const char* password = "WiFi-96629472";

void setup()
{
    Serial.begin(115200);
    delay(500);

    configInit();

    Serial.println("Device ID: " + getDeviceId());
    Serial.println("Heap: " + String(getFreeHeap()));

    //Cuando el dispositivo se inicia por primera vez, no hay configuración guardada, así que se conecta con los datos hardcodeados y luego los guarda en la memoria flash para futuros reinicios
    /*if (wifiConnect(ssid, password))
    {
        otaCheckAndUpdate();
    }*/
    
    String ssid;
    String pass;

    WifiPortalConfig portalCfg;
    portalCfg.apSsid = "SUNBOX_SETUP";
    portalCfg.apPass = "12345678";
    //removeWifiConfig(); // Para pruebas: eliminar configuración guardada y forzar portal
    // 1) Si no hay WiFi guardado => Portal
    if (!loadWifiConfig(ssid, pass))
    {
         startCaptivePortal(portalCfg);
    }
    // 2) Si hay pero no conecta => Portal
    if (!wifiConnect(ssid.c_str(), pass.c_str()))
    {       
        startCaptivePortal(portalCfg);
    }
    // 3) Ya conectado => flujo normal
    otaCheckAndUpdate();    
}

void loop()
{
    // Tu lógica principal
    Serial.println("Tick...");
    delay(1000);
}

/*
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello World desde NodeMCU ESP8266!");
}

void loop() {
  Serial.println("Tick...");
  delay(1000);
}
*/
