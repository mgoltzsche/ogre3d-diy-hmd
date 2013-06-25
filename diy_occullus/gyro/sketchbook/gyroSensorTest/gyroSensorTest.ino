// Name: L3G4200D_Gyro_Init_and_Test
// Written by: William Martin
// Developed for: The University of Tennessee Graduate Robotics Team
// Created: 3/1/2013
// Last Modified: 3/3/2013
// Contact: wmartin8@utk.edu
// License: open-source, but I do appreciate the above lines being included in your code
// Based on: http://bildr.org/2011/06/l3g4200d-arduino/ and the MIT License

/**** GENERAL NOTES ****
 * - this code is only converned with the yaw axis (z) of the gyro. To work with
 * - the roll (x) and pitch (y) axes, you can copy this code with a slight change of variable names. 
 * 
 * - boolean variable 'calibrate' should only need to set to 'true' if you are getting 
 * - unexpected behavior from code. This may happen if your use requires the gyro to be rotated
 * - either extremely quickly or extremly slowly. 
 ******* END NOTES ********/

#include <Wire.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

int L3G4200D_Address = 0x69; //I2C address of the L3G4200D

// These hold the instantaneous values from the gryo for pitch, roll, and yaw
int rollGyroVal;
int pitchGyroVal;
int yawGyroVal;

int gyroPower = A5;
void setup(){

  pinMode(gyroPower, OUTPUT);
  digitalWrite(gyroPower, HIGH);

  Wire.begin();
  Serial.begin(38400);

  setupL3G4200D(2000); // Configure L3G4200  - 250, 500 or 2000 deg/sec

  delay(1500); //wait for the sensor to be ready 

}

void loop(){
  getGyroValues();
  Serial.print("roll\t");
  Serial.print(rollGyroVal);
  Serial.print("\tyaw\t");
  Serial.print(yawGyroVal);
  Serial.print("\tpitch\t");
  Serial.print(pitchGyroVal);
  Serial.println("\t");
  //  testCalibrationResults();
  delay(1);
}

void getGyroValues(){
  // Get instantaneous roll, pitch and yaw values from gyro
  byte rollGyroValMSB = readRegister(L3G4200D_Address, 0x29);
  byte rollGyroValLSB = readRegister(L3G4200D_Address, 0x28);
  rollGyroVal = ((rollGyroValMSB << 8) | rollGyroValLSB);

  byte pitchGyroValMSB = readRegister(L3G4200D_Address, 0x2B);
  byte pitchGyroValLSB = readRegister(L3G4200D_Address, 0x2A);
  pitchGyroVal = ((pitchGyroValMSB << 8) | pitchGyroValLSB);

  byte yawGyroValMSB = readRegister(L3G4200D_Address, 0x2D);
  byte yawGyroValLSB = readRegister(L3G4200D_Address, 0x2C);
  yawGyroVal = ((yawGyroValMSB << 8) | yawGyroValLSB);
}

int setupL3G4200D(int scale){
  //From  Jim Lindblom of Sparkfun's code

    // Enable rollGyroVal, pitchGyroVal, yawGyroVal and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b11001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }
  else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }
  else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
  Wire.beginTransmission(deviceAddress); // start transmission to device 
  Wire.write(address);       // send register address
  Wire.write(val);         // send value to write
  Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

  int v;
  Wire.beginTransmission(deviceAddress);
  Wire.write(address); // register to read
  Wire.endTransmission();

  Wire.requestFrom(deviceAddress, 1); // read a byte

  while(!Wire.available()) {
    // waiting
  }

  v = Wire.read();
  return v;
}
