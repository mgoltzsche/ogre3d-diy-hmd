// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev, HMC5883L and ADXL345 must be installed as libraries, or else the .cpp/.h files
// for all classes must be in the include path of your project
#include "I2Cdev.h"
#include "ADXL345.h"
#include "HMC5883L.h"

// class default I2C address is 0x53
// specific I2C addresses may be passed as a parameter here
// ALT low = 0x53 (default for SparkFun 6DOF board)
// ALT high = 0x1D
ADXL345 accel;

// class default I2C address is 0x1E
// specific I2C addresses may be passed as a parameter here
// this device only supports one I2C address (0x1E)
HMC5883L mag;


int gyroPower = A5;

byte output[12];
int index;

void setup() {
  //Enable PowerSupply to Motion Shield
  pinMode(gyroPower, OUTPUT);
  digitalWrite(gyroPower, HIGH);

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

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accel.testConnection() ? "ADXL345 connection successful" : "ADXL345 connection failed");
  Serial.println(mag.testConnection() ? "HMCL345L connection successful" : "HMCL345L connection failed");


}

void loop() {
  index = 0;
  //Set Data output rate to 75Hz
  mag.setDataRate(HMC5883L_RATE_75);

  //Serial.print("mag rate: ");
  //Serial.print(mag.getDataRate());

  //Serial.print(" accel rate: ");
  //Serial.println(accel.getRate());

  Serial.print("accel:\t");
  Serial.print("\t");
  Serial.print(accel.getAccelerationX());
  Serial.print("\t");
  Serial.print(accel.getAccelerationY());
  Serial.print("\t");
  Serial.print(accel.getAccelerationZ());
  Serial.print("\t");
  Serial.print("\tmag:\t");
    Serial.print(mag.getHeadingX());
  Serial.print("\t");
  Serial.print(mag.getHeadingY());
  Serial.print("\t");
  Serial.print(mag.getHeadingZ());
  Serial.print("\n");


  //write(accel.getAccelerationX());
  //write(accel.getAccelerationY());
  //write(accel.getAccelerationZ());
  //write(mag.getHeadingX());
  //write(mag.getHeadingY());
  //write(mag.getHeadingZ());

  //Serial.write(output, 12);

  delay(1);

}

void write(int16_t value){
  //writes the value as 2Byte to serial
  output[index++] = value/256;
  output[index++] = value%256;
}







