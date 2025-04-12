


// Función mejorada: dibuja texto y limpia fondo del área ocupada
void drawText(String texto, int x, int y, uint16_t colorTexto, uint16_t colorFondo = TFT_BLACK, int textSize = 2) {
  tft.setTextSize(textSize);
  tft.setTextFont(1); // Asegura compatibilidad

  int charWidth = 6 * textSize;  // Tamaño aproximado por carácter (fuente 1)
  int charHeight = 8 * textSize;
  int w = texto.length() * charWidth;
  int h = charHeight;
  // Borrar área
  tft.fillRect(x, y, w, h, colorFondo);
  // Dibujar texto
  tft.setTextColor(colorTexto, colorFondo);
  tft.drawString(texto, x, y);
}

void drawThermometerBar(int x, int y, int width, int height, float value, float minVal, float maxVal, uint16_t color, String label) {
  tft.drawRect(x, y, width, height, TFT_WHITE); // borde
  int filledHeight = map(value, minVal, maxVal, 0, height);
  tft.fillRect(x + 1, y + height - filledHeight, width - 2, filledHeight, color);
  tft.setTextColor(TEXT_COLOR, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(label + String(value, 1), x + width / 2, y + height + 10);
}

void showTempHumidity(float temperature, float humidity) {
  tft.setTextFont(2);
  tft.setTextSize(1);
  drawThermometerBar(5, 40, 15, 50, temperature, 0, 50, TFT_GREEN, "");
  drawThermometerBar(455, 40, 15, 50, humidity, 0, 100, TFT_BLUE, "");
}

#define RADAR_RADIUS 80  // radio del radar
#define RADAR_CENTER_X 240
#define RADAR_CENTER_Y 120
#define MAX_DIST 500.0     // distancia máxima que se puede mostrar
#define MAX_POINTS 180      // Número total de puntos (de -90° a 90°)

// Guardamos las posiciones de los puntos y su distancia asociada
struct Point {
  int x;
  int y;
  float distance;
};

Point points[MAX_POINTS];  // Arreglo para guardar los puntos
int lastLineX = RADAR_CENTER_X; // Última posición de la línea de barrido (x)
int lastLineY = RADAR_CENTER_Y; // Última posición de la línea de barrido (y)

void drawRadarGuides() {
  // Líneas guía (dibujadas como arcos semicirculares en lugar de píxeles)
  tft.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS, TFT_GREEN);
  tft.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS-20, TFT_GREEN);
  tft.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS-40, TFT_GREEN);
  tft.drawCircle(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_RADIUS-60, TFT_GREEN);
  tft.fillRect(RADAR_CENTER_X-RADAR_RADIUS, RADAR_CENTER_Y, RADAR_RADIUS*2+2,RADAR_RADIUS+2,TFT_BLACK);
  tft.drawLine(RADAR_CENTER_X-RADAR_RADIUS-5, RADAR_CENTER_Y, RADAR_CENTER_X+RADAR_RADIUS+5, RADAR_CENTER_Y, TFT_GREEN);
  tft.drawLine(RADAR_CENTER_X, RADAR_CENTER_Y, RADAR_CENTER_X, RADAR_CENTER_Y-RADAR_RADIUS-5, TFT_GREEN);
}


void drawRadar(int ang, float dist) {
  // Calcular la posición del punto (ajustando el ángulo correctamente)
  float a_rad = radians(ang);
  float r = map(dist, 0, MAX_DIST, 0, RADAR_RADIUS);
  // Rotación 90° en sentido antihorario + reflejo de 180° en Y
  int x = RADAR_CENTER_X + cos(a_rad) * r;  // Rotado 90° en sentido antihorario
  int y = RADAR_CENTER_Y - sin(a_rad) * r;  // Reflejado en Y para que apunte hacia abajo

  // Borrar el punto anterior en la misma posición antes de dibujar el nuevo
  tft.fillCircle(points[ang].x, points[ang].y, 2, TFT_BLACK);  // Borra el punto anterior en negro (color de fondo)
  tft.fillCircle(x, y, 2, TFT_YELLOW);  // Punto en amarillo
    points[ang].x = x;
    points[ang].y = y;
    points[ang].distance = dist;
  // Borrar la línea de barrido anterior (redibujarla con el color de fondo)
  tft.drawLine(RADAR_CENTER_X, RADAR_CENTER_Y, lastLineX, lastLineY, TFT_BLACK);  // Borrar la línea de barrido anterior

  // Dibujar la línea de barrido de longitud completa
  int line_x = RADAR_CENTER_X + cos(a_rad) * RADAR_RADIUS;  // Línea completa del radio
  int line_y = RADAR_CENTER_Y - sin(a_rad) * RADAR_RADIUS;  // Línea completa del radio
  tft.drawLine(RADAR_CENTER_X, RADAR_CENTER_Y, line_x, line_y, TFT_GREEN);  // Línea de barrido verde
  // Guardar las coordenadas de la última línea de barrido para poder borrarla en el siguiente ciclo
  lastLineX = line_x;  lastLineY = line_y;

  // Mostrar textos debajo del radar
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.fillRect(RADAR_CENTER_X-RADAR_RADIUS, RADAR_CENTER_Y+1, 150,20, TFT_BLACK);
  tft.drawString(String((int)ang), RADAR_CENTER_X-RADAR_RADIUS/2, RADAR_CENTER_Y + 5);
  tft.drawString(String((int)dist), RADAR_CENTER_X+RADAR_RADIUS/2, RADAR_CENTER_Y + 5);
}

void control_rover()
{
  if (wifi_status != 3) {
    conectar_wifi(ssids[ssid_con], passwords[ssid_con], true);
    }
  actual_action = 2;
  limpiar_pantalla(false);
  init_websockets();
  drawStatusBar("Rover");
  drawRadarGuides();
  while (actual_action == 2)
    {
    if (data_sensor_rec == 1) {
      showTempHumidity(temp, hum);
      }
    if (data_radar_rec == 1) {
       drawRadar(ang,dist);
      }
    if (data_giros_rec == 1) {
      //display_datos_giros();
      }
    loop_websockets();      
    }
}

