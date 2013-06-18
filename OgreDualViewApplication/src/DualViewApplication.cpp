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
		mBodyNode(0), mCameraNode(0), mCameraRotation(), mMove(70), mRotate(0.1), mDirection() {
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
	//MotionTracker::create(mCameraRotation.ptr());

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}

//Local Functions
void DualViewApplication::createScene(void) {
	setupLight();

	// Set up the cloudy skydome
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	SceneNode* rootNode = mSceneMgr->getRootSceneNode();

	Entity* head1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
	SceneNode* head1Node = rootNode->createChildSceneNode("HeadNode1", Vector3(50, 50, 0));
	head1Node->attachObject(head1);

	Entity* head2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
	SceneNode* head2Node = rootNode->createChildSceneNode("HeadNode2", Vector3(-50, 20, 0));
	head2Node->attachObject(head2);
	head2Node->scale(0.5f, 0.5f, 0.5f);
	head2Node->yaw(Ogre::Degree(-45));

	// create Ninja
	Entity* ninja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	SceneNode* ninjaNode = rootNode->createChildSceneNode("NinjaNode");
	ninjaNode->attachObject(ninja);
	ninjaNode->scale(0.3f, 0.3f, 0.3f);
	ninjaNode->yaw(Degree(130));

	// create ground
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500, 1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);

	Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	ground->setMaterialName("Examples/Rockwall");
	ground->setCastShadows(false);
	rootNode->createChildSceneNode("GroundNode")->attachObject(ground);
}

void DualViewApplication::setupLight(void) {
	// configure ambient light
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->createLight("PointLight")->setPosition(1020, 2000, 2000);

	Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setType(Light::LT_SPOTLIGHT);
	spotLight->setDiffuseColour(0.2, 0.3, 1.0);
	spotLight->setSpecularColour(1.0, 0, 0);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Vector3(300, 300, 0));
	spotLight->setSpotlightRange(Degree(10), Degree(30));

	// configure directional light
	Vector3 direction(0.55, -0.3, 0.75);
	direction.normalise();

	Light* directionalLight = mSceneMgr->createLight("DirectionalLight");
	directionalLight->setType(Light::LT_DIRECTIONAL);
	directionalLight->setDirection(direction);
	directionalLight->setDiffuseColour(ColourValue(0.2, 0.2, 0.2));
	directionalLight->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	mSceneMgr->setAmbientLight(ColourValue(0.1, 0.1, 0.1));
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

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

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

	mKeyboard->capture();
	mMouse->capture();

	mBodyNode->translate(mDirection * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
//	printf("pitch: %f\tyaw: %f\troll: %f\n", mCameraRotation->getPitch().valueDegrees(), mCameraRotation->getYaw().valueDegrees(),mCameraRotation->getRoll().valueDegrees());
	//mCameraNode->setOrientation(*mCameraRotation);
	mCameraNode->rotate(mCameraRotation, Ogre::Node::TS_LOCAL);

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
	mBodyNode->yaw(Degree(-mRotate * evt.state.X.rel), Node::TS_WORLD);
	mBodyNode->pitch(Degree(-mRotate * evt.state.Y.rel), Node::TS_LOCAL);
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
