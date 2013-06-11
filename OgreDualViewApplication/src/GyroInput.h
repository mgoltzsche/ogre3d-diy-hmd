#ifndef __GyroInput_h_
#define __GyroInput_h_

#include <boost/asio.hpp>

struct GyroData {
	int accelerationX;
	int accelerationY;
	int accelerationZ;
	int headingX;
	int headingY;
	int headingZ;
};

class GyroInput {

private:
	boost::asio::serial_port* serial;

public:
	GyroInput(const char*,int);
	~GyroInput();
	void read();
	GyroData* data;
};

#endif // #ifndef __GyroInput_h_
