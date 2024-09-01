// // Configuración Wi-Fi STA
// const char* ssid = "ssid";
// const char* password = "password";

// Configuración Wi-Fi AP
const char *ssid = "ArduMIDI config"; // Enter SSID here
const char *password = "Papita34";    // Enter Password here

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Crear instancia del servidor web
WebServer server(80);
