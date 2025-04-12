

#include <TFT_eSPI.h>
#include <TouchScreen.h>


TFT_eSPI tft = TFT_eSPI();

// Estructura para los botones
struct Button {
  int x, y, w, h;
  String caption;
  uint16_t color;
  uint16_t textColor;
  int id;
  byte textSize;
};

#define NUM_BUTTONS 16
Button buttons[NUM_BUTTONS];

// Paleta de colores
#define BUTTON_COLOR TFT_BLUE
#define BUTTON_PRESSED_COLOR TFT_RED
#define BACKGROUND_COLOR TFT_BLACK
#define TEXT_COLOR TFT_WHITE
#define TEXT_PRESSED_COLOR TFT_BLACK
#define STATUS_BAR_COLOR TFT_DARKGREY
#define WIFI_CONNECTED_COLOR TFT_GREEN
#define WIFI_DISCONNECTED_COLOR TFT_RED

// Estados del menú
enum MenuState {
  MAIN_MENU,
  ROVER_MENU
};

MenuState currentMenu = MAIN_MENU;

// Textos para los botones en diferentes menús
const String main_menu[NUM_BUTTONS] = {
  "WiFi", "ROVER", "Config", "Info", 
  "Sensores", "Camara", "Sistema", "Calibrar",
  "Ajustes", "Red", "Vol+", "Vol-", 
  "Salir", "Apagar", "?", "Help"
};

const String rover_menu[NUM_BUTTONS] = {
  "↑", "↓", "←", "→", 
  "Avanzar", "Retroced", "Girar L", "Girar R",
  "Veloc+", "Veloc-", "Stop", "Home", 
  "Menu", "Auto", "Luz", "Sensores"
};

// Función isInside para detectar si un punto está dentro de un botón
bool isInside(int x, int y, Button btn) {
  return (x >= btn.x && x < (btn.x + btn.w)) && (y >= btn.y && y < (btn.y + btn.h));
}

#define CALIBRATION_FILE "/calibrationData"

void calibrar_pantalla()
{
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;
  // check file system
  if (!SPIFFS.begin()) {
    Serial.println("formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }
  // check if calibration file exists
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f) {
      if (f.readBytes((char *)calibrationData, 14) == 14)
        calDataOK = 1;
      f.close();
    }
  }
  if (calDataOK) {
    // calibration data valid
    tft.setTouch(calibrationData);
  } else {
    // data not valid. recalibrate
    tft.fillScreen((0xFFFF));
    tft.setCursor(20, 0, 2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(1);
    tft.println("calibration run");
    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calibrationData, 14);
      f.close();
    }
  }
  tft.fillScreen(BACKGROUND_COLOR);
}

void init_pantalla()
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND_COLOR);
  calibrar_pantalla();    // Sólo se ejecuta si no se encuentra el fichero de calibración
}

byte calculateBestTextSize(Button btn) {
  tft.setTextSize(1);
  int textWidth = tft.textWidth(btn.caption);
  
  byte size = 1;
  for (byte s = 2; s <= 4; s++) {
    tft.setTextSize(s);
    if (tft.textWidth(btn.caption) < btn.w * 0.9 && tft.fontHeight() < btn.h * 0.7) {
      size = s;
    } else {
      break;
    }
  }
  return size;
}

void limpiar_pantalla(bool par)
{
  tft.fillScreen(BACKGROUND_COLOR);
}

void drawWSIcon(int x, int y, bool connected) {
  uint16_t color = connected == 1? WIFI_CONNECTED_COLOR : WIFI_DISCONNECTED_COLOR;
  // Ajuste de posición más a la izquierda
  tft.setCursor(x,y);
  tft.setTextColor(color);
  tft.print("WS");
}

void drawWiFiIcon(int x, int y, bool connected) {
  uint16_t color = connected ? WIFI_CONNECTED_COLOR : WIFI_DISCONNECTED_COLOR;
  // Ajuste de posición más a la izquierda
  x += 5; // Margen izquierdo reducido
  // Tres arcos concéntricos
  for (int i = 0; i < 4; i++) {
    int radius = 12 - (i * 3);
    int startAngle = 120;
    int endAngle = 240;
    // Dibujar arco con grosor
    tft.drawArc(x + 8, y + 15, radius, radius, startAngle, endAngle, color, color);
  }
  tft.fillCircle(x + 8, y + 15, 1, color);
}

void drawBatteryIcon(int x, int y, int percent) {
  tft.drawRect(x, y, 20, 10, TFT_WHITE);
  tft.fillRect(x + 20, y + 3, 2, 4, TFT_WHITE);
  int fillWidth = map(percent, 0, 100, 0, 18);
  tft.fillRect(x + 1, y + 1, fillWidth, 8, percent > 20 ? TFT_GREEN : TFT_RED);
}



void drawButton(Button btn) {
  tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 5, btn.color);
  tft.drawRoundRect(btn.x, btn.y, btn.w, btn.h, 5, TFT_WHITE);
  
  tft.setTextColor(btn.textColor);
  tft.setTextSize(btn.textSize);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(btn.caption, btn.x + btn.w/2, btn.y + btn.h/2);
}

void drawAllButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    drawButton(buttons[i]);
  }
}
// Dimensiones
#define STATUS_BAR_HEIGHT 30  // Altura de la franja de estado
#define BUTTON_MARGIN 5       // Margen entre botones


void drawStatusBar(String message) {
  tft.fillRect(0, 0, tft.width(), STATUS_BAR_HEIGHT, STATUS_BAR_COLOR);
  // Mensaje principal
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(5, (STATUS_BAR_HEIGHT - 8) / 2);
  tft.print(message);
  Serial.print("ws:"); Serial.println(ws_connected);
  drawWSIcon(tft.width() - 90, 2, ws_connected);
  drawWiFiIcon(tft.width() - 60, 2, wifi_status == 3);
  drawBatteryIcon(tft.width() - 30, 5, 75); // 75% de carga
}

void initButtons(MenuState menu) {
  int buttonWidth = tft.width() / 4 - 10;
  int buttonHeight = tft.height() / 4 - 15;
  int margin = 5;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      int index = row * 4 + col;
      buttons[index].x = col * (buttonWidth + margin) + 2*margin;
      buttons[index].y = row * (buttonHeight + margin) + 2*margin + 30;
      buttons[index].w = buttonWidth;
      buttons[index].h = buttonHeight;
      
      // Asignar texto según el menú actual
      if(menu == MAIN_MENU) {
        buttons[index].caption = main_menu[index];
        buttons[index].color = TFT_BLUE; // Color para menú principal
      } else {
        buttons[index].caption = rover_menu[index];
        buttons[index].color = TFT_RED; // Color diferente para menú rover
      }
      
      buttons[index].textColor = TEXT_COLOR;
      buttons[index].id = index;
      buttons[index].textSize = calculateBestTextSize(buttons[index]);
    }
  }
}

void switchMenu(MenuState newMenu) {
  currentMenu = newMenu;
  initButtons(currentMenu);
  limpiar_pantalla(false);
  drawAllButtons();
  
  // Actualizar barra de estado
  if(currentMenu == MAIN_MENU) {
    drawStatusBar("Menu Principal");
  } else {
    drawStatusBar("Control Rover");
  }
}
void upressBtton(int btnIndex) {
  // Feedback visual
  buttons[btnIndex].color = BUTTON_PRESSED_COLOR;
  buttons[btnIndex].textColor = TEXT_PRESSED_COLOR;
  drawButton(buttons[btnIndex]);
  
  // Restaurar apariencia después de un breve retraso
  delay(150);
  buttons[btnIndex].color = currentMenu == MAIN_MENU ? TFT_BLUE : TFT_GREEN;
  buttons[btnIndex].textColor = TEXT_COLOR;
  drawButton(buttons[btnIndex]);
}

int lee_boton() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    for (int btn=0; btn<NUM_BUTTONS; btn++) {
      if (isInside(x, y, buttons[btn])) {
        upressBtton(btn);
        
        if(currentMenu == MAIN_MENU) {
          // Manejar botones del menú principal
          switch(btn) {
            case 1: // Botón ROVER
              switchMenu(ROVER_MENU);
              return 0;
            // ... otros casos del menú principal
          }
        } 
        else {
          // Manejar botones del menú rover
          switch(btn) {
            case 12: // Botón Menu
              switchMenu(MAIN_MENU);
              return 0;
            case 0: // ↑
              //sendWSCommand("{\"action\":\"move\",\"direction\":\"up\"}");
              break;
            case 1: // ↓
              //sendWSCommand("{\"action\":\"move\",\"direction\":\"down\"}");
              break;
            // ... otros casos del menú rover
          }
        }
        return btn+1;
      }
    }
  }
  return 0;
}


