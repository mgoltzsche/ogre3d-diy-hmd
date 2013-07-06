/*
 * OculusCompositorListener.cpp
 *
 *  Created on: 05.07.2013
 *      Author: max
 */

#include "OculusCompositorListener.h"

namespace HMD {

OculusCompositorListener::OculusCompositorListener(HmdConfig *hmdCfg, int factor) {
	mHmdCfg = hmdCfg;
	mFactor = factor;
}

void OculusCompositorListener::notifyMaterialSetup(uint32 passId, MaterialPtr &mat) {
	if (passId == 0)
	    fragmentProgramParams = mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
}

void OculusCompositorListener::notifyMaterialRender(uint32 passId, MaterialPtr &mat) {
	if (passId == 0) {
		fragmentProgramParams->setNamedConstant("HmdWarpParam", mHmdCfg->distortion);
		fragmentProgramParams->setNamedConstant("LensCentre", 0.5f + mFactor * mHmdCfg->projectionCenterOffset / 2.0f);
	}
}

} /* namespace HMD */
