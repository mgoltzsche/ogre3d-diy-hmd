// I2Cdev library collection - L3G4200D I2C device class
// Based on [Manufacturer Name] L3G4200D datasheet, [datasheet date]
// [current release date] by [Author Name] <[Author Email]>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     [YYYY-mm-dd] - updated some broken thing
//     [YYYY-mm-dd] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 [Author Name], Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "L3G4200D.h"

// ============================================================================
// DEVICE LIBRARY CONVENTIONS:
//
// 1. The class name should be the same as the device model if at all possible.
//    No spaces or hyphens, and ideally no underlines unless they're absolutely
//    necessary for clarity. ALL CAPS for model numbers, or FirstInitial caps
//    for full names. For example:
//      - ADXL345
//      - MPU6050
//      - TCA6424A
//      - PanelPilot
//
// 2. All #defines should use a device-specific prefix that is the same as the
//    all-caps version of the class name ("L3G4200D_" in this example).
//
// 3. All #defines should be ALL CAPS, no matter what. This makes them clearly
//    distinctive from variables, classes, and functions.
//
// 4. Class methods and member variables should be named using camelCaps.
//
// 5. Every device class should contain an "initialize()" method which takes
//    no parameters and resets any important settings back to a known state,
//    ideally the defaults outlined in the datasheet. Some devices have a
//    RESET command available, which may be suitable. Some devices may not
//    require any specific initialization, but an empty method should be
//    created for consistency anyway.
//
// 6. Every device class should contain a "testConnection()" method which
//    returns TRUE if the device appears to be connected, or FALSE otherwise.
//    If a known ID register or device code is available, check for that. Since
//    such an ID register is not always available, at least check to make sure
//    that an I2C read may be performed on a specific register and that data
//    does actually come back (the I2Cdev class returns a "bytes read" value
//    for all read operations).
//
// 7. All class methods should be documented with useful information in Doxygen
//    format. You can take the info right out of the datasheet if you want to,
//    since that's likely to be helpful. Writing your own info is fine as well.
//    The goal is to have enough clear documentation right in the code that
//    someone can determine how the device works by studying the code alone.
//
// ============================================================================

/* ============================================================================
   I2Cdev Class Quick Primer:

   The I2Cdev class provides simple methods for reading and writing from I2C
   device registers without messing with the underlying TWI/I2C functions. You
   just specify the device address, register address, and source or destination
   data according to which action you are doing. Here is the list of relevant
   function prototypes from the I2Cdev class (more info in the .cpp/.h files):

    static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);

    static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
    static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
    static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
    static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
    static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

   Note that ALL OF THESE METHODS ARE STATIC. No I2Cdev object is needed; just
   use the static class methods.
   
   Also note that the first two parameters of every one of these methods are
   the same: "devAddr" and "regAddr". For every method, you need to specify the
   target/slave address and the register address.
   
   If your device uses 8-bit registers, you will want to use the following:
       readBit, readBits, readByte, readBytes
       writeBit, writeBits, writeByte, writeBytes
       
   ...but if it uses 16-bit registers, you will want to use these instead:
       readBitW, readBitsW, readWord, readWords
       writeBitW, writeBitsW, writeWord, writeWords
       
   Here's a sample of how to use a few of the methods. Note that in each case, 
   the "buffer" variable is a uint8_t array or pointer, and the "value" variable
   (in three of the write examples) is a uint8_t single byte. The multi-byte
   write methods still require an array or pointer.

       READ 1 BIT FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBit(0x68, 0x02, 4, buffer);

       READ 3 BITS FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBits(0x68, 0x02, 4, 3, buffer);

       READ 1 BYTE FROM DEVICE 0x68, REGISTER 0x02
       bytesRead = I2Cdev::readByte(0x68, 0x02, buffer);

       READ 2 BYTES FROM DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       bytesRead = I2Cdev::readBytes(0x68, 0x02, 2, buffer);

       WRITE 1 BIT TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBit(0x68, 0x02, 4, value);

       WRITE 3 BITS TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBits(0x68, 0x02, 4, 3, value);

       WRITE 1 BYTE TO DEVICE 0x68, REGISTER 0x02
       status = I2Cdev::writeByte(0x68, 0x02, value);

       WRITE 2 BYTES TO DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       status = I2Cdev::writeBytes(0x68, 0x02, 2, buffer);
       
   The word-based methods are exactly the same, except they use 16-bit variables
   instead of 8-bit ones.

   ============================================================================ */

/** Default constructor, uses default I2C address.
 * @see L3G4200D_DEFAULT_ADDRESS
 */
L3G4200D::L3G4200D() {
    devAddr = L3G4200D_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see L3G4200D_DEFAULT_ADDRESS
 * @see L3G4200D_ADDRESS
 */
L3G4200D::L3G4200D(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void L3G4200D::initialize() {
    // ----------------------------------------------------------------------------
    // STUB TODO:
    // Perform any important initialization here. Maybe nothing is required, but
    // the method should exist anyway.
    // ---------------------------------------------------------------------------- 
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG1, 0b00000111);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG2, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG3, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG4, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG5, 0b00000000);    
    I2Cdev::writeByte(devAddr, L3G4200D_RA_FIFO_CTRL_REG, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_CFG, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_SRC, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_XH, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_XL, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_YH, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_YL, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_ZH, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_TSH_ZL, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_CFG, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_CFG, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_DURATION, 0b00000000);  
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool L3G4200D::testConnection() {
    return getDeviceID() == L3G4200D_WHO_AM_I;
}

uint8_t L3G4200D::getDeviceID() {
    I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer);
    return buffer[0];
}

void L3G4200D::getRotations(int16_t* pitch, int16_t* yaw, int16_t* roll){  
    *pitch = getPitch();
    *yaw   = getYaw();
    *roll  = getRoll();
}

void L3G4200D::getRawRotations(uint8_t *_buffer){  
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_X_L, buffer);
    _buffer[0] = buffer[0];
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_X_H, buffer);
    _buffer[1] = buffer[0];
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Y_L, buffer);
    _buffer[2] = buffer[0];
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Y_H, buffer);
    _buffer[3] = buffer[0];
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Z_L, buffer);
    _buffer[4] = buffer[0];
    I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Z_H, buffer);
    _buffer[5] = buffer[0];
}

int16_t L3G4200D::getPitch(){
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_X_L, buffer);
    byte lsb = buffer[0];
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_X_H, buffer);
    byte msb = buffer[0];
    
	return (msb << 8) | lsb;
}

int16_t L3G4200D::getYaw(){
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Y_L, buffer);
    byte lsb = buffer[0];
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Y_H, buffer);
    byte msb = buffer[0];
    
	return (msb << 8) | lsb;
}

int16_t L3G4200D::getRoll(){
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Z_L, buffer);
    byte lsb = buffer[0];
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_Z_H, buffer);
    byte msb = buffer[0];
    
	return (msb << 8) | lsb;
}


void L3G4200D::reset(){

}               

bool L3G4200D::isFIFOEnabled(){
    I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG5 ,L3G4200D_FIFO_EN_BIT , buffer);
    return buffer[0];
}

void L3G4200D::setFIFOEnabled(bool enable){
    I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5 ,L3G4200D_FIFO_EN_BIT , enable);	
}

uint8_t L3G4200D::getFIFOMode(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_FIFO_CTRL_REG ,L3G4200D_FIFO_CTRL_MODE_BIT, L3G4200D_FIFO_CTRL_MODE_LENGTH ,buffer);
    return buffer[0];
}

void L3G4200D::setFIFOMode(uint8_t mode){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_FIFO_CTRL_REG ,L3G4200D_FIFO_CTRL_MODE_BIT, L3G4200D_FIFO_CTRL_MODE_LENGTH ,mode);
}

uint8_t L3G4200D::getDataRate(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_DR_RATE_BIT, L3G4200D_DR_RATE_LENGTH, buffer);
    return buffer[0];
}

void L3G4200D::setDataRate(uint8_t rate){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_DR_RATE_BIT, L3G4200D_DR_RATE_LENGTH, rate);
}

uint8_t L3G4200D::getBandwidth(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_RATE_BIT, L3G4200D_BW_RATE_LENGTH, buffer);
    return buffer[0];
}

void L3G4200D::setBandwidth(uint8_t bw){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_RATE_BIT, L3G4200D_BW_RATE_LENGTH, bw);
}


uint8_t L3G4200D::getDRBWRate(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1 ,L3G4200D_DR_BW_RATE_BIT,L3G4200D_DR_BW_RATE_LENGTH , buffer);
    return buffer[0];
}

void L3G4200D::setDRBWRate(uint8_t rate){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1 ,L3G4200D_DR_BW_RATE_BIT,L3G4200D_DR_BW_RATE_LENGTH ,rate);
}

uint8_t L3G4200D::getScale(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG4 ,L3G4200D_SCALE_RATE_BIT,L3G4200D_SCALE_RATE_LENGTH ,buffer);
    return buffer[0];
}

void L3G4200D::setScale(uint8_t scale){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG4 ,L3G4200D_SCALE_RATE_BIT,L3G4200D_SCALE_RATE_LENGTH ,scale);
}

uint8_t L3G4200D::getPowerMode(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1 ,L3G4200D_POWERMODE_BIT,L3G4200D_POWERMODE_LENGTH ,buffer);
    return buffer[0];
}

void L3G4200D::setPowerMode(uint8_t powerMode){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1 ,L3G4200D_POWERMODE_BIT,L3G4200D_POWERMODE_LENGTH ,powerMode);
}

uint8_t L3G4200D::getHPFMode(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2 ,L3G4200D_HPF_MODE_BIT,L3G4200D_HPF_MODE_LENGTH ,buffer);
    return buffer[0];
}

void L3G4200D::setHPFMode(uint8_t hpfMode){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG2 ,L3G4200D_HPF_MODE_BIT,L3G4200D_HPF_MODE_LENGTH ,hpfMode);
}

uint8_t L3G4200D::getHPFCutOffFrequency(){
    I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2 ,L3G4200D_HPF_CF_MODE_BIT,L3G4200D_HPF_CF_MODE_LENGTH ,buffer);
    return buffer[0];
}

void L3G4200D::setHPFCutOffFrequency(uint8_t freq){
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG2 ,L3G4200D_HPF_CF_MODE_BIT,L3G4200D_HPF_CF_MODE_LENGTH ,freq);
}

bool L3G4200D::isOutHighLowFiltered(){

}

void L3G4200D::setOutHighLowFiltered(){
    I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5 ,L3G4200D_HPEN_BIT ,1);
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5 ,L3G4200D_OUT_SEL_BIT ,L3G4200D_OUT_SEL_LENGTH ,L3G4200D_OUT_SEL_LOWPASS_FILTERED);
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5 ,L3G4200D_INT1_SEL_BIT ,L3G4200D_INT1_SEL_LENGTH ,L3G4200D_INT1_SEL_LOWPASS_FILTERED);
}

bool L3G4200D::isOutLowFiltered(){

}

void L3G4200D::setOutLowFiltered(){

}

bool L3G4200D::isOutHighFiltered(){

}

void L3G4200D::setOutHighFiltered(){

}

bool L3G4200D::isOutNONFiltered(){

}

void L3G4200D::setOutNONFiltered(){

}

bool L3G4200D::isHighLowFilteredInterrupt(){

}

void L3G4200D::setHighLowFilteredInterrupt(){

}

bool L3G4200D::isLowFilteredInterrupt(){

}

void L3G4200D::setLowFilteredInterrupt(){

}

bool L3G4200D::isHighFilteredInterrupt(){

}

void L3G4200D::setHighFilteredInterrupt(){

}

bool L3G4200D::isNONFilteredInterrupt(){

}

void L3G4200D::setNONFilteredInterrupt(){

}

bool L3G4200D::isDRDYInterrupt(){
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3 ,L3G4200D_I2_DRDY_BIT, buffer);
	return buffer[0];
}

void L3G4200D::setDRDYInterrupt(bool enable){    
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3 ,L3G4200D_I2_DRDY_BIT, enable);
}

void L3G4200D::printRegister(){
    I2Cdev::readByte(devAddr, L3G4200D_RA_CTRL_REG1 , buffer);
    Serial.print("L3G4200D_RA_CTRL_REG1\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_CTRL_REG2 , buffer);
    Serial.print("L3G4200D_RA_CTRL_REG2\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_CTRL_REG3 , buffer);
    Serial.print("L3G4200D_RA_CTRL_REG3\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_CTRL_REG4 , buffer);
    Serial.print("L3G4200D_RA_CTRL_REG4\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_CTRL_REG5 , buffer);
    Serial.print("L3G4200D_RA_CTRL_REG5\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_FIFO_CTRL_REG, buffer);
    Serial.print("L3G4200D_RA_FIFO_CTRL_REG\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_CFG, buffer);
    Serial.print("L3G4200D_RA_INT1_CFG\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_XH, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_XH\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_XL, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_XL\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_YH, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_YH\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_YL, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_YL\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_ZH, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_ZH\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_TSH_ZL, buffer);
    Serial.print("L3G4200D_RA_INT1_TSH_ZL\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_CFG, buffer);
    Serial.print("L3G4200D_RA_INT1_CFG\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_CFG, buffer);
    Serial.print("L3G4200D_RA_INT1_CFG\t\t");
    Serial.println(buffer[0], BIN);
    I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_DURATION, buffer);
    Serial.print("L3G4200D_RA_INT1_DURATION\t");
    Serial.println(buffer[0], BIN);
}


// ----------------------------------------------------------------------------
// STUB TODO:
// Define methods to fully cover all available functionality provided by the
// device, according to the datasheet and/or register map. Unless there is very
// clear reason not to, try to follow the get/set naming convention for all
// values, for instance:
//   - uint8_t getThreshold()
//   - void setThreshold(uint8_t threshold)
//   - uint8_t getRate()
//   - void setRate(uint8_t rate)
//
// Some methods may be named differently if it makes sense. As long as all
// functionality is covered, that's the important part. The methods here are
// only examples and should not be kept for your real device.
// ----------------------------------------------------------------------------



