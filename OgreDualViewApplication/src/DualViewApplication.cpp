#include "DualViewApplication.h"
#include "MotionTracker/MotionTracker.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#   include "AppDelegate.h"
#endif

using namespace Ogre;

#define CAMERA_LEFT "LeftCamera"
#define CAMERA_RIGHT "RightCamera"

DualViewApplication::DualViewApplication(void) :
		mBodyNode(0), mCameraNode(0), mCameraRotation(new Quaternion()), mMove(30), mRotate(20), mDirection() {
}

DualViewApplication::~DualViewApplication(void) {
}

void DualViewApplication::go(void) {
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
	MotionTracker::create(mCameraRotation);

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}

//Local Functions
void DualViewApplication::createScene(void) {
	// Set up the cloudy skydome
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	Entity* head1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
	SceneNode* head1Node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			"HeadNode1", Vector3(50, 50, 0));
	head1Node->attachObject(head1);

	Entity* head2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
	SceneNode* head2Node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			"HeadNode2", Vector3(-50, 20, 0));
	head2Node->attachObject(head2);
	head2Node->scale(0.5f, 0.5f, 0.5f);
	head2Node->yaw(Ogre::Degree(-45));

	// create Ninja
	Ogre::Entity* ninja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	SceneNode* ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			"NinjaNode");
	ninjaNode->attachObject(ninja);
	ninjaNode->scale(0.3f, 0.3f, 0.3f);
	ninjaNode->yaw(Ogre::Degree(130));

	// create ground
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	//ground->setMaterialName("Rockwall.tga");
	ground->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode")->attachObject(
			ground);
}

void DualViewApplication::createCameras() {
	SceneNode* rootNode = mSceneMgr->getRootSceneNode();
	mBodyNode = rootNode->createChildSceneNode("BodyNode", Vector3(0, 50, 200));
	mCameraNode = mBodyNode->createChildSceneNode("CameraNode");
	Camera* lftCamera = createCamera(CAMERA_LEFT, -1);
	Camera* rgtCamera = createCamera(CAMERA_RIGHT, 1);

	mCameraNode->attachObject(lftCamera);
	mCameraNode->attachObject(rgtCamera);

}

Camera* DualViewApplication::createCamera(const String &name, int factor) {
	Camera* camera = mSceneMgr->createCamera(name);

	camera->setPosition(10 * factor, 0, 0);
	camera->lookAt(Ogre::Vector3(0, 0, -300));
	camera->setNearClipDistance(5);

	return camera;
}

void DualViewApplication::createViewports() {
	Ogre::Viewport *vp = 0;
	Ogre::Camera *cam1 = mSceneMgr->getCamera(CAMERA_LEFT);
	vp = mWindow->addViewport(cam1, 0, 0, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam1->setAspectRatio(
			Ogre::Real(vp->getActualWidth())
					/ Ogre::Real(vp->getActualHeight()));

	Ogre::Camera *cam2 = mSceneMgr->getCamera(CAMERA_RIGHT);
	vp = mWindow->addViewport(cam2, 1, 0.5, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam2->setAspectRatio(
			Ogre::Real(vp->getActualWidth())
					/ Ogre::Real(vp->getActualHeight()));
}

void DualViewApplication::createFrameListener(void) {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(
			OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(
			OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
}

bool DualViewApplication::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	mBodyNode->translate(mDirection * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
//	printf("pitch: %f\tyaw: %f\troll: %f\n", mCameraRotation->getPitch().valueDegrees(), mCameraRotation->getYaw().valueDegrees(),mCameraRotation->getRoll().valueDegrees());
	mCameraNode->setOrientation(*mCameraRotation);


	return true;
}

bool DualViewApplication::keyPressed(const OIS::KeyEvent &evt) {
	switch (evt.key) {
	case OIS::KC_ESCAPE:
		mShutDown = true;
		break;
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
	}

	return true;
}

bool DualViewApplication::keyReleased(const OIS::KeyEvent &evt) {
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
	default:
		break;
	}

	return true;
}

bool DualViewApplication::mouseMoved(const OIS::MouseEvent &evt) {
	return true;
}

bool DualViewApplication::mousePressed(const OIS::MouseEvent &evt,
		OIS::MouseButtonID id) {
	return true;
}

bool DualViewApplication::mouseReleased(const OIS::MouseEvent &evt,
		OIS::MouseButtonID id) {
	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
		{
#if defined(OGRE_IS_IOS)
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, @"UIApplication", @"AppDelegate");
	[pool release];
	return retVal;
#elif (OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	mAppDelegate = [[AppDelegate alloc] init];
	[[NSApplication sharedApplication] setDelegate:mAppDelegate];
	int retVal = NSApplicationMain(argc, (const char **) argv);

	[pool release];

	return retVal;
#else
	// Create application object
	DualViewApplication app;

	try {
		app.go();
	} catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: "
				<< e.getFullDescription().c_str() << std::endl;
#endif
	}
#endif
	return 0;
}

#ifdef __cplusplus
}
#endif
