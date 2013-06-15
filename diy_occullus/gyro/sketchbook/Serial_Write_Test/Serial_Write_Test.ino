#include "math.h"

int gyroPower = A5;

void setup() {
  //Enable PowerSupply to Motion Shield
  pinMode(gyroPower, OUTPUT);
  digitalWrite(gyroPower, HIGH);

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);
}

void loop() {
  write(255);
  write(256);
  write(257);
  write(pow(2,16));
  
  delay(1);

}

void write(int16_t value){
  //writes the value as 2Byte to serial
  Serial.write(value/256);
  Serial.write(value%256);
}


