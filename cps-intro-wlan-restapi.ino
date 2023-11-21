#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ssid = "...";
const char* password = "...";

#define PIN_LED_1 13
 
WebServer server(80);
int pastStatus = LOW;

void toggleLED() {

  int newStatus = LOW;
  if (pastStatus == LOW) {
    newStatus = HIGH;
  }
  pastStatus = newStatus;
  
  digitalWrite(PIN_LED_1, newStatus);
  server.send(200, "application/json", String(newStatus));
  
}
 
// Manage not found URL => HTTP error 404
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

//hier wird wird der ESP mit dem WLAN verbunden
void connectWiFi(){
  
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to " + String(ssid));

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    WiFi.begin(ssid, password); 
    delay(5000);     
  }

  Serial.println("\nConnected.");  
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
}

void setup(void) {
  
  Serial.begin(115200);
  pinMode(PIN_LED_1, OUTPUT);

  connectWiFi();
 
  // bei HTTP-Aufruf der URL /toggleLED Ausführen der Funktion toggleLED
  server.on(F("/toggleLED"), HTTP_GET, toggleLED); 
  
  // falls URL nicht gefunden, Fehlebehandlung durch die Funktion handleNotFound
  server.onNotFound(handleNotFound);
  
  // start server
  server.begin();
  Serial.println("HTTP server started");
  
}
 
void loop(void) {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }  

  server.handleClient();
}
