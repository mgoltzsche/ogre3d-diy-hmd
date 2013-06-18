// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include <Wire.h>
// I2Cdev, HMC5883L and ADXL345 must be installed as libraries, or else the .cpp/.h files
// for all classes must be in the include path of your project
#include "I2Cdev.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "L3G4200D.h"
// class default I2C address is 0x53
// specific I2C addresses may be passed as a parameter here
// ALT low = 0x53 (default for SparkFun 6DOF board)
// ALT high = 0x1D
ADXL345 accel;

// class default I2C address is 0x1E
// specific I2C addresses may be passed as a parameter here
// this device only supports one I2C address (0x1E)
HMC5883L mag;

L3G4200D gyro;

int motionTrackerPower = A5;


uint8_t output[18];
uint8_t buffer[6];

void setup() {
  //Enable PowerSupply to Motion Shield
  pinMode(motionTrackerPower, OUTPUT);
  digitalWrite(motionTrackerPower, HIGH);

  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accel.initialize();
  mag.initialize();
  gyro.initialize();
  
  //setup devices
  setupGyro();

}

void loop() {
  if(gyro.testConnection() && accel.testConnection() && mag.testConnection()){

    gyro.getRawRotations(buffer);
    output[0] = buffer[0];
    output[1] = buffer[1];
    output[2] = buffer[2];
    output[3] = buffer[3];
    output[4] = buffer[4];
    output[5] = buffer[5];     
    accel.getRawAcceleration(buffer);
    output[6] = buffer[0];
    output[7] = buffer[1];
    output[8] = buffer[2];
    output[9] = buffer[3];
    output[10] = buffer[4];
    output[11] = buffer[5];
    mag.getRawHeading(buffer);
    output[12] = buffer[0];
    output[13] = buffer[1];
    output[14] = buffer[2];
    output[15] = buffer[3];
    output[16] = buffer[4];
    output[17] = buffer[5];
    /*
     output[0]  = 0b000001;
     output[1]  = 0b000000;
     output[2]  = 0b000010;
     output[3]  = 0b000000;
     output[4]  = 0b000011;       
     output[5]  = 0b000000; 
     output[6]  = 0b000100;
     output[7]  = 0b000000; 
     output[8]  = 0b000101; 
     output[9]  = 0b000000; 
     output[10] = 0b000110; 
     output[11] = 0b000000; 
     output[12] = 0b000111; 
     output[13] = 0b000000; 
     output[14] = 0b001000; 
     output[15] = 0b000000; 
     output[16] = 0b001001; 
     output[17] = 0b000000;       
     */
     Serial.write(output, 18);   
     /*
     Serial.print("gyro: \t");
     printBytes(output[0], output[1]);
     Serial.print("\t");
     printBytes(output[2], output[3]);
     Serial.print("\t");
     printBytes(output[4], output[5]);    

     Serial.print("\t acc: \t");
     printBytes(output[6], output[7]);
     Serial.print("\t");
     printBytes(output[8], output[9]);
     Serial.print("\t");
     printBytes(output[10], output[11]);    

     Serial.print("\t mag: \t");
     printBytes(output[12], output[13]);
     Serial.print("\t");
     printBytes(output[14], output[15]);
     Serial.print("\t");
     printBytes(output[16], output[17]);    
     Serial.print("\n");
     */
     
  }
  /*
  gyro.printRegister();
  Serial.println("-------------------------------------------------------------");
  */
  delay(1);

}

void setupGyro(){
  gyro.setPowerMode(L3G4200D_POWERMODE_NORMAL);
  gyro.setDRDYInterrupt(true);
  gyro.setScale(L3G4200D_SCALE_RATE_2000DPS);
  gyro.setRate(L3G4200D_DR_BW_RATE_ODR800_CO30); 
  gyro.setOutHighLowFiltered();
}

void printBytes(uint8_t lsb, uint8_t msb){
  Serial.print(((int16_t) msb << 8 ) | lsb, DEC);
}





