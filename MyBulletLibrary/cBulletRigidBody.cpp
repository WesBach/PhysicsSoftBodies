#include "cBulletRigidBody.h"
#include "nConvert.h"

namespace nPhysics {

	iShape* cBulletRigidBody::GetShape() {
		mBody->activate(true);
		return this->mShape;
	}

	cBulletRigidBody::cBulletRigidBody(const sRigidBodyDesc& desc, iShape* shape) {
		//create a dynamic rigidbody
		mShape = shape;

		btCollisionShape* colShape;
		float radius = 0.0f;
		shape->GetSphereRadius(radius);

		float pConst;
		glm::vec3 normal;
		shape->GetPlaneConst(pConst);
		shape->GetPlaneNormal(normal);

		eShapeType tempShapeType = shape->GetShapeType();
		//Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();


		if (tempShapeType == eShapeType::SHAPE_TYPE_PLANE)
		{
			//colShape = new btBoxShape(btVector3(desc.Extents.x, desc.Extents.y, desc.Extents.z));
			colShape = new btStaticPlaneShape(nConvert::vec3ToBullet(normal),btScalar(0));
			startTransform.setOrigin(btVector3(desc.Position.x, desc.Position.y, desc.Position.z));
		}
		else
		{
			colShape = new btSphereShape(btScalar(radius));
			startTransform.setOrigin(btVector3(desc.Position.x, desc.Position.y, desc.Position.z));
		}

		btScalar mass(desc.Mass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);

		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		this->mMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
		rbInfo.m_linearDamping = 0.5f;
		rbInfo.m_angularDamping = 0.01;
		this->mBody = new btRigidBody(rbInfo);
		mBody->activate(true);
		//mBody->setUserPointer(this);
	}

	void cBulletRigidBody::GetTransform(glm::mat4& transformOut) {
		mBody->activate(true);
		nConvert::ToGLM(this->mBody->getWorldTransform(),transformOut);
	}

	void cBulletRigidBody::GetPosition(glm::vec3& positionOut) {
		mBody->activate(true);
		btTransform trans;
		mBody->getMotionState()->getWorldTransform(trans);
		positionOut = nConvert::vec3ToGLM(trans.getOrigin());
	}

	void cBulletRigidBody::GetRotation(glm::vec3& rotationOut) {
		mBody->activate(true);
		btTransform transform;
		btVector3 rot;
		//make the vec3 a quaternion
		glm::quat& rotation = glm::quat(rotationOut);
		//get the quat values
		mMotionState->getWorldTransform(transform);
		transform.getRotation().getEulerZYX(rot[2], rot[1], rot[0]);

		//nConvert::ToGLM(transform.getRotation(), rotation);
		nConvert::vec3ToGLM(rot, rotationOut);
	}

	void cBulletRigidBody::GetPreviousPosition(glm::vec3& positionOu) {

	}

	void cBulletRigidBody::GetAngularVelocity(glm::vec3& andgularVelOut) {
		mBody->activate(true);
		//andgularVelOut = nConvert::vec3ToGLM(mBody->getAngularVelocity());
	}

	void cBulletRigidBody::GetTorque(glm::vec3& torqueOut) {

	}

	void cBulletRigidBody::GetVelocity(glm::vec3& velOut) {
		mBody->activate(true);
		velOut = nConvert::vec3ToGLM(mBody->getLinearVelocity());
	}

	void cBulletRigidBody::GetInverseMass(glm::vec3& velOut) {

	}

	void cBulletRigidBody::GetMass(float& massOut) {

	}

	void cBulletRigidBody::SetTransform(glm::mat4& transformIn) {
		
	}

	void cBulletRigidBody::SetPosition(glm::vec3& positionIn) {

	} 

	void cBulletRigidBody::SetPreviousPosition(glm::vec3& positionIn) {

	}

	void cBulletRigidBody::SetAngularVelocity(glm::vec3& andgularVelIn) {
		mBody->activate(true);
		btVector3 tempVec =	nConvert::vec3ToBullet(andgularVelIn);
		mBody->setAngularVelocity(tempVec);
	}

	void cBulletRigidBody::SetTorque(glm::vec3& torqueIn) {
		
	}

	void cBulletRigidBody::SetVelocity(glm::vec3& velIn) {
		mBody->activate(true);
		btVector3 imp = nConvert::vec3ToBullet(velIn);
		this->mBody->applyCentralForce(imp);
	}

	void cBulletRigidBody::SetInverseMass(glm::vec3& velIn) {

	}

	void cBulletRigidBody::SetMass(float& massIn) {

	}

	void cBulletRigidBody::ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint) {
		mBody->activate(true);
		btVector3 imp = nConvert::vec3ToBullet(impulse);
		btVector3 relPoint = nConvert::vec3ToBullet(relativePoint);
		mBody->applyImpulse(imp, relPoint);
	}

	void cBulletRigidBody::applyTorque(const glm::vec3& torque) {
		mBody->activate(true);
	}

	void cBulletRigidBody::GetVelocityAtPoint(glm::vec3& relativePoint, glm::vec3& velocityOut) {
		mBody->activate(true);
		btVector3 point = nConvert::vec3ToBullet(relativePoint);
		btVector3 vel = mBody->getVelocityInLocalPoint(point);
		velocityOut = nConvert::vec3ToGLM(vel);
	}

	void cBulletRigidBody::ApplyImpulse(const glm::vec3& impulse) {
		mBody->activate(true);
		btVector3 imp = nConvert::vec3ToBullet(impulse);
		mBody->applyCentralImpulse(imp);
	}

	cBulletRigidBody::~cBulletRigidBody() {
		delete this->mMotionState;
		delete this->mBody;
		delete this->mShape;
		mShape = 0;
	}

	void cBulletRigidBody::SetRotation(glm::vec3& rotationIn) {
		mBody->activate(true);
		btTransform transform;
		//convert vec3 to quat
		glm::quat rotation(rotationIn);
		mMotionState->getWorldTransform(transform);
		//set the rotation
		transform.setRotation(nConvert::ToBullet(rotation));
		mMotionState->setWorldTransform(transform);
	}
}
