#ifndef __DualViewApplication_h_
#define __DualViewApplication_h_

#include "BaseApplication.h"

using namespace Ogre;

class DualViewApplication: public BaseApplication {
public:
	DualViewApplication(void);
	virtual ~DualViewApplication(void);
	virtual void go(void);

protected:
	virtual void createScene(void);
	virtual void createCameras(void);
	virtual void createViewports(void);
	virtual void createFrameListener(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

private:
	SceneNode* mBodyNode;
	SceneNode* mCameraNode;
	Real mRotate;
	Real mMove;
	Vector3 mDirection;
	Quaternion* mCameraRotation;
	Vector2 mBodyRotation;
	Camera* createCamera(const String &name, int factor);
};

#endif // #ifndef __DualViewApplication_h_
