#ifndef _MOTIONTRACKER_H_
#define _MOTIONTRACKER_H_

#include <OgreRoot.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
using namespace Ogre;
using namespace ::boost::asio;

class MotionTracker {
	public:
		void read();
		static void create(Quaternion *_output);

    private:
        Quaternion* output;
        serial_port* serial;
        short avarageZeroRateLevelIndex;
        float gyroZeroRateLevelX, gyroZeroRateLevelY, gyroZeroRateLevelZ;
        double gyroScaleFactor;

        MotionTracker(Quaternion* _output);
        ~MotionTracker();

        short convert(unsigned char lsb, unsigned char msb);
        void assignValues(char *_values);

        void normalise(double *_x, double *_y, double *_z);
        double length(double _x, double _y, double _z);
        bool initialiseCommunication();



};

#endif
