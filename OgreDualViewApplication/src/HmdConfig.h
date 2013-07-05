/*
 * HmdConfig.h
 *
 *  Created on: 05.07.2013
 *      Author: max
 */

#ifndef __HmdConfig_h_
#define __HmdConfig_h_

#include <OgreRoot.h>

struct HmdConfig {
		float projectionCenterOffset;
		float interpupillaryDistance;
		float eyeToScreenDistance;
		Ogre::Vector4 distortion;
		Ogre::Vector3 scale;
		Ogre::Vector3 scaleIn;
};

#endif




