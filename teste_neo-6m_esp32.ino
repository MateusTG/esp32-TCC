#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <HardwareSerial.h>

//COnfigurando o WiFi
const char* ssid = "MATEUS-PC";
const char* password = "12345678";
const char* serverIP = "192.168.137.1";
const int serverPort = 5001; // Substitua pela porta correta do servidor Node.js

WiFiClient client;

//Configurações do GPS
TinyGPSPlus gps;

String idESP;
float latitude, longitude, velocidade; // Velocidade em km/h
uint8_t dia; //Data DD/MM/AAAA
uint8_t mes;
uint16_t ano;
uint8_t hora; // Hora no formato "HH:MM:SS"
uint8_t minuto;
uint8_t segundos;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);//Usa os pinos da Serial 2 do ESP32(RX:16 e TX:17)

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Conecta-se ao servidor
  if (client.connect(serverIP, serverPort)) {
    
    Serial.println("Connected to server!");
  } else {
    Serial.println(serverIP);
    Serial.println(serverPort);
    
    Serial.println("Connection failed.");
  }
}

void loop() {
  while (Serial2.available() > 0) {
    char gpsData = Serial2.read();
    //Serial.print(gpsData);
    if (gps.encode(gpsData)) {
      module_gps();
      if (client.connected()) {
        String jsonData = module_gps();
        client.println(jsonData);
        Serial.println("Data sent to server: " + jsonData);
      } else {
        Serial.println("Connection lost. Reconnecting...");
        // Reconecta ao servidor, se a conexão foi perdida
        if (client.connect(serverIP, serverPort)) {
          Serial.println("Connected to server!");
        } else {
          Serial.println("Connection failed.");
        }
      }
      delay(3000);
    } 
  } 
  delay(3000);
}

String module_gps(){
    Serial.println("");
    Serial.println("Recebendo dados do GPS...");
    if (gps.location.isValid()) {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    }else{
      Serial.println(F("Dados da localização não encontrados"));  
    }
    if(gps.speed.isValid()){
      velocidade = gps.speed.kmph();
    }else{
      Serial.println(F("Dados de velocidade não encontrados"));  
    }
    if(gps.date.isValid()){
      dia = gps.date.day(); //Data DD/MM/AAAA
      mes = gps.date.month();
      ano = gps.date.year();
    }else{
      Serial.println(F("Dados da data não encontrados"));
    }
    if(gps.time.isValid()){
      hora = gps.time.hour()-3; // Hora no formato "HH:MM:SS"
      minuto = gps.time.minute();
      segundos = gps.time.second();
    }else{
      Serial.println(F("Dados da hora não encontrados"));
    }
    idESP = "\"Esp32\"";
    String jsonData = "{\"idESP\":" + String(idESP) + ",\"latitude\":" + String(latitude, 6) + 
    ",\"longitude\":" + String(longitude, 6) + 
    ",\"speed\":" + String(velocidade, 2) + 
    ",\"date\":\"" + ano + "-" + mes + "-" + dia + 
    "\",\"time\":\"" + hora + ":" + minuto + ":" + segundos + "\"}";
    Serial.println(jsonData);
    return jsonData;
} 
