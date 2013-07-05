/*
 * OculusCompositorListener.h
 *
 *  Created on: 05.07.2013
 *      Author: max
 */

#ifndef OCULUSCOMPOSITORLISTENER_H_
#define OCULUSCOMPOSITORLISTENER_H_

#include <OgreRoot.h>
#include <OgreCompositorInstance.h>
#include "HmdConfig.h"

namespace HMD {

using namespace Ogre;

class OculusCompositorListener: public Ogre::CompositorInstance::Listener {
public:
	OculusCompositorListener(HmdConfig *hmdCfg, int factor);
private:
	HmdConfig *mHmdCfg;
	int mFactor;
	GpuProgramParametersSharedPtr fragmentProgramParams;
public:
	void notifyMaterialSetup(uint32 passId, MaterialPtr &mat);
	void notifyMaterialRender(uint32 passId, MaterialPtr &mat);
};

} /* namespace HMD */
#endif /* OCULUSCOMPOSITORLISTENER_H_ */
