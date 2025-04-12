

#include "WiFi.h"

char ssids[3][30] = {"MikroTik-E1E1FB", "ROVER_DIEGO", "Adeept"};
char passwords[3][30] = {"18291892", "12341234", "12345678"};
char socketservers[3][30] = {"192.168.11.91", "192.168.4.1", ""};
const uint16_t websocket_port = 81;
const char* websocket_path = "/";  // Ruta del WebSocket, puede ser "/"

void conectar_wifi(char* ssid, char* password, bool silencio)
{
  limpiar_pantalla(false);
  tft.setTextSize(2);
  tft.setCursor(0, 1);
  if (silencio) {
    tft.print("Conectando a ");  tft.println(ssids[ssid_con]);
    }
  else {
    tft.print("Conectando a ");  tft.println(ssids[ssid_con]);
    tft.print("con password ");  tft.println(passwords[ssid_con]);
    tft.println("WiFi.begin");
    }
  Serial.print("Conectando a ");  Serial.print(ssids[ssid_con]);
  Serial.print(" con password ");  Serial.print(passwords[ssid_con]);
  int retry = 15;
  WiFi.begin (ssids[ssid_con], passwords[ssid_con]);
  while ((WiFi.status() != WL_CONNECTED) && (retry>0))
    {
    retry--;
    if (!silencio) {
      tft.print(".");  
      }
    Serial.print(".");
    delay(500); 
    }
  wifi_status = WiFi.status();
  if (WiFi.status() == WL_CONNECTED)
    {
    if (ssid_con == 0)
      {
      IPAddress dns(8, 8, 8, 8);  // DNS de Google (puedes usar otro)
      IPAddress subnet(255, 255, 255, 0);  // DNS de Google (puedes usar otro)
      WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), subnet, dns);  // Establece subnet y DNS  
      Serial.print("  Conectado "); Serial.println(WiFi.localIP());
      delay(1000);
      }
    else if  (ssid_con == 1)
      {
      IPAddress iplocal(192, 168, 4, 9);  // 
      IPAddress gateway(192, 168, 4, 1);  //
      IPAddress dns(8, 8, 8, 8);  // 
      IPAddress subnet(255, 255, 255, 0);  // 
      WiFi.config(iplocal, gateway, subnet, dns);  // Establece subnet y DNS  
      Serial.print("  Conectado "); Serial.println(WiFi.localIP());
      delay(1000);
      }
    else if  (ssid_con == 2)
      {
      IPAddress iplocal(192, 168, 12, 9);  // 
      IPAddress gateway(192, 168, 12, 1);  // 
      IPAddress dns(8, 8, 8, 8);  //
      IPAddress subnet(255, 255, 255, 0); 
      WiFi.config(iplocal, gateway, subnet, dns);  // Establece subnet y DNS  
      Serial.print("  Conectado "); Serial.println(WiFi.localIP());
      delay(1000);
      }
    }
  else
    {
    Serial.println("  NO Conectado"); 
    delay(3000);
    }
  if (!silencio) {
    limpiar_pantalla(false);
    drawAllButtons();
    }
}
