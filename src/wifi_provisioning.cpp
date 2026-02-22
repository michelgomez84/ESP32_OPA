#include "wifi_provisioning.h"
#include "config_manager.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;
const char* apSSID = "SUNBOX_SETUP";
const char* apPassword = "12345678";

void handleRoot()
{
    String html =
    "<html><body>"
    "<h2>WiFi Setup</h2>"
    "<form action='/save'>"
    "SSID: <input name='ssid'><br>"
    "Password: <input name='pass'><br>"
    "<input type='submit' value='Submit'>"
    "</form>"
    "</body></html>";

    server.send(200, "text/html", html);
}

void handleSave()
{
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    saveWifiConfig(ssid, pass);

    server.send(200, "text/html",
        "<h2>Saved. Restarting...</h2>");

    delay(2000);
    ESP.restart();
}

void startProvisioningPortal()
{
    WiFi.softAP(apSSID, apPassword);

    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/save", handleSave);

    server.begin();

    Serial.println("Setup Portal Initiated...");
    Serial.println(WiFi.softAPIP());

    while (true)
    {
        dnsServer.processNextRequest();
        server.handleClient();
    }
}