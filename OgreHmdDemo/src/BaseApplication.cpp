/*
 -----------------------------------------------------------------------------
 Filename:    BaseApplication.cpp
 -----------------------------------------------------------------------------

 This source file is part of the
 ___                 __    __ _ _    _
 /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 / \_// (_| | | |  __/  \  /\  /| |   <| |
 \___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
 |___/
 Tutorial Framework
 http://www.ogre3d.org/tikiwiki/
 but modified by Max Goltzsche
 -----------------------------------------------------------------------------
 */
#include "BaseApplication.h"

#define CAMERA "PlayerCam"

using namespace Ogre;

namespace HMD {
//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void) :
		mRoot(0), mSceneMgr(0), mWindow(0), mResourcesCfg(StringUtil::BLANK),
		mPluginsCfg(Ogre::StringUtil::BLANK), mShutDown(false),
		mInputManager(0), mMouse(0), mKeyboard(0),
		mBodyNode(0), mCameraNode(0), mMove(100), mRotate(0.1),
		mCameraRotation(), mDirection() {
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void) {
	// Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void) {
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

		return true;
	} else {
		return false;
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void) {
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCameras(void) {
	Camera *camera = mSceneMgr->createCamera(CAMERA);

	camera->setPosition(Vector3(0, 0, 80));
	camera->lookAt(Vector3(0, 0, -300));
	camera->setNearClipDistance(5);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void) {
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void) {
	// Create one viewport, entire window
	Camera* camera = mSceneMgr->getCamera(CAMERA);
	Viewport* vp = mWindow->addViewport(camera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	camera->setAspectRatio(
			Real(vp->getActualWidth())
					/ Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void) {
	// Load resource paths from config file
	ConfigFile cf;

	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;

	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();

		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;

		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
			// OS X does not set the working directory relative to the app,
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location
			if (!Ogre::StringUtil::startsWith(archName, "/", false))// only adjust relative dirs
			archName = Ogre::String(Ogre::macBundlePath() + "/Contents/Resources/" + archName);
#endif
			ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void) {

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void) {
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void) {
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	Ogre::String workingDir = Ogre::macBundlePath()+"/Contents/Resources/";
	mResourcesCfg = workingDir + mResourcesCfg;
	mPluginsCfg = workingDir + mPluginsCfg;
#endif

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void) {
	mRoot = new Root(mPluginsCfg);

	setupResources();

	if (!configure())
		return false;

	chooseSceneManager();
	createBodyAndCameraNodes();
	createCameras();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();

	// Create the scene
	createScene();
	createFrameListener();

	return true;
}
//-------------------------------------------------------------------------------------
void BaseApplication::createBodyAndCameraNodes() {
	SceneNode* rootNode = mSceneMgr->getRootSceneNode();
	mBodyNode = rootNode->createChildSceneNode("BodyNode", Vector3(0, 50, 200));
	mCameraNode = mBodyNode->createChildSceneNode("CameraNode");
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void) {
	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	mDirection.x = mDirection.y = mDirection.z = 0;

	//Set initial mouse clipping size
	windowResized(mWindow);

	mCameraRotation.x = mCameraRotation.y = mCameraRotation.z = 0;
	mCameraRotation.w = 1;

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const FrameEvent& evt) {
	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	mBodyNode->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
	mCameraNode->setOrientation(mCameraRotation);

	return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed(const OIS::KeyEvent &evt) {
	switch (evt.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
			mDirection.z = -mMove;
			break;
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mDirection.z = mMove;
			break;
		case OIS::KC_LEFT:
		case OIS::KC_A:
			mDirection.x = -mMove;
			break;
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mDirection.x = mMove;
			break;
		case OIS::KC_PGDOWN:
		case OIS::KC_Y:
			mDirection.y = -mMove;
			break;
		case OIS::KC_PGUP:
		case OIS::KC_Q:
			mDirection.y = mMove;
			break;
		case OIS::KC_SYSRQ: // take a screenshot
			mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
			break;
		case OIS::KC_ESCAPE: // exit application
			mShutDown = true;
			break;
	}

	return true;
}

bool BaseApplication::keyReleased(const OIS::KeyEvent &evt) {
	switch (evt.key) {
	case OIS::KC_UP:
	case OIS::KC_W:
		mDirection.z = 0;
		break;
	case OIS::KC_DOWN:
	case OIS::KC_S:
		mDirection.z = 0;
		break;
	case OIS::KC_LEFT:
	case OIS::KC_A:
		mDirection.x = 0;
		break;
	case OIS::KC_RIGHT:
	case OIS::KC_D:
		mDirection.x = 0;
		break;
	case OIS::KC_PGDOWN:
	case OIS::KC_Y:
		mDirection.y = 0;
		break;
	case OIS::KC_PGUP:
	case OIS::KC_Q:
		mDirection.y = 0;
		break;
	}

	return true;
}

bool BaseApplication::mouseMoved(const OIS::MouseEvent &evt) {
	mBodyNode->yaw(Degree(-mRotate * evt.state.X.rel), Node::TS_WORLD);
	mBodyNode->pitch(Degree(-mRotate * evt.state.Y.rel), Node::TS_LOCAL);
	return true;
}

bool BaseApplication::mousePressed(const OIS::MouseEvent &evt,
		OIS::MouseButtonID id) {
	return true;
}

bool BaseApplication::mouseReleased(const OIS::MouseEvent &evt,
		OIS::MouseButtonID id) {
	return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(RenderWindow* rw) {
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(RenderWindow* rw) {
	//Only close for window that created OIS (the main window in these demos)
	if (rw == mWindow) {
		if (mInputManager) {
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
}
