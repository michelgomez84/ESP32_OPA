//(Captive Portal real + auto-redirect)
#include "wifi_portal.h"
#include "config_manager.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

static const byte DNS_PORT = 53;

static DNSServer dnsServer;
static WebServer server(80);

static String portalHtml()
{
  return R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>WiFi Setup</title>
  <style>
    body { font-family: Arial; margin: 24px; }
    input { width: 100%; padding: 10px; margin: 8px 0; }
    button { width: 100%; padding: 12px; }
    .box { max-width: 420px; margin:auto; }
  </style>
</head>
<body>
  <div class="box">
    <h2>WiFi Setup</h2>
    <form method="POST" action="/save">
      <label>SSID</label>
      <input name="ssid" required />
      <label>Password</label>
      <input name="pass" type="password" />
      <button type="submit">Submit</button>
    </form>
  </div>
</body>
</html>
)HTML";
}

// Redirección al portal
static void redirectToPortal()
{
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/");
  server.send(302, "text/plain", "");
}

// Endpoints de detección de SO (muy importante)
static void setupCaptiveEndpoints()
{
  // Android
  server.on("/generate_204", HTTP_GET, redirectToPortal);
  server.on("/gen_204", HTTP_GET, redirectToPortal);

  // Windows (NCSI)
  server.on("/ncsi.txt", HTTP_GET, []() { redirectToPortal(); });
  server.on("/connecttest.txt", HTTP_GET, []() { redirectToPortal(); });
  server.on("/redirect", HTTP_GET, redirectToPortal);

  // Apple iOS/macOS
  server.on("/hotspot-detect.html", HTTP_GET, redirectToPortal);
  server.on("/library/test/success.html", HTTP_GET, redirectToPortal);
  server.on("/success.html", HTTP_GET, redirectToPortal);

  // Chrome/others sometimes hit these
  server.on("/favicon.ico", HTTP_GET, []() { server.send(204); });
}

// Página principal
static void handleRoot()
{
  server.send(200, "text/html", portalHtml());
}

// Guardar credenciales
static void handleSave()
{
  const String ssid = server.arg("ssid");
  const String pass = server.arg("pass");

  if (ssid.length() == 0)
  {
    server.send(400, "text/plain", "Required SSID");
    return;
  }

  saveWifiConfig(ssid, pass);

  server.send(200, "text/html",
              "<h2>Saved. Restarting...</h2>");

  delay(1200);
  ESP.restart();
}

void startCaptivePortal(const WifiPortalConfig& cfg)
{
  // Modo AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(cfg.apIP, cfg.apIP, cfg.netM);
  WiFi.softAP(cfg.apSsid.c_str(), cfg.apPass.c_str());

  // DNS wildcard: cualquier dominio -> IP del AP
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Rutas
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  setupCaptiveEndpoints();

  // Cualquier otra ruta => redirige al portal
  server.onNotFound(redirectToPortal);

  server.begin();

  Serial.println("WiFi Setup Portal Initiated...");
  Serial.print("AP SSID: "); Serial.println(cfg.apSsid);
  Serial.print("AP IP  : "); Serial.println(WiFi.softAPIP());

  // Loop del portal (bloqueante a propósito)
  while (true)
  {
    dnsServer.processNextRequest();
    server.handleClient();
    delay(2);
  }
}