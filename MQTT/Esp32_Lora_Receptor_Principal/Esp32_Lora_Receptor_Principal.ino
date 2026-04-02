#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// -------- WIFI --------
const char* ssid     = "Lab_3D";
const char* password = "l4bn3t00";

// -------- MQTT (RabbitMQ) --------
const char* mqtt_server = "guppy.rmq6.cloudamqp.com"; // IP do servidor RabbitMQ
const int   mqtt_port   = 1883;

const char* mqtt_user = "zdvyjqvf:zdvyjqvf";
const char* mqtt_pass = "cQCgsy5mnP3vb2GjSseM3mUHIg3KunCB";

const char* mqtt_topic = "FAZENDA";

// -------- NTP --------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 0;

// -------- LoRa --------
SX1262 radio = new Module(8, 14, 12, 13);

// -------- OLED --------
#define OLED_SDA  17
#define OLED_SCL  18
#define OLED_RST  21
#define VEXT      36
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

// -------- MQTT --------
WiFiClient espClient;
PubSubClient client(espClient);

// -------- Variáveis --------
float umidade, temperatura, ph;
int condutividade, nitrogenio, fosforo, potassio;

String dataAtual = "";
String horaAtual = "";

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // OLED
  pinMode(VEXT, OUTPUT);
  digitalWrite(VEXT, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Inicializando...");
  display.display();

  // WiFi
  conectarWiFi();

  // NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // MQTT
  client.setServer(mqtt_server, mqtt_port);

  // LoRa
  SPI.begin(9, 11, 10, 8);
  int estado = radio.begin(915.0);

  if (estado != RADIOLIB_ERR_NONE) {
    Serial.println("Erro LoRa");
    while (true);
  }

  radio.setSpreadingFactor(7);
  radio.setBandwidth(125.0);

  display.clearDisplay();
  display.println("Sistema pronto!");
  display.display();
}

// ================= LOOP =================
void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();

  String mensagem;

  int estado = radio.receive(mensagem);

  if (estado == RADIOLIB_ERR_NONE) {
    Serial.println("Recebido:");
    Serial.println(mensagem);

    processarJSON(mensagem);
    atualizarDataHora();
    mostrarOLED();
    enviarRabbitMQ();
  }
}

// ================= WIFI =================
void conectarWiFi() {
  Serial.print("Conectando WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("OK!");
}

// ================= MQTT =================
void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");

    if (client.connect("ESP32_LoRa", mqtt_user, mqtt_pass)) {
      Serial.println("OK!");
    } else {
      Serial.print("Erro: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ================= JSON RX =================
void processarJSON(String msg) {
  StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, msg);

  if (error) {
    Serial.println("Erro JSON");
    return;
  }

  umidade       = doc["umidade"];
  temperatura   = doc["temperatura"];
  condutividade = doc["condutividade"];
  ph            = doc["ph"];
  nitrogenio    = doc["nitrogenio"];
  fosforo       = doc["fosforo"];
  potassio      = doc["potassio"];
}

// ================= NTP =================
void atualizarDataHora() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Erro NTP");
    return;
  }

  char dataStr[11];
  char horaStr[9];

  strftime(dataStr, sizeof(dataStr), "%d/%m/%Y", &timeinfo);
  strftime(horaStr, sizeof(horaStr), "%H:%M:%S", &timeinfo);

  dataAtual = String(dataStr);
  horaAtual = String(horaStr);
}

// ================= OLED =================
void mostrarOLED() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("U:"); display.print(umidade, 1);
  display.print(" T:"); display.print(temperatura, 1);

  display.setCursor(0, 12);
  display.print("Cond:"); display.println(condutividade);

  display.setCursor(0, 24);
  display.print("pH:"); display.println(ph, 1);

  display.setCursor(0, 36);
  display.print("N:"); display.print(nitrogenio);
  display.print(" P:"); display.print(fosforo);
  display.print(" K:"); display.println(potassio);

  display.setCursor(0, 52);
  display.print(dataAtual);
  display.print(" ");
  display.print(horaAtual);

  display.display();
}

// ================= ENVIO RABBITMQ =================
void enviarRabbitMQ() {
  StaticJsonDocument<256> doc;
  int id_sensor = 1;
  doc["id_sensor"] = id_sensor;
  doc["data"] = dataAtual;
  doc["hora"] = horaAtual;
  doc["umidade"] = umidade;
  doc["temperatura_solo"] = temperatura;
  doc["condutividade"] = condutividade;
  doc["ph"] = ph;
  doc["nitrogenio"] = nitrogenio;
  doc["fosforo"] = fosforo;
  doc["potassio"] = potassio;

  String jsonFinal;
  serializeJson(doc, jsonFinal);

  Serial.println("Enviando MQTT:");
  Serial.println(jsonFinal);

  client.publish(mqtt_topic, jsonFinal.c_str(), true); // retain opcional
}
