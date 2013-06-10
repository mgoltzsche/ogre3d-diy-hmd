#ifndef __DualViewApplication_h_
#define __DualViewApplication_h_

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "BaseApplication.h"

class DualViewApplication: public BaseApplication {
public:
	DualViewApplication(void);
	virtual ~DualViewApplication(void);

protected:
	virtual void createScene(void);
	virtual void createScene(Ogre::SceneManager*);
	virtual void chooseSceneManager(void);
	virtual void createCamera(void);
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
	Ogre::SceneManager* mPrimarySceneMgr;
	Ogre::SceneManager* mSecondarySceneMgr;
	bool mDual;

	virtual void setupViewport(Ogre::SceneManager *curr);
	virtual void dualViewport(Ogre::SceneManager *primarySceneMgr, Ogre::SceneManager *secondarySceneMgr);
};

#endif // #ifndef __DualViewApplication_h_
