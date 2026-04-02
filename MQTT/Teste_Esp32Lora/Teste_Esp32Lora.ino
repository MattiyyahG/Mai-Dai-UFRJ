#define RE 3
#define DE 4

const byte lerTodosOsSensores[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
byte values[21];

void setup() {
  Serial.begin(115200);  // Monitor serial USB

  // Serial1: RO=GPIO1 (RX), DI=GPIO2 (TX)
  Serial1.begin(4800, SERIAL_8N1, 1, 2);

  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  Serial.println("Teste NPK no Heltec V3");
  delay(500);
}

void loop() {
  LerSensores();

  unsigned int umidade, temperatura, condutividade, ph, nitrogenio, fosforo, potassio;

  umidade       = (values[3]  << 8) | values[4];
  temperatura   = (values[5]  << 8) | values[6];
  condutividade = (values[7]  << 8) | values[8];
  ph            = (values[9]  << 8) | values[10];
  nitrogenio    = (values[11] << 8) | values[12];
  fosforo       = (values[13] << 8) | values[14];
  potassio      = (values[15] << 8) | values[16];

  Serial.print("Umidade: ");      Serial.print(umidade / 10.0);      Serial.println("%");
  Serial.print("Temperatura: "); Serial.print(temperatura / 10.0);  Serial.println(" °C");
  Serial.print("Condutividade: ");Serial.print(condutividade);        Serial.println(" uS/cm");
  Serial.print("PH: ");           Serial.println(ph / 10.0);
  Serial.print("Nitrogênio: ");  Serial.print(nitrogenio);           Serial.println(" mg/kg");
  Serial.print("Fósforo: ");     Serial.print(fosforo);              Serial.println(" mg/kg");
  Serial.print("Potássio: ");    Serial.print(potassio);             Serial.println(" mg/kg");
  Serial.println();

  delay(5000);
}

void LerSensores() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  if (Serial1.write(lerTodosOsSensores, sizeof(lerTodosOsSensores)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    delay(100);

    byte i = 0;
    while (Serial1.available()) {
      values[i++] = Serial1.read();
    }
  }
}
