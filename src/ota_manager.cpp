#include "ota_manager.h"
#include "api_client.h"
#include "device_info.h"
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>


void otaCheckAndUpdate()
{
    FirmwareInfo info;

    if (!apiGetFirmwareInfo(info))
    {
        Serial.println("Error retrieving firmware information");
        return;
    }

    Serial.println("Local Version: " + getFirmwareVersion());
    Serial.println("Remote Version: " + info.version);

    if (info.version != getFirmwareVersion())
    {
        Serial.println("New version detected. Firmware update in progress...");

        WiFiClientSecure client;
        client.setInsecure();
        //client.setCACert(rootCACert);

        t_httpUpdate_return ret =
            httpUpdate.update(client, info.firmwareUrl);

        switch (ret)
        {
            case HTTP_UPDATE_FAILED:
                Serial.printf("Update failed: %s\n",
                    httpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_OK:
                Serial.println("Update OK");
                break;

            default:
                Serial.println("No update");
                break;
        }
    }
    else
    {
        Serial.println("Already updated");
    }
}