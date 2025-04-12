
// ESP32 pantalla 4"

//#include <SPI.h>
#include <FS.h>

int wifi_status = 0;
bool ws_connected = false;
int actual_action = 0;
float temp = 0.0;
float hum = 0.0;
float azim = 0;
float elev = 0;
float giro = 0;
int ang = 0;
float dist = 0;

#include "config.h"
#include "pantalla.h"
#include "miwifi.h"
#include "websockets.h"
#include "controlrover.h"

long time1 = 0;  long timedelay1 = 1000;
long time60 = 0; long timedelay60 = 60000;

// Definiciones de acciones
void Conectar_WiFi() {
  limpiar_pantalla(false);
  tft.setTextSize(2);
  conectar_wifi(ssids[ssid_con], passwords[ssid_con], false);
  drawAllButtons();
  drawStatusBar(WiFi.status()==WL_CONNECTED?"Conectado: " + WiFi.localIP().toString():"No conectado");
}

// Definiciones de acciones
void actionPower() {

  //  aquí las acciones que toquen
  //tft.fillScreen(BACKGROUND_COLOR);
  
  //
  //drawAllButtons();
}

void actionMode() {
  //  aquí las acciones que toquen
  //tft.fillScreen(BACKGROUND_COLOR);
  
  //
  //drawAllButtons();
}

void executeAction(int btnId) {
  Serial.print("boton:"); Serial.println(btnId);
  switch(btnId) {
    case 1: Conectar_WiFi(); break;
    case 2: control_rover(); break;
    case 3: actionPower(); break;
    case 4: actionPower(); break;
    case 5: actionPower(); break;
    case 6: actionPower(); break;
    case 7: actionPower(); break;
    case 8: actionPower(); break;
    case 9: actionPower(); break;
    case 10: actionPower(); break;
    case 11: actionPower(); break;
    case 12: actionPower(); break;
    case 13: actionPower(); break;
    case 14: actionPower(); break;
    case 15: actionPower(); break;
    case 16: actionPower(); break;
  }
}

void setup(void) 
{
  Serial.begin(115200);
  Serial.println();
  init_pantalla();
  tft.setTextSize(2);
  drawStatusBar("Iniciando...");
  initButtons(MAIN_MENU);
  if (conectar_wifi_inicio == 1)
    {
    conectar_wifi(ssids[ssid_con], passwords[ssid_con], false);
    }
  drawAllButtons();
  drawStatusBar(WiFi.status()==WL_CONNECTED?"Conectado: " + WiFi.localIP().toString():"No conectado");
}

void cosas_cada_segundo(){  time1 = millis();}
void cosas_cada_segundo60(){  time60 = millis();}

void loop() {
  if ((millis() - time1) > timedelay1)   { cosas_cada_segundo(); }
  if ((millis() - time60) > timedelay60) { cosas_cada_segundo60(); }
  if (actual_action == 0)
    {
    int boton = lee_boton();
    if ((boton > 0) && (boton <= 16)) {
      executeAction(boton);
      }
    }
    
}

