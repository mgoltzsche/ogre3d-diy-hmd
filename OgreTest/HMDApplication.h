#ifndef __HMDApplication_h_
#define __HMDApplication_h_

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "BaseApplication.h"

class HMDApplication: public BaseApplication {
public:
	HMDApplication(void);
	virtual ~HMDApplication(void);

protected:
	virtual void createScene(void);
	virtual void setupLight(void);
	virtual void setupTerrain(void);

private:
	Ogre::Light* mDirectionalLight;
	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::TerrainGroup* mTerrainGroup;
	bool mTerrainsImported;

	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	void configureTerrainDefaults(void);
};

#endif
