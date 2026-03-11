#pragma once
#include <Arduino.h>


void configInit();

////////////////////////////
//// WIFI CONFIG
void saveWifiConfig(const String& ssid, const String& password);
bool loadWifiConfig(String& ssid, String& password);
bool removeWifiConfig();

////////////////////////////
//// AWS PROVISIONING
bool aws_isProvisioned();
void aws_saveCertificates(String cert, String key);
bool aws_loadCertificates(String& cert, String& key);
bool aws_removeCertificates();
