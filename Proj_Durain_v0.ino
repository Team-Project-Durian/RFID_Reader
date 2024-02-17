#include <SoftwareSerial.h>
SoftwareSerial RFID(D5, D6); // RX and TX

String text;
char c;

void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  Serial.println("Bring your RFID Card Closer...");
}

void loop() {
  while (RFID.available() > 0) {
    delay(5);
    c = RFID.read();
    text += c;
  }
  if (text.length() > 20) {
    check();
    text = "";
  }
}

void check() {
  text = text.substring(1, 11);
  Serial.println("Card ID : " + text);
  delay(2000);
  Serial.println(" ");
  Serial.println("Bring your RFID card closer …");
}