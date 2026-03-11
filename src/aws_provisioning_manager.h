#ifndef AWS_PROVISIONING_MANAGER_H
#define AWS_PROVISIONING_MANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class Aws_ProvisioningManager {
public:
    Aws_ProvisioningManager();
    bool begin();
    bool startProvisioning();

private:
    void connectMQTT();
    void requestCertificate();
    void registerThing();
    bool waitForResponse(String& targetVar, int timeoutMs);
    bool waitForProvisioning(int timeoutMs);

    // Callback estático requerido por PubSubClient
    static void mqttCallback(char* topic, byte* payload, unsigned int length);

    WiFiClientSecure netclient;
    PubSubClient mqtt;

    // Variables de estado
    String certificatePem;
    String privateKey;
    String ownershipToken;
    bool provisioningSuccess;

    // Singleton para acceso desde el callback estático
    static Aws_ProvisioningManager* instance;
};

#endif