#pragma once
#include "iPhysicsWorld.h"
#include <vector>
#include <iRigidBody.h>
#include "cIntegrator.h"

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld{
	public:
		virtual ~cPhysicsWorld();

		virtual void TimeStep(double time, float deltaTime);
		virtual void AddCollisionBody(iCollisionBody* rigidBody);
		virtual void RemoveCollisionBody(iCollisionBody* rigidBody);
		void Collide(iCollisionBody* rigidBodyA, iCollisionBody* rigidBodyB);
		virtual void setIntegrationType(IntegrationType theType);

	private:	
		std::vector<iCollisionBody*> mCollisionBody;
		cIntegrator* mIntegrator;

		float mDeltaTime;
		double mTime;
	};
}