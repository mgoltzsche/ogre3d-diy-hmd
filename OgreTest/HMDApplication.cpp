#include "HMDApplication.h"

using namespace Ogre;

//-------------------------------------------------------------------------------------
HMDApplication::HMDApplication(void) {
}
//-------------------------------------------------------------------------------------
HMDApplication::~HMDApplication(void) {
}

//-------------------------------------------------------------------------------------
void HMDApplication::createScene(void) {
	setupLight();
	setupTerrain();

	//mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox"); // missing file

	// create two ogre head entities and attach each to its SceneNode to be rendered
	Entity* head1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
	SceneNode* head1Node = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode1", Vector3(50, 50, 0));
	head1Node->attachObject(head1);

	Entity* head2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
	SceneNode* head2Node = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode2", Vector3(-50, 20, 0));
	head2Node->attachObject(head2);
	head2Node->scale(0.5f, 0.5f, 0.5f);
	head2Node->yaw(Ogre::Degree(-45));

	// create Ninja
	Ogre::Entity* ninja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	SceneNode* ninjaNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
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
	mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode")->attachObject(ground);
}

void HMDApplication::setupLight(void) {
	// configure ambient light
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mSceneMgr->createLight("PointLight")->setPosition(1020, 2000, 2000);

	Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setType(Light::LT_SPOTLIGHT);
	spotLight->setDiffuseColour(1.0, 0, 0);
	spotLight->setSpecularColour(1.0, 0, 0);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Vector3(300, 300, 0));
	spotLight->setSpotlightRange(Degree(10), Degree(30));

	// configure directional light
	Vector3 direction(0.55, -0.3, 0.75);
	direction.normalise();

	mDirectionalLight = mSceneMgr->createLight("DirectionalLight");
	mDirectionalLight->setType(Light::LT_DIRECTIONAL);
	mDirectionalLight->setDirection(direction);
	mDirectionalLight->setDiffuseColour(ColourValue::White);
	mDirectionalLight->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
}

void HMDApplication::setupTerrain(void) {
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
	mTerrainGroup->setOrigin(Vector3::ZERO);

	configureTerrainDefaults();

	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);

	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);

	if (mTerrainsImported) {
		TerrainGroup::TerrainIterator ti =
				mTerrainGroup->getTerrainIterator();
		while (ti.hasMoreElements()) {
			Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();
}

void HMDApplication::configureTerrainDefaults(void) {
	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(mDirectionalLight->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(mDirectionalLight->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img) {
    img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

void HMDApplication::defineTerrain(long x, long y) {
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
	if (ResourceGroupManager::getSingleton().resourceExists(
			mTerrainGroup->getResourceGroup(), filename)) {
		mTerrainGroup->defineTerrain(x, y);
	} else {
		Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainsImported = true;
	}
}

void HMDApplication::initBlendMaps(Ogre::Terrain* terrain) {
	TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
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
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}


// start the application
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
	// Create application object
	HMDApplication app;

	// run application
	try {
		app.go();
	} catch (Ogre::Exception& e) {
		// log exceptions
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: "
				<< e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
