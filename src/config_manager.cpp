#include "config_manager.h"
#include <Preferences.h>

Preferences preferences;

void configInit()
{
    preferences.begin("device-config", false);
}

////////////////////////////
//// WIFI CONFIG
void saveWifiConfig(const String& ssid, const String& password)
{
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_pass", password);
}

bool loadWifiConfig(String& ssid, String& password)
{
    ssid = preferences.getString("wifi_ssid", "");
    password = preferences.getString("wifi_pass", "");

    return ssid.length() > 0;
}

bool removeWifiConfig()
{
    if(preferences.remove("wifi_ssid") && preferences.remove("wifi_pass"))
    {
        return true;
    }    
}

////////////////////////////
//// AWS PROVISIONING 

bool aws_isProvisioned()
{
    bool exists = preferences.isKey("cert");
    return exists;
}

void aws_saveCertificates(String cert, String key)
{
    preferences.putString("cert", cert);
    preferences.putString("key", key);
}

bool aws_loadCertificates(String& cert, String& key)
{
    cert = preferences.getString("cert", "");
    key = preferences.getString("key", "");

    return cert.length() > 0;
}


bool aws_removeCertificates()
{
    if(preferences.remove("cert") && preferences.remove("key"))
    {
        return true;
    }    
}
