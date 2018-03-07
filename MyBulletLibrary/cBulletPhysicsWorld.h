#ifndef _cBulletPhysicsWorld_HG_
#define _cBulletPhysicsWorld_HG_
#include <iPhysicsWorld.h>
#include <bullet\btBulletDynamicsCommon.h>

namespace nPhysics {

	class cBulletPhysicsWorld : public iPhysicsWorld {
	public:
		cBulletPhysicsWorld();
		~cBulletPhysicsWorld();

		void TimeStep(double time, float deltaTime);
		void AddRigidBody(iRigidBody* rigidBody);
		void RemoveRigidBody(iRigidBody* rigidBody);
		void setIntegrationType(IntegrationType theType);

	protected:
		btDefaultCollisionConfiguration* mCollisionConfiguration;// = new btDefaultCollisionConfiguration();
		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* mDispatcher; //= new btCollisionDispatcher(collisionConfiguration);
		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* mOverlappingPairCache;// = new btDbvtBroadphase();
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* mSolver;// = new btSequentialImpulseConstraintSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;// = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	};
}

#endif // !_cBulletPhysicsFactory_HG_
