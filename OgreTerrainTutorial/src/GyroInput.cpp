/*
 * GyroInput.cpp
 *
 *  Created on: 10.06.2013
 *      Author: max
 */
#include "GyroInput.h"
#include <boost/asio.hpp>

using namespace ::boost::asio;

GyroInput::GyroInput(const char* serialConsoleName, int baudRate) {
	data = new GyroData();

	serial_port_base::baud_rate BAUD(baudRate);
	serial_port_base::parity PARITY(serial_port_base::parity::none);
	serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);

	io_service io;

	serial = new serial_port(io, serialConsoleName);

	serial->set_option(BAUD);
	serial->set_option(PARITY);
	serial->set_option(STOP);
}

GyroInput::~GyroInput() {
	delete serial;
	delete data;
}

void GyroInput::readAsync() {
	//write(serial, buffer("#", 1));

	while (true) {
		int length = 12;
		char r[length];

		int size = read(*serial, buffer(&r, length), transfer_at_least(length));

		for (int i = 0; i < length; i++)
			printf(" %d", r[i]);

		printf("\n");
	}
}
