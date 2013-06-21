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

bool calculateZeroRate;

uint8_t zeroRateCompensationIndex , zeroRateCompensationNSamples, gyroThreshHold;

uint8_t output[22];
uint8_t buffer[6];

int16_t x, y, z;
int16_t gyroXZR, gyroYZR, gyroZZR;

long prevTime, currentTime;

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

  calculateZeroRate = true;
  zeroRateCompensationIndex = 0;
  zeroRateCompensationNSamples = 50;
  gyroThreshHold = 50;
  prevTime = millis();

}

void loop() {
  if(true){ // No Debug
    if(calculateZeroRate){
      if(zeroRateCompensationIndex <= zeroRateCompensationNSamples){
        gyro.getRotations(&x,&y,&z);
        gyroXZR += abs(x);
        gyroYZR += abs(y);
        gyroZZR += abs(z);
        zeroRateCompensationIndex++;
      } 
      else {        
        calculateZeroRate = false;
        gyroXZR /= zeroRateCompensationNSamples;
        gyroYZR /= zeroRateCompensationNSamples;
        gyroZZR /= zeroRateCompensationNSamples;

        gyroThreshHold = (gyroXZR + gyroYZR + gyroZZR) / 3;
      }
    } 
    else {
      gyro.getRotations(&x,&y,&z);
      x = abs(x)-gyroXZR > gyroThreshHold ? x : 0;
      y = abs(y)-gyroYZR > gyroThreshHold ? y : 0;
      z = abs(z)-gyroZZR > gyroThreshHold ? z : 0;      
      convertAndInsert(x, 0);
      convertAndInsert(y, 2);
      convertAndInsert(z, 4);      
      accel.getRawAcceleration(buffer);
      output[6] = buffer[0];
      output[7] = buffer[1];
      output[8] = buffer[2];
      output[9] = buffer[3];
      output[10] = buffer[4];
      output[11] = buffer[5];
      mag.getHeading(&x,&y,&z);
      output[12] = buffer[0];
      output[13] = buffer[1];
      output[14] = buffer[2];
      output[15] = buffer[3];
      output[16] = buffer[4];
      output[17] = buffer[5];

      Serial.write(output, 22);      

    }
  } 
  else {
    gyro.printRegister();
    Serial.println("-------------------------------------------------------------");
  }
}

void setupGyro(){
  gyro.setPowerMode(L3G4200D_POWERMODE_NORMAL);
  gyro.setDRDYInterrupt(true);
  gyro.setScale(L3G4200D_SCALE_RATE_2000DPS);
  gyro.setDRBWRate(L3G4200D_DR_BW_RATE_ODR800_CO30); 
  gyro.setHPFMode(L3G4200D_HPF_MODE_NORMAL);
  gyro.setOutHighLowFiltered();

  if(gyro.getScale() == L3G4200D_SCALE_RATE_2000DPS){
    convertAndInsert(2000, 18);
  }
  else if (gyro.getScale() == L3G4200D_SCALE_RATE_500DPS){
    convertAndInsert(500, 18);
  }
  else if (gyro.getScale() == L3G4200D_SCALE_RATE_250DPS){
    convertAndInsert(250, 18);
  }

  if(gyro.getDataRate() == L3G4200D_DR_RATE_800){
    convertAndInsert(800, 20);
  }
  else if(gyro.getDataRate() == L3G4200D_DR_RATE_400){
    convertAndInsert(400, 20);
  }
  else if(gyro.getDataRate() == L3G4200D_DR_RATE_200){
    convertAndInsert(200, 20);
  }
  else if(gyro.getDataRate() == L3G4200D_DR_RATE_100){
    convertAndInsert(100, 20);
  }

}

void convertAndInsert(int16_t value, uint8_t index ){
  output[index]   = value % 256 ;
  output[index+1] = value / 256;
}
















