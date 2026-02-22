#pragma once
#include <Arduino.h>

struct FirmwareInfo
{
    String version;
    String firmwareUrl;
};

bool apiGetFirmwareInfo(FirmwareInfo& info);