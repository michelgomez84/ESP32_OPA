#include "config_manager.h"
#include <Preferences.h>

Preferences preferences;

void configInit()
{
    preferences.begin("device-config", false);
}

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