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
	// configure ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));
	mSceneMgr->createLight()->setPosition(20, 80, 50);

	//mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

	// create an ogre head entity and attach it to a node
	Entity* head = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(head);
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

	return 0;
}

#ifdef __cplusplus
}
#endif
