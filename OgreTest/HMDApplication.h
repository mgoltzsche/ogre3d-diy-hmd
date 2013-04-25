#ifndef __HMDApplication_h_
#define __HMDApplication_h_

#include "BaseApplication.h"

class HMDApplication : public BaseApplication
{
public:
    HMDApplication(void);
    virtual ~HMDApplication(void);

protected:
    virtual void createScene(void);
};

#endif
