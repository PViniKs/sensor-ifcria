// Importação das Bibliotecas
#include <DHT.h>
#include <HTTP_Method.h>
#include <WebServer.h>
#include <WiFi.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ThingsBoard.h>

// Definir os dados do Wi-Fi para o ESP32 se conectar
#define ssid "Nome da Rede Wi-Fi"
#define password "Senha da Rede Wi-Fi"

// Definir o tipo dos DHTs como sendo DHT11
#define DHTTYPE DHT11

// Constantes para armazenar as portas nas quais os DHTs estão conectados
const int DHTPIN = 4;
const int DHTPIN2 = 5;

// Constantes para acesso ao ThingsBoard
const char* thingsboardServer = "http://geati.camboriu.ifc.edu.br:8083";
const char* accessToken = "Token de Acesso do ThingsBoard";
const char* deviceId = "Id do Dispositivo";

// Definir os 2 DHTs, cada um em sua porta
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// Variáveis para armazenar os valores de temperatura e umidade, internos e externos
float h;
float h2;
float t;
float t2;
// Variáveis para armazenar os Índices de Calor, cálculo realizado pela própria biblioteca do DHT
float hic = dht.computeHeatIndex(t, h, false);
float hic2 = dht2.computeHeatIndex(t2, h2, false);

// Setup do programa
void setup() {
  // Inicializa a porta Serial com uma taxa de dados de 9600bps (Bits Por Segundo)
  Serial.begin(9600);

  // Declara os pinos dos 2 DHTs como sendo de entrada de dados
  pinMode(DHTPIN, INPUT);
  pinMode(DHTPIN, INPUT);

  // Inicializa os DHTs
  dht.begin();
  dht2.begin();

  // Tenta conectar à Rede Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Não continua até que conecte
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

// Loop do programa
void loop() {
  // Executa a função "sensorDHT"
  sensorDHT();
 
  // Cria um JSON com os dados a serem enviados
  String payload = "{\"temperatura\":" + String(t) +
                    ",\"humidade\":" + String(h) +
                    ",\"calor\":" + String(hic) +
                    ",\"temperatura2\":" + String(t2) +
                    ",\"humidade2\":" + String(h2) +
                    ",\"calor2\":" + String(hic2) + "}";

  // Cria a URL para enviar os dados para o ThingsBoard
  String url = String(thingsboardServer) + "/api/v1/" + accessToken + "/telemetry";  

  // Cria um cliente HTTP e envia os dados utilizando o método POST HTTP por meio da biblioteca HTTPClient;
  HTTPClient http;  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  // Encerra o cliente HTTP
  http.end();

  // Espera 5 minutos para repetir o loop
  delay(5*60*1000);
}

// Função para leitura dos dados dos DHTs
void sensorDHT(){
  // Lê as umidades interna e externa
  h = dht.readHumidity();
  h2 = dht2.readHumidity();

  // Lê as temperaturas interna e externa
  t = dht.readTemperature();
  t2 = dht2.readTemperature();

  // Calcula os índices de calor interno e externo
  hic = dht.computeHeatIndex(t, h, false);
  hic2 = dht2.computeHeatIndex(t2, h2, false);
}
