#include "aws_iot_manager.h"
#include "device_info.h"

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "aws_certificates.h"

WiFiClientSecure net;
PubSubClient client(net);

const char* awsEndpoint =
"ag1np0o0njzia-ats.iot.us-east-1.amazonaws.com";

const int awsPort = 8883;

String topicTelemetry;
String topicCommand;

void messageHandler(char* topic, byte* payload, unsigned int length)
{
    Serial.println("Mensaje AWS recibido");

    String msg;

    for(int i=0;i<length;i++)
        msg += (char)payload[i];

    Serial.println(msg);
}

void awsInit()
{
    topicTelemetry =
    "sunbox/device/" + getDeviceId() + "/telemetry";

    topicCommand =
    "sunbox/device/" + getDeviceId() + "/command";

    net.setCACert(AWS_ROOT_CA);
    net.setCertificate(AWS_DEVICE_CERT);
    net.setPrivateKey(AWS_PRIVATE_KEY);

    client.setServer(awsEndpoint, awsPort);
    client.setCallback(messageHandler);

    Serial.println("Connecting to AWS IoT...");

    while (!client.connect(getDeviceId().c_str()))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("AWS Connected!!!");

    client.subscribe(topicCommand.c_str());
}

void awsLoop()
{
    client.loop();
}

void awsPublishTelemetry()
{
    StaticJsonDocument<200> doc;

    doc["device"] = getDeviceId();
    doc["firmware"] = getFirmwareVersion();
    doc["heap"] = getFreeHeap();
    doc["rssi"] = WiFi.RSSI();

    char buffer[256];

    serializeJson(doc, buffer);

    client.publish(topicTelemetry.c_str(), buffer);
}