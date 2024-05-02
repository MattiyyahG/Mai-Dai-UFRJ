//sudo chmod a+rw /dev/ttyUSB0
#include <ESP8266WiFi.h>

#include <Wire.h>

#include <PubSubClient.h>

//#include <FS.h>

#include <LittleFS.h>

#include <RTClib.h>

const char* ssid = "LENS-ESE"; // Wifi

const char* password = "LensESE*789"; //Senha

const char* mqtt_server = "test.mosquitto.org"; //Broker


WiFiClient espClient; //Configurações do cliente

PubSubClient client(espClient);

RTC_DS3231 rtc;

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE	(500)  //Definindo o tamanho da mensagem que pode chegar no broker, só aumentar dps

char msg[MSG_BUFFER_SIZE];

int failCount = 0; // Contador de falhas

void setup_wifi() {

  delay(10);

  Serial.println();

  Serial.print("Conectando a ");

  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  randomSeed(micros());

  Serial.println("");

  Serial.println("Conectado!");

  Serial.println("Endereço IP ");

  Serial.println(WiFi.localIP());

}

void callback(char* topic, byte* payload, unsigned int length) { //Toda vez que uma mensagem chegar no broker, sera enviado uma mensagem de confirmação no serial e em qual tópico chegou, pode remover sem nenhum problema

  Serial.print("Mensagem Recebida no tópico [");

  Serial.print(topic);

  Serial.print("] ");

  for (int i = 0; i < length; i++) {

    Serial.print((char)payload[i]);

  }

  Serial.println();

}

void reconnect() { // Função para reconexão de rede


  while (!client.connected()) {

    Serial.print("...");

    String clientId = "ESP8266Client-"; //Id do usuario, mudar conforme necessário

    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("Conectado!");

      client.subscribe("MaiDai/Uva"); // Tópico definido

      failCount = 0; // Resetar o contador de falhas após uma conexão bem sucedida

    } else {

      Serial.print("Falha...");

      Serial.print(client.state());

      failCount++; // Incrementar o contador de falhas

      /*if (failCount >= 3) { // Se falhou 3 vezes

        if (!LittleFS.begin()) {

          Serial.println("Falha ao montar o sistema de arquivos LittleFS");

          return;

        }

        File file = LittleFS.open("/data_log.txt", "a"); // Abrir o arquivo em modo de anexação

        if (!file) {

          Serial.println("Falha ao abrir o arquivo para escrita");

          return;

        }

        if (file.println(msg)) { // Escrever no arquivo

          Serial.println("Dados escritos com sucesso");

        } else {

          Serial.println("Falha na escrita");

        }

        file.close();

        failCount = 0; // Resetar o contador de falhas após a escrita bem sucedida

      }*/

      delay(5000);

    }

  }

}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) {
    
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
}

void loop() {
  
  DateTime now = rtc.now();

  if (!client.connected()) {

  reconnect();

  }

  client.loop();
//100.0 1 2024-05-02 13:49:32
  int ano = now.year();
  int mes = now.month();
  int dia = now.day();

  int hor = now.hour();
  int min = now.minute();
  int seg = now.second();

  String data = String(ano) + "-" + String(mes) + "-" + String(dia);
  String hora = String(hor) + ":" + String(min) + ":" + String(seg);


  String coleta_d = data;

  String coleta_h = hora;

  unsigned long now_a = millis();

  float umidade;

  int sensor_a;

  sensor_a = analogRead(A0);
  
  umidade = ( 100 - ( ( sensor_a / 1023.00 ) * 100 ) );

  delay(1000);


  if (now_a - lastMsg > 2000) {

    lastMsg = now_a;

    snprintf (msg, MSG_BUFFER_SIZE, "%.1f %d %s %s", umidade, 1, coleta_d, coleta_h); // <-------------------- Mensagem que será enviada para o broker

    Serial.println(msg);

    client.publish("MaiDai/Uva", msg);

    if (failCount >= 3) {

      if (!LittleFS.begin()) {

        Serial.println("Falha ao montar o sistema de arquivos LittleFS");

        return;

      }

      File file = LittleFS.open("/data_log.txt", "a");

      if (!file) {

        Serial.println("Falha ao abrir o arquivo para escrita");

        return;

      }

      file.println(msg);

      file.close();

      Serial.println("Dados salvos no LittleFS");

      failCount = 0;

    }

    delay(1000);

  }

}
