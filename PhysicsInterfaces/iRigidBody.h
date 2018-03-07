#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "iShape.h"
#include "iCollisionBody.h"

namespace nPhysics
{
	class iRigidBody : public iCollisionBody{
	public:
		virtual ~iRigidBody() {}

		virtual iShape* GetShape() = 0;

		virtual void GetTransform(glm::mat4& transformOut) = 0;
		virtual void GetPosition(glm::vec3& positionOut) = 0;
		virtual void GetRotation(glm::vec3& rotationOut) = 0;
		virtual void GetPreviousPosition(glm::vec3& positionOu) = 0;
		virtual void GetAngularVelocity(glm::vec3& andgularVelOut) = 0;
		virtual void GetTorque(glm::vec3& torqueOut) = 0;
		virtual void GetVelocity(glm::vec3& velOut) = 0;
		virtual void GetInverseMass(glm::vec3& velOut) = 0;
		virtual void GetMass(float& massOut) = 0;

		virtual void SetTransform(glm::mat4& transformIn) = 0;
		virtual void SetPosition(glm::vec3& positionIn) = 0;
		virtual void SetRotation(glm::vec3& rotationIn) = 0;
		virtual void SetPreviousPosition(glm::vec3& positionIn) = 0;
		virtual void SetAngularVelocity(glm::vec3& andgularVelIn) = 0;
		virtual void SetTorque(glm::vec3& torqueIn) = 0;
		virtual void SetVelocity(glm::vec3& velIn) = 0;
		virtual void SetInverseMass(glm::vec3& velIn) = 0;
		virtual void SetMass(float& massIn) = 0;

		virtual void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint) = 0;
		virtual void ApplyTorque(const glm::vec3& torque) = 0;
		virtual void GetVelocityAtPoint(glm::vec3& relativePoint, glm::vec3& velocityOut) = 0;
		virtual void ApplyImpulse(const glm::vec3& impulse) = 0;

	protected:
		iRigidBody() : iCollisionBody(RIGID_BODY) {}
		iRigidBody(const iRigidBody& other) : iCollisionBody(RIGID_BODY) {}
		iRigidBody& operator=(const iRigidBody& other) { return *this; }
	};
}