#pragma once
#include <Arduino.h>

void configInit();
void saveWifiConfig(const String& ssid, const String& password);
bool loadWifiConfig(String& ssid, String& password);
bool removeWifiConfig();