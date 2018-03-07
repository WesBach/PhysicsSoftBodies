#pragma once
#include "iPhysicsWorld.h"
#include "iRigidBody.h"
#include "iSoftBody.h"
#include "sRigidBodyDesc.h"
#include "sSoftBodyDesc.h"

namespace nPhysics
{
	class iPhysicsFactory{
	public:
		virtual ~iPhysicsFactory() {}

		virtual iPhysicsWorld* CreateWorld() = 0;

		virtual iCollisionBody* CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) = 0;
		virtual iCollisionBody* CreateSoftBody(const sSoftBodyDesc& desc) = 0;

		virtual iShape* CreateSphere(float radius) = 0;
		virtual iShape* CreatePlane(const glm::vec3& normal, float planeConst) = 0;
	};
}