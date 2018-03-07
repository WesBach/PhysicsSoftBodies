#include "cBulletPhysicsFactory.h"
#include "cBulletRigidBody.h"
#include "shapes.h"
#include "cBulletPhysicsWorld.h"

namespace nPhysics {

	cBulletPhysicsFactory::~cBulletPhysicsFactory(){}

	cBulletPhysicsFactory::cBulletPhysicsFactory() {

	}

	iPhysicsWorld * cBulletPhysicsFactory::CreateWorld()
	{
		return new cBulletPhysicsWorld();
	}

	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape) {
		return new cBulletRigidBody(desc, shape);
	}

	iShape* cBulletPhysicsFactory::CreateSphere(float radius) {
		return new cBulletSphereShape(radius);
	}

	iShape* cBulletPhysicsFactory::CreatePlane(const glm::vec3& normal, float planeConst) {
		return new cBulletPlaneShape(normal,planeConst);
	}
}