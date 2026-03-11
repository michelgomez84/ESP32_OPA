#pragma once

class Wifi_Manager
{
public:

    bool Connect(const char* ssid, const char* password);

    bool IsConnected();

    void syncTime();   // nuevo método

};