

#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;
int data_sensor_rec = 0;
int data_radar_rec = 0;
int data_giros_rec = 0;

WebsocketsClient wsclient;
const char* websockets_server_host = "192.168.4.1"; //Enter server adress
const uint16_t websockets_server_port = 81; // Enter server port

void extraerDatosJSON(const String& json) {
  StaticJsonDocument<256> doc;

  // Parsear el JSON
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Error al parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Detectar tipo
  const char* type = doc["type"];
  if (type == nullptr) {
    Serial.println("Campo 'type' no encontrado");
    return;
  }

  // Mostrar datos según el tipo
  if (strcmp(type, "sensor") == 0) {
    temp = doc["t"];
    hum = doc["h"];
    data_sensor_rec = 1;
    } 
  else if (strcmp(type, "giros") == 0) {
    azim = doc["a"];
    elev = doc["e"];
    giro = doc["g"];
    data_giros_rec = 1;
    } 
  else if (strcmp(type, "radar") == 0) {
    ang = doc["ang"];
    dist = doc["dis"];
    data_radar_rec = 1;
    } 
  else {
    Serial.print("Tipo desconocido: "); Serial.println(type);
    }
}

// Función aparte para manejar los mensajes recibidos
void handleWebSocketMessage(WebsocketsMessage message) {
  //Serial.print("Rec: ");  Serial.println(message.data());
  extraerDatosJSON(message.data());
  Serial.print(temp);Serial.print("-");
  Serial.print(hum);Serial.print("-");
  Serial.print(azim);Serial.print("-");
  Serial.print(elev);Serial.print("-");
  Serial.print(giro);Serial.print("-");
  Serial.print(ang);Serial.print("-");
  Serial.println(dist);
}

void init_websockets()
{
   Serial.println("Connected to Wifi, Connecting to server.");
    // try to connect to Websockets server
    ws_connected = wsclient.connect(websockets_server_host, websockets_server_port, "/");
    if (ws_connected) {
      Serial.println("Conectado WS");
      wsclient.send("Hola Rover");
    } else {
      Serial.println("No conectado");
    }
    // run callback when messages are received
    wsclient.onMessage(handleWebSocketMessage);
}

void loop_websockets()
{
  if(wsclient.available()) {
    wsclient.poll();
    }

}