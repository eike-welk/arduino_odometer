#include <Encoder.h>

// Interrupt capable pins on Arduino Nano:
//  Pins: D2, D3
// I2C Pins:
// A4 (SDA) and A5 (SCL)

Encoder enc_1(2, 4);

void setup() {
  Serial.begin(9600);
  Serial.println("Hall Encoder Test");
}

long pos_1 = 0;

void loop() {
  long pos_1_new = enc_1.read();
  if (pos_1 != pos_1_new){
    Serial.print("enc_1 = ");
    Serial.print(pos_1_new);
    Serial.println();
  }
  pos_1 = pos_1_new;
  
  delay(100);
}
