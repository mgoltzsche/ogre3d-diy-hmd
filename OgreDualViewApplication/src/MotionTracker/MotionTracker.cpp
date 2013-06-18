/*
 * MotionTracker.cpp
 *
 *  Created on: 16.06.2013
 *      Author: dlx
 */

#include "MotionTracker.h"

static void startReading(MotionTracker* _mt) {
	while (true) {
		_mt->read();
	}
}

void MotionTracker::create(Quaternion *_output) {
	MotionTracker* mt = new MotionTracker(_output);
	boost::thread thread(startReading, mt);
	thread.detach();
}

MotionTracker::MotionTracker(Quaternion *_output) :
		gyroScaleFactor(0.070), gyroZeroRateLevelX(0), gyroZeroRateLevelY(0), gyroZeroRateLevelZ(
				0), avarageZeroRateLevelIndex(0) {
	output = _output;

	serial_port_base::baud_rate BAUD(38400);
	serial_port_base::parity PARITY(serial_port_base::parity::none);
	serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);

	io_service io;

	serial = new serial_port(io, "/dev/ttyACM0");

	serial->set_option(BAUD);
	serial->set_option(PARITY);
	serial->set_option(STOP);

	char startCommand[1] = { 0xFF };
	write(*serial, buffer(startCommand, 1));
}

MotionTracker::~MotionTracker() {
	delete output;
}

void MotionTracker::read() {
	int length = 18;
	char r[length];

	boost::asio::read(*serial, buffer(&r, length), transfer_at_least(length));

	if (avarageZeroRateLevelIndex <= 500) {
		avarageZeroRateLevelIndex++;
		gyroZeroRateLevelX += convert(r[0], r[1]);
		gyroZeroRateLevelY += convert(r[2], r[3]);
		gyroZeroRateLevelZ += convert(r[4], r[5]);
		if (avarageZeroRateLevelIndex == 500) {
			gyroZeroRateLevelX /= avarageZeroRateLevelIndex;
			gyroZeroRateLevelY /= avarageZeroRateLevelIndex;
			gyroZeroRateLevelZ /= avarageZeroRateLevelIndex;
			printf("noiseReduction x: %.3f y: %.3f z: %.3f\n",gyroZeroRateLevelX,gyroZeroRateLevelY,gyroZeroRateLevelZ);
		}
	} else {
		assignValues(r);
	}
}

void MotionTracker::assignValues(char *_values) {
	double pitch = (convert(_values[0], _values[1]) - gyroZeroRateLevelX)
			* gyroScaleFactor;
	double yaw = (convert(_values[2], _values[3]) - gyroZeroRateLevelY)
			* gyroScaleFactor;
	double roll = (convert(_values[4], _values[5]) - gyroZeroRateLevelZ)
			* gyroScaleFactor;
	double accX = convert(_values[6], _values[7]);
	double accY = convert(_values[8], _values[9]);
	double accZ = convert(_values[10], _values[11]);
	double magX = convert(_values[12], _values[13]);
	double magY = convert(_values[14], _values[15]);
	double magZ = convert(_values[16], _values[17]);

	printf(
			"pitch: %.3f\tyaw: %.3f\troll: %.3f\taccX: %.3f\taccY: %.3f\taccZ: %.3f\n",
			pitch, yaw, roll, accX, accY, accZ);
	normalise(&pitch, &yaw, &roll);
	Quaternion currentRot(length(pitch, yaw, roll), pitch, yaw, roll);
	//output = new Quaternion(length(pitch, yaw, roll), pitch, yaw, roll);
	//printf("pitch: %.3f\tyaw: %.3f\troll: %.3f\n",pitch, yaw, roll);
	//currentRot = currentRot * (*output);
	currentRot.normalise();
	output->swap(currentRot);

}

short MotionTracker::convert(unsigned char lsb, unsigned char msb) {
	short output = 0x0000;
	output = output | msb;
	output = (output << 8) | lsb;
	return output;
}

void MotionTracker::normalise(double *_x, double *_y, double *_z) {
	double l = length(*_x, *_y, *_z);
	*_x = *_x / l;
	*_y = *_y / l;
	*_z = *_z / l;
}

double MotionTracker::length(double _x, double _y, double _z) {
	return sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2));
}

