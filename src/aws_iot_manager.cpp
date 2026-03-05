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

String deviceId = getDeviceId();

//Maneja los mensajes recibidos desde AWS IoT Core. En este ejemplo, simplemente se imprimen en el monitor serial, pero aquí es donde se podrían implementar acciones específicas basadas en los comandos recibidos.
void messageHandler(char* topic, byte* payload, unsigned int length)
{
    Serial.println("Message from AWS received");

    String msg;

    for(int i=0;i<length;i++)
        msg += (char)payload[i];

    Serial.println(msg);
}

//Inicializa la conexión con AWS IoT Core. Configura los certificados, establece la conexión MQTT y se suscribe al topic de comandos.
void awsInit()
{    
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

    // SUBSCRIBE TOPICS
    String topicCommands[5] = {
    "$aws/things/" + deviceId + "/shadow/get/accepted",
    "$aws/things/" + deviceId + "/shadow/get/rejected",
    "$aws/things/" + deviceId + "/shadow/update/accepted",
    "$aws/things/" + deviceId + "/shadow/update/rejected",
    "$aws/things/" + deviceId + "/shadow/update/delta"
    };

    for (int i = 0; i < 5; i++){
        if(client.subscribe(topicCommands[i].c_str())){
            Serial.println("Subscribed to topic: " + topicCommands[i]);
            delay(250);
        } else {
            Serial.println("Failed to subscribe to topic: " + topicCommands[i]);
        }
    }
}

//En este bucle, se llama a client.loop() para mantener la conexión MQTT activa y procesar los mensajes entrantes. Además, se podría implementar una lógica para publicar telemetría periódicamente o en respuesta a eventos específicos.
void awsLoop()
{
    client.loop();
}

//Esta función se encarga de publicar la telemetría del dispositivo en el topic correspondiente. En este ejemplo, se crea un documento JSON con información del dispositivo, como su ID, versión de firmware, memoria disponible y nivel de señal WiFi, y luego se publica en AWS IoT Core.
void awsPublishTelemetry()
{
    String topic = "$aws/things/" + deviceId + "/shadow/get";
    StaticJsonDocument<200> doc;

    doc["device"] = getDeviceId();
    doc["firmware"] = getFirmwareVersion();
    doc["heap"] = getFreeHeap();
    doc["rssi"] = WiFi.RSSI();

    char buffer[256];

    serializeJson(doc, buffer);

    client.publish(topic.c_str(), buffer);
}

void awsPublishUpdateShadow()
{
    String topic = "$aws/things/" + deviceId + "/shadow/update";
    StaticJsonDocument<200> doc;

    JsonObject state = doc.createNestedObject("state");
    JsonObject desired = state.createNestedObject("desired");
    desired["date"] = "2026-03-05";
    desired["locked"] = "0";
    desired["pincode"] = "1111";
    //JsonObject code = desired.createNestedObject("code");
    //code["pin"] = "1111";
    //code["validUntil"] = "2025-02-18T01:00:01.000Z";

    char buffer[256];

    serializeJson(doc, buffer);

    client.publish(topic.c_str(), buffer);
}

/*
{
  "state": {
    "desired": {
      "lockerStatus": "locked",
      "code": {
        "pin": "1111",
        "validUntil": "2025-02-18T01:00:01.000Z"
      }
    }
  }
}
*/