#include "DualViewApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#   include "AppDelegate.h"
#endif

using namespace Ogre;

#define CAMERA_NAME "SceneCamera"

/*typedef union {
  float floatingPoint;
  char binary[4];
} binaryFloat;

binaryFloat x = ;*/

DualViewApplication::DualViewApplication(void) :
		mPrimarySceneMgr(0), mSecondarySceneMgr(0), mDual(false) {
}

DualViewApplication::~DualViewApplication(void) {
}

//Local Functions
void DualViewApplication::setupViewport(Ogre::SceneManager *curr) {
	mWindow->removeAllViewports();

	Ogre::Camera *cam = curr->getCamera(CAMERA_NAME); //The Camera
	Ogre::Viewport *vp = mWindow->addViewport(cam); //Our Viewport linked to the camera

	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void DualViewApplication::dualViewport(Ogre::SceneManager *primarySceneMgr,
		Ogre::SceneManager *secondarySceneMgr) {
	mWindow->removeAllViewports();

	Ogre::Viewport *vp = 0;
	Ogre::Camera *cam = primarySceneMgr->getCamera(CAMERA_NAME);
	vp = mWindow->addViewport(cam, 0, 0, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	cam = secondarySceneMgr->getCamera(CAMERA_NAME);
	vp = mWindow->addViewport(cam, 1, 0.5, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void DualViewApplication::createScene(void) {
	// Set up the space SceneManager
	mPrimarySceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	// Set up the Cloudy SceneManager
	mSecondarySceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	createScene(mPrimarySceneMgr);
	createScene(mSecondarySceneMgr);
}

void DualViewApplication::createScene(SceneManager* scene) {
	Entity* head1 = scene->createEntity("Head1", "ogrehead.mesh");
	SceneNode* head1Node = scene->getRootSceneNode()->createChildSceneNode("HeadNode1", Vector3(50, 50, 0));
	head1Node->attachObject(head1);

	Entity* head2 = scene->createEntity("Head2", "ogrehead.mesh");
	SceneNode* head2Node = scene->getRootSceneNode()->createChildSceneNode("HeadNode2", Vector3(-50, 20, 0));
	head2Node->attachObject(head2);
	head2Node->scale(0.5f, 0.5f, 0.5f);
	head2Node->yaw(Ogre::Degree(-45));

	// create Ninja
	Ogre::Entity* ninja = scene->createEntity("Ninja", "ninja.mesh");
	SceneNode* ninjaNode = scene->getRootSceneNode()->createChildSceneNode("NinjaNode");
	ninjaNode->attachObject(ninja);
	ninjaNode->scale(0.3f, 0.3f, 0.3f);
	ninjaNode->yaw(Ogre::Degree(130));

	// create ground
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* ground = scene->createEntity("Ground", "ground");
	//ground->setMaterialName("Rockwall.tga");
	ground->setCastShadows(false);
	scene->getRootSceneNode()->createChildSceneNode("GroundNode")->attachObject(ground);
}

void DualViewApplication::chooseSceneManager(void) {
	mPrimarySceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "primary");
	mSecondarySceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "secondary");
}

void DualViewApplication::createCamera() {
	mPrimarySceneMgr->createCamera(CAMERA_NAME);
	mSecondarySceneMgr->createCamera(CAMERA_NAME);
}

void DualViewApplication::createViewports() {
	setupViewport(mPrimarySceneMgr);
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

	return true;
}

bool DualViewApplication::keyPressed(const OIS::KeyEvent &arg) {
	if (arg.key == OIS::KC_ESCAPE) {
		mShutDown = true;
	} else if(arg.key == OIS::KC_V){
	    mDual = !mDual;

	    if (mDual)
		dualViewport(mPrimarySceneMgr, mSecondarySceneMgr);
	    else
		setupViewport(mPrimarySceneMgr);
	}

	return true;
}

bool DualViewApplication::keyReleased(const OIS::KeyEvent &arg) {
	return true;
}

bool DualViewApplication::mouseMoved(const OIS::MouseEvent &arg) {
	return true;
}

bool DualViewApplication::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	return true;
}

bool DualViewApplication::mouseReleased(const OIS::MouseEvent &arg,
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
