#include "cPhysicsFactory.h"
#include "cRigidBody.h"
#include "cSoftBody.h"
#include "shapes.h"
#include "cPhysicsWorld.h"

namespace nPhysics
{
	cPhysicsFactory::~cPhysicsFactory() {}

	iPhysicsWorld* cPhysicsFactory::CreateWorld(){
		return new cPhysicsWorld();
	}

	iCollisionBody* cPhysicsFactory::CreateRigidBody(const sRigidBodyDesc& desc, iShape* shape){
		return new cRigidBody(desc, shape);
	}

	iCollisionBody* cPhysicsFactory::CreateSoftBody(const sSoftBodyDesc& desc) {
		return new cSoftBody(desc);
	}


	iShape* cPhysicsFactory::CreateSphere(float radius){
		return new cSphereShape(radius);
	}

	iShape* cPhysicsFactory::CreatePlane(const glm::vec3& normal, float planeConst){
		return new cPlaneShape(normal, planeConst);
	}
}