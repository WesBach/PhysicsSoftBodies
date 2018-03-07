#ifndef _collisionResponses_HG_
#define _collisionResponses_HG_
#include "cRigidBody.h"
#include "cSoftBody.h"

namespace nPhysics {
	void SphereSphereCollisionResponse(iRigidBody* a, iRigidBody* b, float dt){

		glm::vec3 posA, posB, prevPosA, prevPosB, velA, velB;
		a->GetPreviousPosition(prevPosA);
		b->GetPreviousPosition(prevPosB);
		a->GetPosition(posA);
		b->GetPosition(posB);

		glm::vec3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

		float m1, m2, x1, x2;
		glm::vec3 v1temp, vel1, vel2, v1x, v2x, v1y, v2y, x(posA - posB), v1, v2;

		glm::normalize(x);
		a->GetVelocity(v1);
		x1 = dot(x, v1);
		v1x = x * x1;
		v1y = v1 - v1x;
		a->GetMass(m1);

		x = x*-1.0f;
		b->GetVelocity(v2);
		x2 = dot(x, v2);
		v2x = x * x2;
		v2y = v2 - v2x;
		b->GetMass(m2);

		//ua is the initial velocity of the first object before impact
		//ub is the initial velocity of the second object before impact
		//ma is the mass of the first object
		//mb is the mass of the second object
		//v = { m_a u_a + m_b  u_b }/{m_a + m_b}

		//set the position of the spheres to their previous non contact positions to unstick them.
		a->GetVelocity(vel1);
		b->GetVelocity(vel2);

		a->GetVelocity(v1);
		b->GetVelocity(v2);
		/*posA = prevPosA + (vel1 * dt);
		posB = prevPosB + (vel2 * dt);

		float coef = 0.2f;
		float mt = m1 + m2;

		velA = (coef * m2 *(vel1 - vel2) + m1 * vel1 + m2*vel2) / mt;
		velB = (coef * m1 *(vel2 - vel1) + m1 * vel1 + m2*vel2) / mt;
		*/
		velA = (glm::vec3(v1x*(m1 - m2) / (m1 + m2) + v2x*(2 * m2) / (m1 + m2) + v1y) / 4.0f) * (dt / 0.5f);
		velB = (glm::vec3(v1x*(2 * m1) / (m1 + m2) + v2x*(m2 - m1) / (m1 + m2) + v2y) / 4.0f) * (dt / 0.5f);

		a->SetPosition(prevPosA);
		a->ApplyImpulse(velA);

		b->SetPosition(prevPosB);
		b->ApplyImpulse(velB);
	}

	void SphereNodeCollisionResponse(iRigidBody* a, cSoftBody::cNode* b, float dt) {

		glm::vec3 posA, posB, prevPosA, prevPosB, velA, velB;
		a->GetPreviousPosition(prevPosA);
		//b->GetPreviousPosition(prevPosB);
		a->GetPosition(posA);
		//b->GetPosition(posB);
		posB = b->Position;
		prevPosB = b->PreviousPosition;

		glm::vec3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

		float m1, m2, x1, x2;
		glm::vec3 v1temp, vel1, vel2, v1x, v2x, v1y, v2y, x(posA - posB), v1, v2;

		glm::normalize(x);
		a->GetVelocity(v1);
		x1 = dot(x, v1);
		v1x = x * x1;
		v1y = v1 - v1x;
		a->GetMass(m1);

		x = x*-1.0f;
		//b->GetVelocity(v2);
		v2 = b->Velocity;
		x2 = dot(x, v2);
		v2x = x * x2;
		v2y = v2 - v2x;
		//b->GetMass(m2);
		m2 = b->Mass;
		//ua is the initial velocity of the first object before impact
		//ub is the initial velocity of the second object before impact
		//ma is the mass of the first object
		//mb is the mass of the second object
		//v = { m_a u_a + m_b  u_b }/{m_a + m_b}

		//set the position of the spheres to their previous non contact positions to unstick them.
		a->GetVelocity(vel1);
		//b->GetVelocity(vel2);
		vel2 = b->Velocity;

		a->GetVelocity(v1);
		//b->GetVelocity(v2);
		v2 = b->Velocity;
		/*posA = prevPosA + (vel1 * dt);
		posB = prevPosB + (vel2 * dt);

		float coef = 0.2f;
		float mt = m1 + m2;

		velA = (coef * m2 *(vel1 - vel2) + m1 * vel1 + m2*vel2) / mt;
		velB = (coef * m1 *(vel2 - vel1) + m1 * vel1 + m2*vel2) / mt;
		*/
		//velA = (glm::vec3(v1x*(m1 - m2) / (m1 + m2) + v2x*(2 * m2) / (m1 + m2) + v1y) / 4.0f) * (dt / 0.5f);
		velB = (glm::vec3(v1x*(2 * m1) / (m1 + m2) + v2x*(m2 - m1) / (m1 + m2) + v2y) / 4.0f) * (dt / 0.5f);

		//a->SetPosition(prevPosA);
		//a->ApplyImpulse(velA);

		b->Position = prevPosB;
		//+= impulse * (1 / this->mMass);
		b->Velocity += velB *(1/b->Mass);
	}
}
#endif // 