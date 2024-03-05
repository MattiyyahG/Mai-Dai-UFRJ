#include <ESP8266WiFi.h>

#include <PubSubClient.h>

const char* ssid = "wPESC-Visitante"; // Wifi

const char* password = ""; //Senha

const char* mqtt_server = "test.mosquitto.org"; //Broker


WiFiClient espClient; //Configurações do cliente

PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE	(50)  //Definindo o tamanho da mensagem que pode chegar no broker, só aumentar dps

char msg[MSG_BUFFER_SIZE];

int value = 0;


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

      client.subscribe(""); // Tópico definido

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

}

void loop() {

  if (!client.connected()) {

    reconnect();

  }

  client.loop();

  unsigned long now = millis();

  if (now - lastMsg > 2000) {

    lastMsg = now;

    snprintf (msg, MSG_BUFFER_SIZE, " ", ); // <-------------------- Mensagem que será enviada para o broker 

    Serial.println(msg);

    client.publish("", msg);

    delay(1000);

  }

}
