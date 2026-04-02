/*
  Heltec WiFi LoRa 32 V3
  - Recebe dados do Pro Mini via Serial1 (GPIO1)
  - Monta JSON
  - Exibe no OLED
  - Envia via LoRa 915MHz
  
  Bibliotecas necessárias:
  - RadioLib
  - Adafruit SSD1306
  - Adafruit GFX
*/

#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- LoRa SX1262 ---
SX1262 radio = new Module(8, 14, 12, 13); // NSS, DIO1, RST, BUSY

// --- OLED ---
#define OLED_SDA  17
#define OLED_SCL  18
#define OLED_RST  21
#define VEXT      36
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

// --- Serial1: recebe do Pro Mini ---
#define PIN_RX1 1

// --- Variaveis dos sensores ---
float    umidade, temperatura, ph;
int      condutividade, nitrogenio, fosforo, potassio;
String   dataHora = "";

void setup() {
  Serial.begin(115200);

  // Liga alimentação do OLED via VEXT
  pinMode(VEXT, OUTPUT);
  digitalWrite(VEXT, LOW); // LOW = liga no Heltec V3

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  delay(100);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha no OLED");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando LoRa...");
  display.display();

  // LoRa
  SPI.begin(9, 11, 10, 8); // SCK, MISO, MOSI, NSS
  int estado = radio.begin(915.0); // 915MHz — Brasil
  if (estado != RADIOLIB_ERR_NONE) {
    Serial.print("Falha LoRa: ");
    Serial.println(estado);
    display.println("Falha LoRa!");
    display.display();
    while (true);
  }
  radio.setOutputPower(14);    // 14 dBm — ajuste conforme necessário
  radio.setSpreadingFactor(7); // SF7 — bom equilíbrio velocidade/alcance
  radio.setBandwidth(125.0);

  // Serial1 — recebe do Pro Mini
  Serial1.begin(4800, SERIAL_8N1, PIN_RX1, -1);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LoRa OK - 915MHz");
  display.println("Aguardando dados...");
  display.display();

  Serial.println("Heltec V3 pronto!");
}

void loop() {
  if (Serial1.available()) {
    String linha = Serial1.readStringUntil('\n');
    linha.trim();

    if (linha.startsWith("DATA|")) {
      // Extrai valores
      umidade       = getValue(linha, '|', 1).toFloat();
      temperatura   = getValue(linha, '|', 2).toFloat();
      condutividade = getValue(linha, '|', 3).toInt();
      ph            = getValue(linha, '|', 4).toFloat();
      nitrogenio    = getValue(linha, '|', 5).toInt();
      fosforo       = getValue(linha, '|', 6).toInt();
      potassio      = getValue(linha, '|', 7).toInt();

      // Monta JSON
      String json = "{";
      json += "\"data_hora\":\"" + dataHora + "\",";
      json += "\"umidade\":"       + String(umidade, 1)      + ",";
      json += "\"temperatura\":"   + String(temperatura, 1)  + ",";
      json += "\"condutividade\":" + String(condutividade)    + ",";
      json += "\"ph\":"            + String(ph, 1)            + ",";
      json += "\"nitrogenio\":"    + String(nitrogenio)       + ",";
      json += "\"fosforo\":"       + String(fosforo)          + ",";
      json += "\"potassio\":"      + String(potassio);
      json += "}";

      Serial.println("JSON: " + json);

      // Envia via LoRa
      int estado = radio.transmit(json);
      if (estado == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa enviado!");
      } else {
        Serial.print("Erro LoRa: ");
        Serial.println(estado);
      }

      // Atualiza OLED
      mostrarOLED();
    }
  }
}

void mostrarOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Umi:"); display.print(umidade, 1);
  display.print("% T:"); display.print(temperatura, 1); display.println("C");

  display.setCursor(0, 12);
  display.print("Cond:"); display.print(condutividade); display.println("uS");

  display.setCursor(0, 24);
  display.print("pH:"); display.println(ph, 1);

  display.setCursor(0, 36);
  display.print("N:"); display.print(nitrogenio);
  display.print(" P:"); display.print(fosforo);
  display.print(" K:"); display.println(potassio);

  display.setCursor(0, 52);
  display.println("LoRa 915MHz TX OK");

  display.display();
}

String getValue(String data, char sep, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == sep || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
