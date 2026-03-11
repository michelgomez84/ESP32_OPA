#include "aws_provisioning_manager.h"
#include "aws_certificates.h" // Asegúrate que contiene AWS_ROOT_CA, AWS_DEVICE_CERT (Claim), etc.
#include "device_info.h"      // Contiene getDeviceId(), getDeviceModel(), getAppName()
#include "config_manager.h"

// Configuración de AWS
const char* awsEndpoint1 = "a2r8wnw1c348d3-ats.iot.us-east-2.amazonaws.com";
const int awsPort1 = 8883;
const char* provisioningTemplate = "ESP32_Test_Template";

Aws_ProvisioningManager* Aws_ProvisioningManager::instance = nullptr;

Aws_ProvisioningManager::Aws_ProvisioningManager() : mqtt(netclient) {
    instance = this;
    provisioningSuccess = false;
}

bool Aws_ProvisioningManager::begin() {
    netclient.setCACert(AWS_ROOT_CA);
    netclient.setCertificate(AWS_DEVICE_CERT); // Certificado de Claim
    netclient.setPrivateKey(AWS_PRIVATE_KEY);   // Llave de Claim

    mqtt.setServer(awsEndpoint1, awsPort1);
    mqtt.setCallback(mqttCallback);
    mqtt.setBufferSize(4096); // IMPORTANTE: Aumentar buffer para recibir llaves/certificados

    return true;
}

void Aws_ProvisioningManager::connectMQTT() {
    String clientId = "Provisioner-" + getDeviceId();
    while (!mqtt.connected()) {
        Serial.print("Connecting to AWS IoT for provisioning...");
        if (mqtt.connect(clientId.c_str())) {
            Serial.println("Connected!!!");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" Trying again in 2 seconds...");
            delay(2000);
        }
    }
}

bool Aws_ProvisioningManager::startProvisioning() {
    connectMQTT();

    // Fase 1: Obtener Certificado y Llave Privada
    mqtt.subscribe("$aws/certificates/create/json/accepted");
    mqtt.subscribe("$aws/certificates/create/json/rejected");

    requestCertificate();

    if (!waitForResponse(certificatePem, 15000)) {
        Serial.println("Error: AWS certificate not received.");
        return false;
    }

    // Fase 2: Registrar el "Thing" con la plantilla
    String provTopicAccepted = "$aws/provisioning-templates/" + String(provisioningTemplate) + "/provision/json/accepted";
    String provTopicRejected = "$aws/provisioning-templates/" + String(provisioningTemplate) + "/provision/json/rejected";
    
    mqtt.subscribe(provTopicAccepted.c_str());
    mqtt.subscribe(provTopicRejected.c_str());

    registerThing();

    if (!waitForProvisioning(15000)) {
        Serial.println("Error: Device registration failed on AWS.");
        return false;
    }

    // Guardar resultados
    // Supone que tienes una función externa definida para persistir en NVS
    Serial.println("Certificates: " + instance->certificatePem + " / " + instance->privateKey);
     aws_saveCertificates(instance->certificatePem, instance->privateKey);

    Serial.println("Provisioning completed successfully.");
    return true;
}

void Aws_ProvisioningManager::requestCertificate() {
    // Payload vacío para Create Certificate from Amazon
    mqtt.publish("$aws/certificates/create/json", "");
}

void Aws_ProvisioningManager::registerThing() {
    StaticJsonDocument<1024> doc;
    doc["certificateOwnershipToken"] = ownershipToken;

    JsonObject params = doc.createNestedObject("parameters");
    params["SerialNumber"] = getDeviceId();
    params["DeviceModel"] = getDeviceModel();
    params["AppName"] = getAppName();

    char buffer[1024];
    serializeJson(doc, buffer);

    String topic = "$aws/provisioning-templates/" + String(provisioningTemplate) + "/provision/json";
    mqtt.publish(topic.c_str(), buffer);
}

bool Aws_ProvisioningManager::waitForResponse(String& targetVar, int timeoutMs) {
    unsigned long start = millis();
    while (targetVar == "" && (millis() - start < timeoutMs)) {
        mqtt.loop();
        delay(10);
    }
    return (targetVar != "");
}

bool Aws_ProvisioningManager::waitForProvisioning(int timeoutMs) {
    unsigned long start = millis();
    while (!provisioningSuccess && (millis() - start < timeoutMs)) {
        mqtt.loop();
        delay(10);
    }
    return provisioningSuccess;
}

void Aws_ProvisioningManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Usamos DynamicJsonDocument porque el payload puede ser > 2KB
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }

    String topicStr = String(topic);

    if (topicStr.endsWith("/accepted")) {
        if (topicStr.indexOf("certificates/create") >= 0) {
            instance->certificatePem = doc["certificatePem"].as<String>();
            instance->privateKey = doc["privateKey"].as<String>();
            instance->ownershipToken = doc["certificateOwnershipToken"].as<String>();
            Serial.println("Certificates received correctly.");
        } 
        else if (topicStr.indexOf("provision") >= 0) {
            instance->provisioningSuccess = true;
            Serial.println("Registration in AWS IoT successful.");
        }
    } 
    else if (topicStr.endsWith("/rejected")) {
        Serial.println("Request REJECTED by AWS. Please review the policies attached to the Claim Certificate.");
        // Imprimir el error de AWS para debug
        const char* errorCode = doc["errorCode"];
        const char* errorMessage = doc["errorMessage"];
        Serial.printf("Error Code: %s, Message: %s\n", errorCode, errorMessage);
    }
}