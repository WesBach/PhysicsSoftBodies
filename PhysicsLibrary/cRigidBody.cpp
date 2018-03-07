#include "cRigidBody.h"

namespace nPhysics
{
	cRigidBody::cRigidBody(const sRigidBodyDesc& desc, iShape* shape)
		: mShape(shape)
		, mPosition(desc.Position)
		, mVelocity(desc.Velocity)
		, mMass(desc.Mass)
		, mRotation(desc.Rotation)
	{

	}

	cRigidBody::~cRigidBody(){
		delete this->mShape;
	}

	iShape* cRigidBody::GetShape(){
		return mShape;
	}

	void cRigidBody::GetTransform(glm::mat4& transformOut){
		transformOut = glm::mat4_cast(mRotation);
		transformOut[3][0] = mPosition.x;
		transformOut[3][1] = mPosition.y;
		transformOut[3][2] = mPosition.z;
		transformOut[3][3] = 1.f;
	}

	void cRigidBody::GetPosition(glm::vec3& positionOut){
		positionOut = mPosition;
	}

	void cRigidBody::GetRotation(glm::vec3& rotationOut){
		rotationOut = glm::eulerAngles(mRotation);
	}

	void cRigidBody::GetPreviousPosition(glm::vec3& previousPositionOut) {
		previousPositionOut = this->mPreviousPosition;
	}


	void cRigidBody::GetAngularVelocity(glm::vec3& angularVelOut) {
		angularVelOut = this->mAngularVelocity;
	}

	void cRigidBody::GetTorque(glm::vec3& torqueOut) {
		torqueOut = this->mTorque;
	}

	void cRigidBody::ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint) {
		mVelocity += impulse* (1 / mMass);
		//Might be wrong
		mAngularVelocity += glm::cross(relativePoint, mVelocity);
	}

	void cRigidBody::ApplyTorque(const glm::vec3& torque) {
		mTorque += torque;
	}

	void cRigidBody::GetVelocityAtPoint(glm::vec3& relativePoint, glm::vec3& velocityOut){
		velocityOut = mVelocity + glm::cross(mAngularVelocity, relativePoint - this->mPosition);
	}

	void cRigidBody::ApplyImpulse(const glm::vec3& impulse){
		mVelocity += impulse * (1 / this->mMass);
	}

	void cRigidBody::GetVelocity(glm::vec3& velOut) {
		velOut = this->mVelocity;
	}

	void cRigidBody::SetTransform(glm::mat4& transformIn) {
		this->mRotation = transformIn;
	}

	void cRigidBody::SetPosition(glm::vec3& positionIn) {
		this->mPosition = positionIn;
	}

	void cRigidBody::SetRotation(glm::vec3& rotationIn) {
		this->mRotation = rotationIn;
	}

	void cRigidBody::SetPreviousPosition(glm::vec3& positionIn) {
		this->mPreviousPosition = positionIn;
	}

	void cRigidBody::SetAngularVelocity(glm::vec3& angularVelIn) {
		this->mAngularVelocity = angularVelIn;
	}

	void cRigidBody::SetTorque(glm::vec3& torqueIn) {
		this->mTorque = torqueIn;
	}

	void cRigidBody::SetVelocity(glm::vec3& velIn) {
		this->mVelocity = velIn;
	}

	void cRigidBody::GetInverseMass(glm::vec3& massOut) {
		massOut = this->mInverseMass;
	}
	void cRigidBody::SetInverseMass(glm::vec3& massIn) {
		this->mInverseMass = massIn;
	}

	void cRigidBody::GetMass(float& massOut) {
		massOut = this->mMass;
	}

	void cRigidBody::SetMass(float& massIn) {
		this->mMass = massIn;
	}
}