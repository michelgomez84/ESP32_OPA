#pragma once
#include <Arduino.h>

struct WifiPortalConfig {
  String apSsid = "SUNBOX_SETUP";
  String apPass = "12345678";   // mínimo 8 chars para WPA2
  IPAddress apIP = IPAddress(192,168,4,1);
  IPAddress netM = IPAddress(255,255,255,0);
};

void startCaptivePortal(const WifiPortalConfig& cfg);