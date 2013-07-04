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

MotionTracker::MotionTracker(Quaternion *_output) :
		driftCounter(0), avAcc(Vector3(0.0)) {
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
	double timeDelta = convert(_values[20], _values[21]) / pow(10, 6);
	//printf("timeDelta %f\n", timeDelta);

	if (scaleRate == 2000) {
		scaleRate = 70.0 / 1000;
	} else if (scaleRate == 500) {
		scaleRate = 17.50 / 1000;
	} else {
		scaleRate = 8.75 / 1000;
	}

	double magX = convert(_values[12], _values[13]);
	double magY = convert(_values[14], _values[15]);
	double magZ = convert(_values[16], _values[17]);

	Vector3 gyro(toRadian(convert(_values[0], _values[1]) * scaleRate),
			toRadian(convert(_values[2], _values[3]) * scaleRate),
			toRadian(convert(_values[4], _values[5]) * scaleRate));

	avAcc = (avAcc
			+ Vector3(convert(_values[6], _values[7]) / 256.0,
					convert(_values[8], _values[9]) / 256.0,
					convert(_values[10], _values[11]) / 256.0)) / 2.0;

	Quaternion currentRot(Radian(gyro.length() * timeDelta), gyro);

	currentRot.normalise();
	currentRot = *output * currentRot;
	//printf("X: %.3f\t%.3f\t%.3f\n", avAcc.x, avAcc.y, avAcc.z);
	//printf("X: %.3f\t%.3f\t%.3f\n", currentRot.yAxis().x, currentRot.yAxis().y, currentRot.yAxis().z);
	if (driftCounter == 10) {
		driftCounter = 0;
		Vector3 tiltAxis(-avAcc.z, 0, avAcc.x);

		tiltAxis.normalise();
		//printf("tiltAxis: %.3f\t%.3f\t%.3f\n", tiltAxis.x,tiltAxis.y,tiltAxis.z);

		Radian driftAngle = avAcc.angleBetween(output->yAxis());

		//Radian driftAngle = Vector3(0.0,1.0,0.0).angleBetween(Vector3(0.0,1.0,0.01));
		printf("driftAngle: %.3f\n", driftAngle.valueDegrees());

		//Quaternion driftRotation(driftAngle, tiltAxis);

		//driftRotation.normalise();
		if (avAcc.angleBetween(output->yAxis()).valueDegrees() > 1)
			currentRot = currentRot.yAxis().getRotationTo(avAcc, tiltAxis)
					* currentRot;
	} else if (gyro.length() < 0.3 && avAcc.length() < 1.1) {
		driftCounter++;
	} else {
		driftCounter = 0;
	}

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
