/*
 -----------------------------------------------------------------------------
 Filename:    TerrainApplication.cpp
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
 -----------------------------------------------------------------------------
 */
#include "TerrainApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#   include "AppDelegate.h"
#endif

TerrainApplication::TerrainApplication(void) {
}

TerrainApplication::~TerrainApplication(void) {
}

void TerrainApplication::createScene(void) {
	// setup camera
	mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
	mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
	mCamera->setNearClipDistance(0.1);
	mCamera->setFarClipDistance(50000);

	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(
			Ogre::RSC_INFINITE_FAR_PLANE)) {
		mCamera->setFarClipDistance(0); // enable infinite far clip distance if we can
	}

	// setup light
	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	Ogre::Light* light = mSceneMgr->createLight("tstLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

	// setup global terrain options
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr,
			Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"),
			Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	configureTerrainDefaults(light);

	// define terrains
	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);

	mTerrainGroup->loadAllTerrains(true);

	// init blendmaps on every terrain
	if (mTerrainsImported) {
		Ogre::TerrainGroup::TerrainIterator ti =
				mTerrainGroup->getTerrainIterator();
		while (ti.hasMoreElements()) {
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();
}

void TerrainApplication::destroyScene(void) {
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;
}

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img) {
	img.load("terrain.png",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
}

void TerrainApplication::defineTerrain(long x, long y) {
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(
			mTerrainGroup->getResourceGroup(), filename)) {
		mTerrainGroup->defineTerrain(x, y);
	} else {
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainsImported = true;
	}
}

void TerrainApplication::initBlendMaps(Ogre::Terrain* terrain) {
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y) {
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x) {
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real) 0, (Ogre::Real) 1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real) 0, (Ogre::Real) 1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void TerrainApplication::configureTerrainDefaults(Ogre::Light* light) {
	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp =
			mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back(
			"dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back(
			"dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back(
			"grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back(
			"grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back(
			"growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back(
			"growth_weirdfungus-03_normalheight.dds");
}

void TerrainApplication::createFrameListener(void) {
	BaseApplication::createFrameListener();

	mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}

bool TerrainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt) {
	bool ret = BaseApplication::frameRenderingQueued(evt);

	if (mTerrainGroup->isDerivedDataUpdateInProgress()) {
		mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
		mInfoLabel->show();
		if (mTerrainsImported) {
			mInfoLabel->setCaption("Building terrain, please wait...");
		} else {
			mInfoLabel->setCaption("Updating textures, patience...");
		}
	} else {
		mTrayMgr->removeWidgetFromTray(mInfoLabel);
		mInfoLabel->hide();
		if (mTerrainsImported) {
			mTerrainGroup->saveAllTerrains(true);
			mTerrainsImported = false;
		}
	}

	return ret;
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
	TerrainApplication app;

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
