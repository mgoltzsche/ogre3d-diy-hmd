#include "DualViewApplication.h"
#include "MotionTracker/MotionTracker.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#   include "AppDelegate.h"
#endif

using namespace Ogre;

#define CAMERA_LEFT "LeftCamera"
#define CAMERA_RIGHT "RightCamera"
#define HSCREEN_SIZE 0.14976f
#define VSCREEN_SIZE 0.0935f
#define EYE_TO_SCREEN_DISTANCE 0.068f
#define IPD 0.064f

const float projectionCenterOffset = 0.14;
const float DEFAULT_DISTORTION[4] = {1.0f, 0.22f, 0.24f, 0};

DualViewApplication::DualViewApplication(void) :
		mBodyNode(0), mCameraNode(0), mCameraRotation(), mMove(100), mRotate(0.1), mDirection(), leftViewport(0), rightViewport(0) {
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
	MotionTracker::create(&mCameraRotation);

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}

//Local Functions
void DualViewApplication::createScene() {
	setupLight();
	setupHmdPostProcessing();

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

void DualViewApplication::setupLight() {
	// configure ambient light
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->createLight("PointLight")->setPosition(1020, 2000, 2000);

	Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setType(Light::LT_SPOTLIGHT);
	spotLight->setDiffuseColour(0.2, 0.2, 0.2);
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

void DualViewApplication::setupHmdPostProcessing() {
	MaterialPtr matLeft = MaterialManager::getSingleton().getByName("Ogre/Compositor/Oculus");
	MaterialPtr matRight = matLeft->clone("Ogre/Compositor/Oculus/Right");
	GpuProgramParametersSharedPtr pParamsLeft = matLeft->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	GpuProgramParametersSharedPtr pParamsRight = matRight->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	Vector4 hmdWarp = Vector4(
			DEFAULT_DISTORTION[0],
			DEFAULT_DISTORTION[1],
			DEFAULT_DISTORTION[2],
			DEFAULT_DISTORTION[3]
	);

	pParamsLeft->setNamedConstant("HmdWarpParam", hmdWarp);
	pParamsRight->setNamedConstant("HmdWarpParam", hmdWarp);
	pParamsLeft->setNamedConstant("LensCentre", 0.5f + projectionCenterOffset / 2.0f);
	pParamsRight->setNamedConstant("LensCentre", 0.5f - projectionCenterOffset / 2.0f);

	CompositorPtr comp = Ogre::CompositorManager::getSingleton().getByName("OculusRight");
	comp->getTechnique(0)->getOutputTargetPass()->getPass(0)->setMaterialName("Ogre/Compositor/Oculus/Right");

	CompositorInstance* leftComp = CompositorManager::getSingleton().addCompositor(leftViewport, "OculusLeft");
	CompositorInstance* rightComp = CompositorManager::getSingleton().addCompositor(rightViewport, "OculusRight");

	leftComp->setEnabled(true);
	rightComp->setEnabled(true);
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

	camera->setPosition(IPD * 0.5 * factor, 0, 0);
	camera->lookAt(Vector3(0, 0, -300));
	camera->setNearClipDistance(EYE_TO_SCREEN_DISTANCE);
	camera->setFarClipDistance(10000);
	camera->setFOVy(Radian(Degree(85)));

	Matrix4 proj = Matrix4::IDENTITY;
	proj.setTrans(Vector3(-projectionCenterOffset * factor, 0, 0));
	camera->setCustomProjectionMatrix(true, proj * camera->getProjectionMatrix());

	return camera;
}

void DualViewApplication::createViewports() {
	Camera *cam = mSceneMgr->getCamera(CAMERA_LEFT);
	leftViewport = mWindow->addViewport(cam, 0, 0, 0, 0.5, 1);
	leftViewport->setBackgroundColour(ColourValue(0, 0, 0));
	cam->setAspectRatio(
			Real(leftViewport->getActualWidth())
					/ Real(leftViewport->getActualHeight()));

	cam = mSceneMgr->getCamera(CAMERA_RIGHT);
	rightViewport = mWindow->addViewport(cam, 1, 0.5, 0, 0.5, 1);
	rightViewport->setBackgroundColour(ColourValue(0, 0, 0));
	cam->setAspectRatio(
			Real(rightViewport->getActualWidth())
					/ Real(rightViewport->getActualHeight()));
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

	mDirection.x = mDirection.y = mDirection.z = 0;

	//Set initial mouse clipping size
	windowResized(mWindow);

	mCameraRotation.x = mCameraRotation.y = mCameraRotation.z = 0;
	mCameraRotation.w = 1;

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
//printf("%.1f  %.1f  %.1f\n", mDirection.x, mDirection.y, mDirection.z);
	mBodyNode->translate(mDirection * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	mCameraNode->setOrientation(mCameraRotation);

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
