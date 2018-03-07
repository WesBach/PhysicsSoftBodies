#include "CreatePhysicsFactory.h"

namespace nPhysics {

	EXTERN_DLL_EXPORT iPhysicsFactory* CreateFactory(){
		return new cPhysicsFactory();
	}
}

