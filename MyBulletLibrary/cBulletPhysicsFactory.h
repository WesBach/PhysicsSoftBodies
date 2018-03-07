#ifndef _cBulletPhysicsFactory_HG_
#define _cBulletPhysicsFactory_HG_
#include <iPhysicsFactory.h>

namespace nPhysics {
	class cBulletPhysicsFactory : public iPhysicsFactory{
	public:
		virtual ~cBulletPhysicsFactory();
		cBulletPhysicsFactory();
		virtual iPhysicsWorld* CreateWorld();

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape);

		virtual iShape* CreateSphere(float radius);
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst);
	};
}

#endif // !_cBulletPhysicsFactory_HG_

