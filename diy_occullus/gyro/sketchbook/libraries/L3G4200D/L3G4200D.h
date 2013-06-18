// I2Cdev library collection - L3G4200D I2C device class header file
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

#ifndef _L3G4200D_H_
#define _L3G4200D_H_

#include "I2Cdev.h"



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

// ----------------------------------------------------------------------------
// STUB TODO:
// List all possible device I2C addresses here, if they are predefined. Some
// devices only have one possible address, in which case that one should be
// defined as both [L3G4200D]_ADDRESS and [L3G4200D]_DEFAULT_ADDRESS for
// consistency. The *_DEFAULT address will be used if a device object is
// created without an address provided in the constructor. Note that I2C
// devices conventionally use 7-bit addresses, so these will generally be
// between 0x00 and 0x7F.
// ----------------------------------------------------------------------------


#define L3G4200D_ADDRESS 		0x69
#define L3G4200D_DEFAULT_ADDRESS 	L3G4200D_ADDRESS

// ----------------------------------------------------------------------------
// STUB TODO:
// List all registers that this device has. The goal for all device libraries
// is to have complete coverage of all possible functions, so be sure to add
// everything in the datasheet. Register address constants should use the extra
// prefix "RA_", followed by the datasheet's given name for each register.
// ----------------------------------------------------------------------------

#define L3G4200D_RA_WHO_AM_I 		0x0F
#define L3G4200D_RA_CTRL_REG1 		0x20
#define L3G4200D_RA_CTRL_REG2 		0x21
#define L3G4200D_RA_CTRL_REG3 		0x22
#define L3G4200D_RA_CTRL_REG4 		0x23
#define L3G4200D_RA_CTRL_REG5 		0x24
#define L3G4200D_RA_REFERENCE 		0x25
#define L3G4200D_RA_OUT_TEMP 		0x26
#define L3G4200D_RA_STATUS_REG 		0x27
#define L3G4200D_RA_OUT_X_L 		0x28
#define L3G4200D_RA_OUT_X_H 		0x29
#define L3G4200D_RA_OUT_Y_L 		0x2A
#define L3G4200D_RA_OUT_Y_H 		0x2B
#define L3G4200D_RA_OUT_Z_L 		0x2C
#define L3G4200D_RA_OUT_Z_H 		0x2D
#define L3G4200D_RA_FIFO_CTRL_REG	0x2E
#define L3G4200D_RA_FIFO_SRC_REG	0X2F
#define L3G4200D_RA_INT1_CFG		0x30
#define L3G4200D_RA_INT1_SRC		0x31
#define L3G4200D_RA_INT1_TSH_XH		0x32
#define L3G4200D_RA_INT1_TSH_XL		0x33
#define L3G4200D_RA_INT1_TSH_YH		0x34
#define L3G4200D_RA_INT1_TSH_YL		0x35
#define L3G4200D_RA_INT1_TSH_ZH		0x36
#define L3G4200D_RA_INT1_TSH_ZL		0x37
#define L3G4200D_RA_INT1_DURATION	0x38


// ----------------------------------------------------------------------------
// STUB TODO:
// List register structures wherever necessary. Bit position constants should
// end in "_BIT", and are defined from 7 to 0, with 7 being the left/MSB and
// 0 being the right/LSB. If the device uses 16-bit registers instead of the
// more common 8-bit registers, the range is 15 to 0. Field length constants
// should end in "_LENGTH", but otherwise match the name of bit position
// constant that it complements. Fields that are a single bit in length don't
// need a separate field length constant.
// ----------------------------------------------------------------------------
#define L3G4200D_DR_BW_RATE_BIT     	7
#define L3G4200D_DR_BW_RATE_LENGTH  	4
#define L3G4200D_PD_BIT			        3
#define L3G4200D_ZEN_BIT		        2
#define L3G4200D_YEN_BIT		        1
#define L3G4200D_XEN_BIT		        0
#define L3G4200D_POWERMODE_BIT	    	3
#define L3G4200D_POWERMODE_LENGTH	    4


#define L3G4200D_HPF_MODE_BIT		    5
#define L3G4200D_HPF_MODE_LENGTH	    2
#define L3G4200D_HPF_CF_MODE_BIT	    3
#define L3G4200D_HPF_CF_MODE_LENGTH	    4


#define L3G4200D_I1_INT1_BIT		    7
#define L3G4200D_I1_BOOT_BIT		    6
#define L3G4200D_H_LACTIVE_BIT		    5
#define L3G4200D_PP_OD_BIT		        4
#define L3G4200D_I2_DRDY_BIT		    3
#define L3G4200D_I2_WTM_BIT		        2
#define L3G4200D_I2_ORUN_BIT		    1
#define L3G4200D_I2_EMPTY_BIT		    0


#define L3G4200D_BDU_BIT		        7
#define L3G4200D_BLE_BIT		        6
#define L3G4200D_SCALE_RATE_BIT		    5
#define L3G4200D_SCALE_RATE_LENGTH	    2
#define L3G4200D_SELFTEST_BIT		    2
#define L3G4200D_SELFTEST_LENGTH	    2
#define L3G4200D_WIRE_MODE_BIT		    0

#define L3G4200D_BOOT_BIT		        7
#define L3G4200D_FIFO_EN_BIT		    6
#define L3G4200D_HPEN_BIT		        4
#define L3G4200D_INT1_SEL_BIT		    3
#define L3G4200D_INT1_SEL_LENGTH    	2
#define L3G4200D_OUT_SEL_BIT	    	1
#define L3G4200D_OUT_SEL_LENGTH		    2

#define L3G4200D_REFERENCE_REF7_BIT	    7
#define L3G4200D_REFERENCE_REF6_BIT	    6
#define L3G4200D_REFERENCE_REF5_BIT	    5
#define L3G4200D_REFERENCE_REF4_BIT	    4
#define L3G4200D_REFERENCE_REF3_BIT 	3
#define L3G4200D_REFERENCE_REF2_BIT 	2
#define L3G4200D_REFERENCE_REF1_BIT 	1
#define L3G4200D_REFERENCE_REF0_BIT	    0

#define L3G4200D_OUT_TEMP_BIT		    7
#define L3G4200D_OUT_TEMP_LENGTH	    8

#define L3G4200D_STATUS_ZYXOR_BIT	    7
#define L3G4200D_STATUS_ZOR_BIT		    6
#define L3G4200D_STATUS_YOR_BIT		    5
#define L3G4200D_STATUS_XOR_BIT		    4
#define L3G4200D_STATUS_ZYXDA_BIT	    3
#define L3G4200D_STATUS_ZDA_BIT		    2
#define L3G4200D_STATUS_YDA_BIT		    1
#define L3G4200D_STATUS_XDA_BIT		    0

#define L3G4200D_FIFO_CTRL_MODE_BIT	    7
#define L3G4200D_FIFO_CTRL_MODE_LENGTH	3




// ----------------------------------------------------------------------------
// STUB TODO:
// List any special predefined values for each register according to the
// datasheet. For example, MEMS devices often provide different options for
// measurement rates, say 25Hz, 50Hz, 100Hz, and 200Hz. These are typically
// represented by arbitrary bit values, say 0b00, 0b01, 0b10, and 0b11 (or 0x0,
// 0x1, 0x2, and 0x3). Defining them here makes it easy to know which options
// are available.
// ----------------------------------------------------------------------------
#define L3G4200D_WHO_AM_I			            0b11010011

#define L3G4200D_DR_BW_RATE_ODR100_CO12_5	    0b0000
#define L3G4200D_DR_BW_RATE_ODR100_CO25_0	    0b0001
#define L3G4200D_DR_BW_RATE_ODR100_CO25_1	    0b0010
#define L3G4200D_DR_BW_RATE_ODR100_CO25_2	    0b0011

#define L3G4200D_DR_BW_RATE_ODR200_CO12_5	    0b0100
#define L3G4200D_DR_BW_RATE_ODR200_CO25		    0b0101
#define L3G4200D_DR_BW_RATE_ODR200_CO50		    0b0110
#define L3G4200D_DR_BW_RATE_ODR200_CO70		    0b0111

#define L3G4200D_DR_BW_RATE_ODR400_CO20		    0b1000
#define L3G4200D_DR_BW_RATE_ODR400_CO25		    0b1001
#define L3G4200D_DR_BW_RATE_ODR400_CO50		    0b1010
#define L3G4200D_DR_BW_RATE_ODR400_CO110	    0b1011

#define L3G4200D_DR_BW_RATE_ODR800_CO30		    0b1100
#define L3G4200D_DR_BW_RATE_ODR800_CO35		    0b1101
#define L3G4200D_DR_BW_RATE_ODR800_CO50		    0b1110
#define L3G4200D_DR_BW_RATE_ODR800_CO110	    0b1111

#define L3G4200D_POWERMODE_POWERDOWN		    0b0111
#define L3G4200D_POWERMODE_SLEEP		        0b1000
#define L3G4200D_POWERMODE_NORMAL		        0b1111

#define L3G4200D_HPF_MODE_NORMAL_RESET		    0b00
#define L3G4200D_HPF_MODE_REFERENCE_SIGNAL	    0b01
#define L3G4200D_HPF_MODE_NORMAL		        0b10
#define L3G4200D_HPF_MODE_AUTORESET_INTERRUPT	0b11

#define L3G4200D_HPF_CF_MODE_0			        0b0000
#define L3G4200D_HPF_CF_MODE_1			        0b0001
#define L3G4200D_HPF_CF_MODE_2			        0b0010
#define L3G4200D_HPF_CF_MODE_3			        0b0011
#define L3G4200D_HPF_CF_MODE_4			        0b0100
#define L3G4200D_HPF_CF_MODE_5			        0b0101
#define L3G4200D_HPF_CF_MODE_6			        0b0110
#define L3G4200D_HPF_CF_MODE_7			        0b0111
#define L3G4200D_HPF_CF_MODE_8			        0b1000
#define L3G4200D_HPF_CF_MODE_9			        0b1001


#define L3G4200D_BDU_CONTINOUS			        0b0
#define L3G4200D_BDU_WAIT			            0b1

#define L3G4200D_BLE_LITTLE_ENDIAN	        	0
#define L3G4200D_BLE_BIG_ENDIAN		        	1

#define L3G4200D_SCALE_RATE_250DPS		        0b00
#define L3G4200D_SCALE_RATE_500DPS		        0b01
#define L3G4200D_SCALE_RATE_2000DPS 	        0b10


#define L3G4200D_SELFTEST_NORMAL	        	0b00
#define L3G4200D_SELFTEST_POSITIV	        	0b01
#define L3G4200D_SELFTEST_UNKNOWN	        	0b10
#define L3G4200D_SELFTEST_NEGATIVE	        	0b11

#define L3G4200D_WIRE_MODE_4WI		        	0b0
#define L3G4200D_WIRE_MODE_3WI		        	0b1

#define L3G4200D_BOOT_NORMAL		        	0b0
#define L3G4200D_BOOT_REBOOT			        0b1

#define L3G4200D_INT1_SEL_NON_HIGHPASS		    0b00
#define L3G4200D_INT1_SEL_HIGHPASS_FILTERED 	0b01
#define L3G4200D_INT1_SEL_LOWPASS_FILTERED	    0b10

#define L3G4200D_OUT_SEL_NON_HIGHPASS		    0b00
#define L3G4200D_OUT_SEL_HIGHPASS_FILTERED	    0b01
#define L3G4200D_OUT_SEL_LOWPASS_FILTERED	    0b10

#define L3G4200D_FIFO_CTRL_MODE_BYPASS		    0b000
#define L3G4200D_FIFO_CTRL_MODE_FIFO		    0b001
#define L3G4200D_FIFO_CTRL_MODE_STREAM		    0b010
#define L3G4200D_FIFO_CTRL_MODE_STREAMTOFIFO	0b011
#define L3G4200D_FIFO_CTRL_MODE_BYPASSTOSTREAM	0b100



class L3G4200D {
    public:
        L3G4200D();
        L3G4200D(uint8_t address);

        void initialize();
        bool testConnection();

	    uint8_t getDeviceID();

	    void getRotations(int16_t* x, int16_t* y, int16_t* z);
        void getRawRotations(uint8_t *_buffer);
        int16_t getPitch();
        int16_t getYaw();
        int16_t getRoll();


        void reset();               
        bool isFIFOEnabled();
        void setFIFOEnabled(bool enabled);
	    uint8_t getFIFOMode();
	    void setFIFOMode(uint8_t mode);

        uint8_t getRate();
        void setRate(uint8_t rate);

	    uint8_t getScale();
	    void setScale(uint8_t scale);

	    uint8_t getPowerMode();
	    void setPowerMode(uint8_t powerMode);

        uint8_t getHPFMode();
        void setHPFMode(uint8_t hpfMode);

        uint8_t getHPFCutOffFrequency();
        void setHPFCutOffFrequency(uint8_t freq);
        
	    bool isOutHighLowFiltered();
	    void setOutHighLowFiltered();
	    bool isOutLowFiltered();
	    void setOutLowFiltered();
	    bool isOutHighFiltered();
	    void setOutHighFiltered();
	    bool isOutNONFiltered();
	    void setOutNONFiltered();

	    bool isHighLowFilteredInterrupt();
	    void setHighLowFilteredInterrupt();
	    bool isLowFilteredInterrupt();
	    void setLowFilteredInterrupt();
	    bool isHighFilteredInterrupt();
	    void setHighFilteredInterrupt();
	    bool isNONFilteredInterrupt();
	    void setNONFilteredInterrupt();

	    bool isDRDYInterrupt();
	    void setDRDYInterrupt(bool enable);
        void printRegister();

    private:
        uint8_t devAddr;
	    uint8_t buffer[6];
        int16_t pitch, yaw, roll;
};

#endif
