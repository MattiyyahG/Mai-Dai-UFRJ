#include <WiFi.h>

#include <PubSubClient.h>

#include <DHTesp.h>

const char* ssid = "Wokwi-GUEST"; // Wifi

const char* password = ""; //Senha

const char* mqtt_server = "test.mosquitto.org"; //Broker

const int DHT_PIN = 15;

DHTesp dhtSensor;

WiFiClient espClient; //Configurações do cliente

PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE	(100)  //Definindo o tamanho da mensagem que pode chegar no broker, só aumentar dps

char msg[MSG_BUFFER_SIZE];

int value = 0;

float umi = 0.0;


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

      client.subscribe("lab/net"); // Tópico definido

    } else {

      Serial.print("Falha...");

      Serial.print(client.state());

      delay(5000);

    }

  }

}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

}

void loop() {

  if (!client.connected()) {

    reconnect();

  }

  client.loop();

  unsigned long now = millis();

  TempAndHumidity  data = dhtSensor.getTempAndHumidity();

  int umidade = data.humidity;

  delay(2000);


  if (now - lastMsg > 2000) {

    lastMsg = now;

    snprintf (msg, MSG_BUFFER_SIZE, "Umidade: %ld", umidade); // <-------------------- Mensagem que será enviada para o broker

    Serial.println(msg);

    client.publish("lab/net", msg);

    delay(1000);

  }

}
