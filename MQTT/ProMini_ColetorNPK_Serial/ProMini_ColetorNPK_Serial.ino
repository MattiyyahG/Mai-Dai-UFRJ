#include <SoftwareSerial.h>
#define RE 8
#define DE 7
byte values[21];
SoftwareSerial mod(2, 3);
const byte lerTodosOsSensores[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };

void setup() {
  Serial.begin(4800);
  mod.begin(4800);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  Serial.println("Leitor NPK PHCTH-S 7 em 1");
  delay(500);
}

void loop() {
  LerSensores();
  
  unsigned int umidade, temperatura, condutividade, ph, nitrogenio, fosforo, potassio;
  Serial.print("Umidade: ");
  umidade = (values[3] << 8) | values[4];
  Serial.print(umidade / 10.0);
  Serial.println("%");
  Serial.print("Temperatura: ");
  temperatura = (values[5] << 8) | values[6];
  Serial.print(temperatura / 10.0);
  Serial.println(" °C");
  Serial.print("Condutividade: ");
  condutividade = (values[7] << 8) | values[8];
  Serial.print(condutividade);
  Serial.println(" uS/cm");
  Serial.print("PH: ");
  ph = (values[9] << 8) | values[10];
  Serial.println(ph / 10.0);
  Serial.print("Nitrogênio (N): ");
  nitrogenio = (values[11] << 8) | values[12];
  Serial.print(nitrogenio);
  Serial.println(" mg/kg");
  Serial.print("Fósforo (P): ");
  fosforo = (values[13] << 8) | values[14];
  Serial.print(fosforo);
  Serial.println(" mg/kg");
  Serial.print("Potássio (K): ");
  potassio = (values[15] << 8) | values[16];
  Serial.print(potassio);
  Serial.println(" mg/kg");
  Serial.println();

  // Envia dados pelo TX (pino 1) em formato CSV simples
  // Formato: DATA|umidade|temperatura|condutividade|ph|nitrogenio|fosforo|potassio
  Serial.print("DATA|");
  Serial.print(umidade / 10.0);      Serial.print("|");
  Serial.print(temperatura / 10.0);  Serial.print("|");
  Serial.print(condutividade);        Serial.print("|");
  Serial.print(ph / 10.0);           Serial.print("|");
  Serial.print(nitrogenio);           Serial.print("|");
  Serial.print(fosforo);              Serial.print("|");
  Serial.print(potassio);
  Serial.println("|END");

  delay(5000);
}

void LerSensores() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(lerTodosOsSensores, sizeof(lerTodosOsSensores)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    byte i = 0;
    while(mod.available()){
      values[i++] = mod.read();
      printHexByte(values[i-1]);
    }
    Serial.println();
  }
}

void printHexByte(byte b)
{
  Serial.print((b >> 4) & 0xF, HEX);
  Serial.print(b & 0xF, HEX);
  Serial.print(' ');
}
