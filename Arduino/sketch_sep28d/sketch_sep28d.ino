#include "ESP8266WiFi.h"


const char* ssid = "UPCCF552C7";
const char* password = "Ebv8mcuaf4cc";

WiFiServer server(80);
WiFiClient client;
WiFiClient httpClient;

// Poprzednio zmierzony czas
unsigned long prevTime = micros();
  
void setup() 
{
  Serial.begin(115200);
  Serial.println(ESP.getChipId(), HEX);
  Serial.println(ESP.getFlashChipId(), HEX);

  Serial.println(ESP.getFlashChipSize());
  Serial.println(ESP.getFlashChipSpeed());

  WiFi.begin(ssid, password);

  while( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFI connected. Adres IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

  pinMode(D5, OUTPUT);
}

void loop() {
  
  String command = handleCommand();
  
  if( command == "power" ) {
    togglePower();
  }
  
  if( command == "status" ) {
    recvStatus();
  }

  bringServer();
}

/**
 *  Obsługa przestawiania mocy diody
 */
void togglePower()
{
      if( digitalRead(D5) == HIGH ) {
      digitalWrite(D5, LOW);      
    } else {
      digitalWrite(D5, HIGH);
    }
    
    String response = "{\"status\": \"ok\"}";
    
    Serial.println(response);
    client.println(response);
}

/**
 *  Zwrócenie statusu diody
 */
void recvStatus()
{
    String response = "{\"status\": \"ok\", \"data\": " + String(digitalRead(D5)) + String("}");
    
    Serial.println(response);
    client.println(response);
}

void bringServer()
{
  // Wyślij co 30 sekund informację o działaniu klienta
  if( abs(micros() - prevTime) < 1000000 * 30 ) {
    return;
  }
  
  if( httpClient.connect("192.168.0.157", 80) ) {
      Serial.println("Connected to server");
      httpClient.println("GET /SmartHome/server.php HTTP/1.0");
      httpClient.println();
  } else {
      Serial.println("Server is alone");
  }

  prevTime = micros(); 
}

/**
 *  Uzyskanie komendy 
 *
 * @return NULL - Brak nadesłanej komendy, String - Komenda
 */
String handleCommand()
{
  client = server.available();

  if( !client ) {
    return "";
  }
  
    String request = "";
    Serial.println("new client");

    while( client.connected() ) 
  {
    if( !client.available()) {
      continue;
    }
    
    char c = client.read();
  
        if( c == '\n' ) {
      Serial.println("Read value: " + request);
      return request;
    }
    
    request += c;
    }
  
  return "";
}
