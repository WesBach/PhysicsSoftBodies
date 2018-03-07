#pragma once
#include "iPhysicsFactory.h"

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory{
	public:
		virtual ~cPhysicsFactory();

		virtual iPhysicsWorld* CreateWorld();

		virtual iCollisionBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape);
		virtual iCollisionBody* CreateSoftBody(const sSoftBodyDesc& desc);

		virtual iShape* CreateSphere(float radius);
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst);
	};
}