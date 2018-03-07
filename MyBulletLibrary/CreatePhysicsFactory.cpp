#include "CreatePhysicsFactory.h"
#include "cBulletPhysicsFactory.h"

namespace nPhysics {
	EXTERN_DLL_EXPORT iPhysicsFactory* CreateFactory() {
		return new cBulletPhysicsFactory();
	}
}