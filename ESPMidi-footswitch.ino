#include <Arduino_Helpers.h>
#include <AH/Hardware/Button.hpp>
#include <AH/Hardware/MultiPurposeButton.hpp>
#include <AH/Hardware/FilteredAnalog.hpp>

#include <WiFi.h>
#include <WebServer.h>

#include "webServer/webServerSetup.h"
#include "midi/midiSetup.h"

TaskHandle_t DisplayTask0;
TaskHandle_t WebServerTask0;

void setup() {
  xTaskCreatePinnedToCore(displayloop, "DisplayTask_0", 4096, NULL, 1, &DisplayTask0, 0);
  xTaskCreatePinnedToCore(serverloop, "WebServerTask0", 4096, NULL, 1, &WebServerTask0, 0);
  Serial.begin(115200);
  Serial.println(ESP.getFlashChipSize());

  // AP Mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  // Serial.print("Conectado a WiFi, IP: ");
  // Serial.println(WiFi.localIP());

  // Configurar rutas del servidor
  server.on("/", handleRoot);
  server.on("/setBank", handleSetBank);
  server.on("/editBank", handleEditBank);  // Nueva ruta para editar banco
  server.on("/saveBank", handleSaveBank);  // Nueva ruta para guardar banco
  server.begin(); // Iniciar servidor web
  // Serial.println("Servidor iniciado");

  pinMode(ledPin, OUTPUT);
  pushbutton.begin();  // Inicia el botón pushbutton

  // Inicializar LEDs indicadores de bancos
  for (pin_t pin : bankLeds) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Apagar todos los LEDs inicialmente
  }

  configureButton(btn1);
  configureButton(btn2);

  // Select the correct ADC resolution
  analog1.setupADC();
  analog2.setupADC();
  analog3.setupADC();
  analog4.setupADC();
  // Initialize the filter to whatever the value on the input is right now
  // (otherwise, the filter is initialized to zero and you get transients)
  analog1.resetToCurrentValue();
  analog2.resetToCurrentValue();
  analog3.resetToCurrentValue();
  analog4.resetToCurrentValue();

  midi.begin(); // Inicializar la interfaz MIDI

  updateBankIndicators(); // Update leds at startup
}

using ValueArray = uint8_t[2];
void handleButton(MultiPurposeButton &button, const MIDIAddress &shortPressAddr,
                  ValueArray shortPressValues, const MIDIAddress &longPressAddr,
                  ValueArray longPressValues, bool &shortPressStatus,
                  bool &longPressStatus, bool bankCycle) {
  switch (button.update()) {
  case button.None:
    break;
  case button.PressStart:
    break;
  case button.ShortPressRelease:
    shortPressStatus = !shortPressStatus;
    midi.sendCC(shortPressAddr,
                shortPressStatus ? shortPressValues[1] : shortPressValues[0]);
    break;
  case button.LongPress:
    if (bankCycle) {
      currentBank =
          (currentBank + 1) %
          (sizeof(banks) / sizeof(banks[0])); // Cambiar al siguiente banco
      updateBankIndicators(); // Actualizar LEDs para reflejar el banco actual
      fastBlink(ledPin, internalLedState);
    } else {
      internalLedState = !internalLedState; // Cambiar el estado del LED
      longPressStatus = !longPressStatus;
      midi.sendCC(longPressAddr,
                  longPressStatus ? longPressValues[1] : longPressValues[0]);
      digitalWrite(ledPin, internalLedState ? HIGH : LOW);
    }
    break;
  }
}

// Función para manejar el botón pushbutton
void handlePushButton() {
  if (pushbutton.update() == Button::Falling) {
    midi.sendCC(banks[currentBank].btn2ShortPressAddress,
                banks[currentBank].btn2ShortPressValues[1]);
  }
}

void handlePot(
    AH::FilteredAnalog<7, 4, uint32_t, analog_t> &analog,
    int CCNumber) {
  if (analog.update())
    midi.sendCC({CCNumber, Channel_1}, analog.getValue());
}

void loop() {
  handlePushButton();

  handleButton(btn1, 
                banks[currentBank].btn1ShortPressAddress, banks[currentBank].btn1ShortPressValues, 
                banks[currentBank].btn1LongPressAddress, banks[currentBank].btn1LongPressValues, 
                banks[currentBank].btn1ShortPressStatus, banks[currentBank].btn1LongPressStatus,
                false);

  handleButton(btn2, 
                banks[currentBank].btn2ShortPressAddress, banks[currentBank].btn2ShortPressValues, 
                banks[currentBank].btn2LongPressAddress, banks[currentBank].btn2LongPressValues, 
                banks[currentBank].btn2ShortPressStatus, banks[currentBank].btn2LongPressStatus,
                true);

  handlePot(analog1, 91);
  handlePot(analog2, 92);
  handlePot(analog3, 93);
  handlePot(analog4, 94);

  midi.update(); // Manejar o descartar la entrada MIDI
}

void displayloop(void *parameter) {
  while (true) {
    delay(300);
  }
  vTaskDelay(10);
}

void serverloop(void *parameter) {
  while (true) {
    server.handleClient();
  }
  vTaskDelay(10);
}

// Función para configurar un MultiPurposeButton
void configureButton(MultiPurposeButton &button) {
  button.setLongPressDelay(600);
  button.setMultiPressDelay(400);
  button.begin();
}

void fastBlink(pin_t pin, bool state) {
    digitalWrite(pin, !state);
    delay(100);
    digitalWrite(pin, state);
}

constexpr uint8_t numBanks = sizeof(bankLeds)/sizeof(bankLeds[0]);

void updateBankIndicators() {
    for (uint8_t i = 0; i < numBanks; i++) {
        digitalWrite(bankLeds[i], i == currentBank ? HIGH : LOW);
    }
}

// Función para manejar la página principal
void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }";
  html += "h1 { color: #333; }";
  html += "form { display: inline-block; padding: 20px; background-color: #f9f9f9; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
  html += "label { font-size: 1.2em; }";
  html += "select { padding: 10px; font-size: 1em; margin: 10px 0; }";
  html += "input[type=submit] { padding: 10px 20px; font-size: 1em; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }";
  html += "input[type=submit]:hover { background-color: #45a049; }";
  html += "</style>";
  html += "<meta charset=\"UTF-8\">";
  html += "</head><body>";
  html += "<h1>Configuración de Bancos MIDI 🎛️</h1>";
  html += "<form action=\"/setBank\" method=\"GET\">";
  html += "<label for=\"bank\">Selecciona Banco:</label><br>";
  html += "<select name=\"bank\">";
  for (int i = 0; i < sizeof(banks)/sizeof(banks[0]); i++) {
    html += "<option value=\"" + String(i) + "\">Banco " + String(i + 1) + "</option>";
  }
  html += "</select><br>";
  html += "<input type=\"submit\" value=\"Establecer Banco\">";
  html += "</form><br><br>";
  html += "<a href=\"/editBank?bank=" + String(currentBank) + "\"><button>Editar Banco Actual</button></a>";  // Botón para editar el banco actual
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Función para manejar la configuración del banco actual
void handleSetBank() {
  String bankStr = server.arg("bank");
  int bank = bankStr.toInt();

  if (bankStr.length() == 0 || !isDigit(bankStr[0]) || bank < 0 || bank >= numBanks) {
      server.send(400, "text/plain", "Valor de banco no válido o fuera de rango");
  } else {
      currentBank = bank;
      updateBankIndicators();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleEditBank() {
  if (server.hasArg("bank")) {
    uint8_t bankIndex = currentBank;
    String html = "<html><head>";
    html += "<meta charset=\"UTF-8\">";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }";
    html += "h1 { color: #333; }";
    html += "form { display: inline-block; padding: 20px; background-color: #f9f9f9; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
    html += "label { font-size: 1.2em; }";
    html += "input[type=text], input[type=number] { padding: 10px; font-size: 1em; margin: 10px 0; width: 100%; }";
    html += "input[type=submit] { padding: 10px 20px; font-size: 1em; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }";
    html += "input[type=submit]:hover { background-color: #45a049; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Editar Banco " + String(bankIndex + 1) + "</h1>";
    html += "<form action=\"/saveBank\" method=\"POST\">";

    // // Botón 1 Short Press Address
    // html += "<label for=\"btn1_short_addr\">Dirección Short Press Botón 1:</label>";
    // html += "<input type=\"number\" name=\"btn1_short_addr\" value=\"" + String(banks[bankIndex].btn1ShortPressAddress.getAddress()) + "\"><br>";

    // Botón 1 Short Press Values
    html += "<label for=\"btn1_short_value0\">Valor Short Press Botón 1 (Estado 0):</label>";
    html += "<input type=\"number\" name=\"btn1_short_value0\" value=\"" + String(banks[bankIndex].btn1ShortPressValues[0]) + "\"><br>";
    html += "<label for=\"btn1_short_value1\">Valor Short Press Botón 1 (Estado 1):</label>";
    html += "<input type=\"number\" name=\"btn1_short_value1\" value=\"" + String(banks[bankIndex].btn1ShortPressValues[1]) + "\"><br>";

    // // Botón 1 Long Press Address
    // html += "<label for=\"btn1_long_addr\">Dirección Long Press Botón 1:</label>";
    // html += "<input type=\"number\" name=\"btn1_long_addr\" value=\"" + String(banks[bankIndex].btn1LongPressAddress.getAddress()) + "\"><br>";

    // Botón 1 Long Press Values
    html += "<label for=\"btn1_long_value0\">Valor Long Press Botón 1 (Estado 0):</label>";
    html += "<input type=\"number\" name=\"btn1_long_value0\" value=\"" + String(banks[bankIndex].btn1LongPressValues[0]) + "\"><br>";
    html += "<label for=\"btn1_long_value1\">Valor Long Press Botón 1 (Estado 1):</label>";
    html += "<input type=\"number\" name=\"btn1_long_value1\" value=\"" + String(banks[bankIndex].btn1LongPressValues[1]) + "\"><br>";

    // // Botón 2 Short Press Address
    // html += "<label for=\"btn2_short_addr\">Dirección Short Press Botón 2:</label>";
    // html += "<input type=\"number\" name=\"btn2_short_addr\" value=\"" + String(banks[bankIndex].btn2ShortPressAddress.getAddress()) + "\"><br>";

    // Botón 2 Short Press Values
    html += "<label for=\"btn2_short_value0\">Valor Short Press Botón 2 (Estado 0):</label>";
    html += "<input type=\"number\" name=\"btn2_short_value0\" value=\"" + String(banks[bankIndex].btn2ShortPressValues[0]) + "\"><br>";
    html += "<label for=\"btn2_short_value1\">Valor Short Press Botón 2 (Estado 1):</label>";
    html += "<input type=\"number\" name=\"btn2_short_value1\" value=\"" + String(banks[bankIndex].btn2ShortPressValues[1]) + "\"><br>";

    // // Botón 2 Long Press Address
    // html += "<label for=\"btn2_long_addr\">Dirección Long Press Botón 2:</label>";
    // html += "<input type=\"number\" name=\"btn2_long_addr\" value=\"" + String(banks[bankIndex].btn2LongPressAddress.getAddress()) + "\"><br>";

    // Botón 2 Long Press Values
    html += "<label for=\"btn2_long_value0\">Valor Long Press Botón 2 (Estado 0):</label>";
    html += "<input type=\"number\" name=\"btn2_long_value0\" value=\"" + String(banks[bankIndex].btn2LongPressValues[0]) + "\"><br>";
    html += "<label for=\"btn2_long_value1\">Valor Long Press Botón 2 (Estado 1):</label>";
    html += "<input type=\"number\" name=\"btn2_long_value1\" value=\"" + String(banks[bankIndex].btn2LongPressValues[1]) + "\"><br>";

    html += "<input type=\"hidden\" name=\"bank\" value=\"" + String(bankIndex) + "\">";
    html += "<input type=\"submit\" value=\"Guardar\">";
    html += "</form>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  } else {
    server.send(400, "text/plain", "Falta el parámetro 'bank'");
  }
}

void handleSaveBank() {
  if (server.hasArg("bank")) {
    uint8_t bankIndex = server.arg("bank").toInt();

    // // Botón 1 Short Press Address
    // banks[bankIndex].btn1ShortPressAddress.MIDIAddress = server.arg("btn1_short_addr").toInt();

    // Botón 1 Short Press Values
    banks[bankIndex].btn1ShortPressValues[0] = server.arg("btn1_short_value0").toInt();
    banks[bankIndex].btn1ShortPressValues[1] = server.arg("btn1_short_value1").toInt();

    // // Botón 1 Long Press Address
    // banks[bankIndex].btn1LongPressAddress[0] = server.arg("btn1_long_addr").toInt();

    // Botón 1 Long Press Values
    banks[bankIndex].btn1LongPressValues[0] = server.arg("btn1_long_value0").toInt();
    banks[bankIndex].btn1LongPressValues[1] = server.arg("btn1_long_value1").toInt();

    // // Botón 2 Short Press Address
    // banks[bankIndex].btn2ShortPressAddress[0] = server.arg("btn2_short_addr").toInt();

    // Botón 2 Short Press Values
    banks[bankIndex].btn2ShortPressValues[0] = server.arg("btn2_short_value0").toInt();
    banks[bankIndex].btn2ShortPressValues[1] = server.arg("btn2_short_value1").toInt();

    // // Botón 2 Long Press Address
    // banks[bankIndex].btn2LongPressAddress[0] = server.arg("btn2_long_addr").toInt();

    // Botón 2 Long Press Values
    banks[bankIndex].btn2LongPressValues[0] = server.arg("btn2_long_value0").toInt();
    banks[bankIndex].btn2LongPressValues[1] = server.arg("btn2_long_value1").toInt();

    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(400, "text/plain", "Falta el parámetro 'bank'");
  }
  fastBlink(ledPin, internalLedState);
}
