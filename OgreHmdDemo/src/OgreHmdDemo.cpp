#include "OgreHmdDemo.h"
#include "OculusCompositorListener.h"
#include "MotionTracker/MotionTracker.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#   include "AppDelegate.h"
#endif

using namespace Ogre;

#define CAMERA_LEFT "LeftCamera"
#define CAMERA_RIGHT "RightCamera"
#define COMPOSITOR_LEFT "OculusLeft"
#define COMPOSITOR_RIGHT "OculusRight"

namespace HMD {

OgreHmdDemo::OgreHmdDemo() :
		mHmdCfg(), mLeftViewport(0), mRightViewport(0),
		mLeftCompositorListener(0), mRightCompositorListener(0) {
	mHmdCfg.projectionCenterOffset = 0.13f;
	mHmdCfg.interpupillaryDistance = 0.064f;
	mHmdCfg.eyeToScreenDistance = 0.068f;
	mHmdCfg.distortion.x = 1.0f;
	mHmdCfg.distortion.y = 0.22f;
	mHmdCfg.distortion.z = 0.24f;
	mHmdCfg.distortion.w = 0;
	mHmdCfg.scale.x = 0.3;
	mHmdCfg.scale.y = 0.343;
	mHmdCfg.scaleIn.x = 2;
	mHmdCfg.scaleIn.y = 2;
}

OgreHmdDemo::~OgreHmdDemo() {
	delete mLeftCompositorListener;
	delete mRightCompositorListener;
}

void OgreHmdDemo::go() {
	// start MotionTracker
	try {
		MotionTracker::create(&mCameraRotation);
	} catch(std::exception & e) {
		printf("Error while connecting to MotionTracker: ", e.what());
	}

	BaseApplication::go();
}

//Local Functions
void OgreHmdDemo::createScene() {
	setupLight();
	setupHmdPostProcessing();

	// Set up the cloudy skydome
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	SceneNode* rootNode = mSceneMgr->getRootSceneNode();

	// add ogre heads
	Entity* head1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
	SceneNode* head1Node = rootNode->createChildSceneNode("HeadNode1", Vector3(50, 50, 0));
	head1Node->attachObject(head1);

	Entity* head2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
	SceneNode* head2Node = rootNode->createChildSceneNode("HeadNode2", Vector3(-50, 20, 0));
	head2Node->attachObject(head2);
	head2Node->scale(0.5f, 0.5f, 0.5f);
	head2Node->yaw(Ogre::Degree(-45));

	// add houses
	Vector3 housePositions[4] = {Vector3(1000, 500, 300), Vector3(-1000, 500, 500), Vector3(-300, 500, -900), Vector3(900, 500, -900)};

	for (int i = 0; i < 4; i++) {
		Entity* house = mSceneMgr->createEntity("tudorhouse.mesh");
		house->setMaterialName("Examples/TudorHouse");
		SceneNode* houseNode = rootNode->createChildSceneNode("HouseNode" + i, housePositions[i]);

		if (i % 2 != 0)
			houseNode->yaw(Degree(90), Node::TS_LOCAL);

		houseNode->attachObject(house);
	}

	// create ground
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			7000, 7000, 50, 50, true, 1, 5, 5, Vector3::UNIT_Z);

	Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	ground->setMaterialName("Examples/Rockwall");
	ground->setCastShadows(false);
	rootNode->createChildSceneNode("GroundNode")->attachObject(ground);
}

void OgreHmdDemo::setupHmdPostProcessing() {
	MaterialPtr matLeft = MaterialManager::getSingleton().getByName("Ogre/Compositor/Oculus");
	MaterialPtr matRight = matLeft->clone("Ogre/Compositor/Oculus/Right");
	GpuProgramParametersSharedPtr pParamsLeft = matLeft->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	GpuProgramParametersSharedPtr pParamsRight = matRight->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	pParamsLeft->setNamedConstant("Scale", mHmdCfg.scale);
	pParamsRight->setNamedConstant("Scale", mHmdCfg.scale);
	pParamsLeft->setNamedConstant("ScaleIn", mHmdCfg.scaleIn);
	pParamsRight->setNamedConstant("ScaleIn", mHmdCfg.scaleIn);
	pParamsLeft->setNamedConstant("HmdWarpParam", mHmdCfg.distortion);
	pParamsRight->setNamedConstant("HmdWarpParam", mHmdCfg.distortion);
	pParamsLeft->setNamedConstant("LensCentre", 0.5f + mHmdCfg.projectionCenterOffset / 2.0f);
	pParamsRight->setNamedConstant("LensCentre", 0.5f - mHmdCfg.projectionCenterOffset / 2.0f);

	CompositorManager &compositorMngr = CompositorManager::getSingleton();

	CompositorPtr comp = compositorMngr.getByName("OculusRight");
	comp->getTechnique(0)->getOutputTargetPass()->getPass(0)->setMaterialName("Ogre/Compositor/Oculus/Right");

	CompositorInstance* leftComp = compositorMngr.addCompositor(mLeftViewport, COMPOSITOR_LEFT);
	CompositorInstance* rightComp = compositorMngr.addCompositor(mRightViewport, COMPOSITOR_RIGHT);

	mLeftCompositorListener = new OculusCompositorListener(&mHmdCfg, 1);
	mRightCompositorListener = new OculusCompositorListener(&mHmdCfg, -1);

	leftComp->addListener(mLeftCompositorListener);
	rightComp->addListener(mRightCompositorListener);
	leftComp->setEnabled(true);
	rightComp->setEnabled(true);
}

void OgreHmdDemo::setupLight() {
	// configure ambient light
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->createLight("PointLight")->setPosition(1020, 2000, 2000);

	Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setType(Light::LT_SPOTLIGHT);
	spotLight->setDiffuseColour(0.3, 0.3, 0.3);
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

void OgreHmdDemo::createCameras() {
	mCameraNode->attachObject(createCamera(CAMERA_LEFT, -1));
	mCameraNode->attachObject(createCamera(CAMERA_RIGHT, 1));
}

Camera* OgreHmdDemo::createCamera(const String &name, int factor) {
	Camera* camera = mSceneMgr->createCamera(name);

	camera->setPosition(mHmdCfg.interpupillaryDistance * 0.5 * factor, 0, 0);
	camera->lookAt(Vector3(0, 0, -300));
	camera->setNearClipDistance(mHmdCfg.eyeToScreenDistance);
	camera->setFarClipDistance(10000);
	camera->setFOVy(Radian(Degree(110)));

	Matrix4 proj = Matrix4::IDENTITY;
	proj.setTrans(Vector3(-mHmdCfg.projectionCenterOffset * factor, 0, 0));
	camera->setCustomProjectionMatrix(true, proj * camera->getProjectionMatrix());

	return camera;
}

void OgreHmdDemo::createViewports() {
	Camera *cam = mSceneMgr->getCamera(CAMERA_LEFT);
	mLeftViewport = mWindow->addViewport(cam, 0, 0, 0, 0.5, 1);
	mLeftViewport->setBackgroundColour(ColourValue::Black);
	cam->setAspectRatio(
			Real(mLeftViewport->getActualWidth())
			/ Real(mLeftViewport->getActualHeight()));

	cam = mSceneMgr->getCamera(CAMERA_RIGHT);
	mRightViewport = mWindow->addViewport(cam, 1, 0.5, 0, 0.5, 1);
	mRightViewport->setBackgroundColour(ColourValue::Black);
	cam->setAspectRatio(
			Real(mRightViewport->getActualWidth())
			/ Real(mRightViewport->getActualHeight()));
}

bool OgreHmdDemo::keyPressed(const OIS::KeyEvent &evt) {
	BaseApplication::keyPressed(evt);

	switch (evt.key) {
	case OIS::KC_1:
		mHmdCfg.distortion.x += 0.01;
		break;
	case OIS::KC_2:
		mHmdCfg.distortion.x -= 0.01;
		break;
	case OIS::KC_3:
		mHmdCfg.distortion.y += 0.01;
		break;
	case OIS::KC_4:
		mHmdCfg.distortion.y -= 0.01;
		break;
	case OIS::KC_5:
		mHmdCfg.distortion.z += 0.01;
		break;
	case OIS::KC_6:
		mHmdCfg.distortion.z -= 0.01;
		break;
	case OIS::KC_7:
		mHmdCfg.distortion.w += 0.01;
		break;
	case OIS::KC_8:
		mHmdCfg.distortion.w -= 0.01;
		break;
	}

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
	OgreHmdDemo app;

	try {
		// run application
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
}
