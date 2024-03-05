#include <Arduino.h>

void setup() {

  Serial.begin(115200);

}

void loop() {

  int a=analogRead(13);
  Serial.println(a);
  delay(1000); 

}
