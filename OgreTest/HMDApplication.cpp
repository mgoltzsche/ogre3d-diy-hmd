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
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	    plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	//ground->setMaterialName("Rockwall.tga");
	ground->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode")->attachObject(ground);
}

void HMDApplication::setupLight(void) {
	// configure ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.05f, 0.05f, 0.05f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneMgr->createLight("PointLight")->setPosition(1020, 2000, 2000);
	Ogre::Light* spotLight = mSceneMgr->createLight("SpotLight");
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDiffuseColour(1.0, 0, 0);
	spotLight->setSpecularColour(1.0, 0, 0);
	spotLight->setDirection(-1, -1, 0);
	spotLight->setPosition(Ogre::Vector3(300, 300, 0));
	spotLight->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(30));

	// configure directional light
	Vector3 direction(0.55, -0.3, 0.75);
	direction.normalise();

	mDirectionalLight = mSceneMgr->createLight("DirectionalLight");
	mDirectionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	mDirectionalLight->setDirection(direction);
	mDirectionalLight->setDiffuseColour(Ogre::ColourValue::White);
	mDirectionalLight->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
}

void HMDApplication::setupTerrain(void) {
	//mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	/*mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);*/

	//configureTerrainDefaults();
}

void HMDApplication::configureTerrainDefaults(void) {

}

void HMDApplication::defineTerrain(long x, long y) {

}

void HMDApplication::initBlendMaps(Ogre::Terrain* terrain) {

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
