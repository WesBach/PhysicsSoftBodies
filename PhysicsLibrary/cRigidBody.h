#pragma once
#include "iRigidBody.h"
#include "sRigidBodyDesc.h"
#include <glm\vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

namespace nPhysics
{
	class cPhysicsWorld;
	class cRigidBody : public iRigidBody{
	public:
		cRigidBody(const sRigidBodyDesc& desc, iShape* shape);
		virtual ~cRigidBody();

		virtual iShape* GetShape();

		virtual void GetTransform(glm::mat4& transformOut);
		virtual void GetPosition(glm::vec3& positionOut);
		virtual void GetRotation(glm::vec3& rotationOut);	
		virtual void GetPreviousPosition(glm::vec3& positionOu);
		virtual void GetAngularVelocity(glm::vec3& andgularVelOut);
		virtual void GetTorque(glm::vec3& torqueOut);
		virtual void GetVelocity(glm::vec3& velOut);
		virtual void GetInverseMass(glm::vec3& velOut);
		virtual void GetMass(float& massOut);

		virtual void SetTransform(glm::mat4& transformIn);
		virtual void SetPosition(glm::vec3& positionIn);
		virtual void SetRotation(glm::vec3& rotationIn);
		virtual void SetPreviousPosition(glm::vec3& positionIn);
		virtual void SetAngularVelocity(glm::vec3& andgularVelIn);
		virtual void SetTorque(glm::vec3& torqueIn);
		virtual void SetVelocity(glm::vec3& velIn);
		virtual void SetInverseMass(glm::vec3& velIn);
		virtual void SetMass(float& massIn);



		void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint);
		void ApplyTorque(const glm::vec3& torque);
		void GetVelocityAtPoint(glm::vec3& relativePoint, glm::vec3& velocityOut);
		void ApplyImpulse(const glm::vec3& impulse);

	private:
		friend class cPhysicsWorld;
		iShape* mShape;
		glm::vec3 mAngularVelocity;
		glm::vec3 mInverseMass;
		glm::vec3 mTorque;

		glm::vec3 mPosition;
		glm::vec3 mVelocity;
		glm::quat mRotation;
		glm::vec3 mPreviousPosition;
		float mMass;
	};
}