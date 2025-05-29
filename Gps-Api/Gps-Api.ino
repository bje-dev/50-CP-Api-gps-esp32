#include <WiFi.h>
#include <WebServer.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;  // Objeto TinyGPS++
HardwareSerial ss(1);

const char* ssid = "NETWORK";        
const char* password = "PASSWORD"; 

WebServer server(80); 

void setup() {
  Serial.begin(115200);
  ss.begin(9600, SERIAL_8N1, 16, 17); // RX en GPIO16, TX en GPIO17

  Serial.println("📡 GPS Data API JSON");

  // Conectar a la red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Conexión establecida");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configurar rutas del servidor
  server.on("/gps", HTTP_GET, handleGPSData);
  server.begin();
  Serial.println("🌐 Servidor iniciado");
}

void loop() {
  server.handleClient(); 

  // GPS Data
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

// API JSON
void handleGPSData() {
  if (gps.location.isValid()) {
    String json = "{";
    json += "\"latitude\":" + String(gps.location.lat(), 6) + ",";
    json += "\"longitude\":" + String(gps.location.lng(), 6) + ",";
    json += "\"altitude\":" + String(gps.altitude.meters(), 2) + ",";
    json += "\"satellites\":" + String(gps.satellites.value()) + ",";
    json += "\"speed\":" + String(gps.speed.kmph(), 2) + ",";
    json += "\"time\":\"" + formatTime(gps.time.hour() - 3, gps.time.minute(), gps.time.second()) + "\",";
    json += "\"accuracy\":" + String(gps.hdop.value());
    json += "}";
    server.send(200, "application/json", json);
  } else {
    server.send(503, "application/json", "{\"error\":\"GPS signal not valid\"}");
  }
}

// Función para formatear la hora
String formatTime(int hour, int minute, int second) {
  String time = String(hour < 10 ? "0" : "") + String(hour) + ":" +
                String(minute < 10 ? "0" : "") + String(minute) + ":" +
                String(second < 10 ? "0" : "") + String(second);
  return time;
}

