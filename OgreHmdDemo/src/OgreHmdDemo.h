#ifndef __DualViewApplication_h_
#define __DualViewApplication_h_

#include "BaseApplication.h"
#include "HmdConfig.h"
#include "OculusCompositorListener.h"

using namespace Ogre;

namespace HMD {

class OgreHmdDemo: public BaseApplication {
public:
	OgreHmdDemo(void);
	virtual ~OgreHmdDemo(void);
	virtual void go(void);

protected:
	virtual void createScene(void);
	virtual void createCameras(void);
	virtual void createViewports(void);

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg);

private:
	HmdConfig mHmdCfg;
	Viewport* mLeftViewport;
	Viewport* mRightViewport;
	OculusCompositorListener* mLeftCompositorListener;
	OculusCompositorListener* mRightCompositorListener;
	Camera* createCamera(const String &name, int factor);
	void setupLight(void);
	void setupHmdPostProcessing(void);
};
}
#endif // #ifndef __DualViewApplication_h_
