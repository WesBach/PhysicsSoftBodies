#include "cBulletPhysicsWorld.h"
#include "nConvert.h"
#include "cBulletRigidBody.h"

namespace nPhysics
{

	cBulletPhysicsWorld::~cBulletPhysicsWorld() {
		delete this->mDynamicsWorld;
		delete this->mSolver;
		delete this->mOverlappingPairCache;
		delete this->mDispatcher;
		delete this->mCollisionConfiguration;
	}

	cBulletPhysicsWorld::cBulletPhysicsWorld() {
		this->mCollisionConfiguration = new btDefaultCollisionConfiguration();
		this->mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
		this->mOverlappingPairCache = new btDbvtBroadphase();
		this->mSolver = new btSequentialImpulseConstraintSolver;
		this->mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
		this->mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	}

	void cBulletPhysicsWorld::TimeStep(double time, float deltaTime) {
		//TODO the time step (call the timestep with dynamicsWorld)
		this->mDynamicsWorld->stepSimulation(deltaTime);
	}

	void cBulletPhysicsWorld::AddRigidBody(iRigidBody* rigidBody) {
		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);

		btRigidBody* btBody = myBody->getBulletRigidBody();
		if (btBody->isInWorld())
		{
			//do nothing
		}
		else
		{
			this->mDynamicsWorld->addRigidBody(btBody);
		}

	}

	void cBulletPhysicsWorld::RemoveRigidBody(iRigidBody* rigidBody) {
		cBulletRigidBody* myBody = dynamic_cast<cBulletRigidBody*>(rigidBody);

		btRigidBody* btBody = myBody->getBulletRigidBody();
		if (btBody->isInWorld())
		{
			//remove body
			this->mDynamicsWorld->removeRigidBody(btBody);
			//delete btBody;
			//btBody = 0;
		}

	}

	void cBulletPhysicsWorld::setIntegrationType(IntegrationType theType) {
		
	}
}

