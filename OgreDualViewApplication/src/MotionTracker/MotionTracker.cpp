/*
 * MotionTracker.cpp
 *
 *  Created on: 16.06.2013
 *      Author: dlx
 */

#include "MotionTracker.h"
#include <math.h>

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

MotionTracker::MotionTracker(Quaternion *_output) {
	output = _output;

	serial_port_base::baud_rate BAUD(38400);
	serial_port_base::parity PARITY(serial_port_base::parity::none);
	serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);

	io_service io;

	serial = new serial_port(io, "/dev/ttyACM0");

	serial->set_option(BAUD);
	serial->set_option(PARITY);
	serial->set_option(STOP);
}

MotionTracker::~MotionTracker() {
	delete output;
}

void MotionTracker::read() {
	int length = 22;
	char r[length];

	boost::asio::read(*serial, buffer(&r, length), transfer_at_least(length));
	assignValues(r);

}

void MotionTracker::assignValues(char *_values) {
	double scaleRate = convert(_values[18], _values[19]);
	double timeDelta = 1.0 / convert(_values[20], _values[21]);

	if (scaleRate == 2000) {
		scaleRate = 70.0 / 1000;
	} else if (scaleRate == 500) {
		scaleRate = 17.50 / 1000;
	} else {
		scaleRate = 8.75 / 1000;
	}

	double pitch = toRadian(
			convert(_values[0], _values[1]) * scaleRate);
	double yaw = toRadian(
			convert(_values[2], _values[3]) * scaleRate);
	double roll = toRadian(
			convert(_values[4], _values[5]) * scaleRate);
	double accX = convert(_values[6], _values[7]);
	double accY = convert(_values[8], _values[9]);
	double accZ = convert(_values[10], _values[11]);
	double magX = convert(_values[12], _values[13]);
	double magY = convert(_values[14], _values[15]);

	double magZ = convert(_values[16], _values[17]);

	Vector3 axes(pitch, yaw, roll);
	Quaternion currentRot(Radian(axes.length()*timeDelta), axes);

	currentRot.normalise();
	currentRot = *output * currentRot;
	output->swap(currentRot);
}

short MotionTracker::convert(unsigned char lsb, unsigned char msb) {
	short output = 0x0000;
	output = output | msb;
	output = (output << 8) | lsb;
	return output;
}

double MotionTracker::toRadian(double _degree) {
	return _degree * (M_PI / 180);
}

