#include "cIntegrator.h"
#include "cRigidBody.h"
#include "cSoftBody.h"

namespace nPhysics {
	cIntegrator::cIntegrator(IntegrationType type) {
		this->mIntegrationype = type;
	}

	cIntegrator::~cIntegrator() {

	}

	IntegrationType cIntegrator::getType() {
		return this->mIntegrationype;
	}

	void cIntegrator::integrate(std::vector<iCollisionBody*>& theBodies, double time, float dt) {
		if (this->mIntegrationype == IntegrationType::RK4){

			glm::vec3 pos;
			glm::vec3 vel;
			glm::vec3 accel(0.0f, -9.8f, 0.0f);
			glm::vec3 rot;
			glm::vec3 angularVelocity;
			cRigidBody* tempRigidBody = NULL;
			cSoftBody*	tempSoftBody = NULL;

			for (int i = 0; i < theBodies.size(); i++)
			{
				tempRigidBody = NULL;
				tempSoftBody = NULL;
				//check to see what type of body it is
				eBodyType currBodyType = theBodies[i]->GetBodyType();
				if (currBodyType == eBodyType::RIGID_BODY)
				{
					tempRigidBody = dynamic_cast<cRigidBody*>(theBodies[i]);
				}
				else
				{
					tempSoftBody = dynamic_cast<cSoftBody*>(theBodies[i]);
				}

				if (tempRigidBody != NULL){
					if (tempRigidBody->GetShape()->GetShapeType() == SHAPE_TYPE_PLANE)
						continue;

					//set the old position
					tempRigidBody->GetPosition(pos);
					tempRigidBody->GetVelocity(vel);
					tempRigidBody->GetRotation(rot);
					tempRigidBody->GetAngularVelocity(angularVelocity);

					//reduce angular velocity by 10%
					//angularVelocity *= 0.9f;
					tempRigidBody->SetAngularVelocity(angularVelocity);

					tempRigidBody->SetPreviousPosition(pos);
					integrationStepRK4(pos, vel, accel, rot, angularVelocity, dt);

					//set the old position / velocity
					tempRigidBody->SetPosition(pos);
					tempRigidBody->SetVelocity(vel);
					//tempRigidBody->SetRotation(rot);
				}
				else if(tempSoftBody != NULL){
					//update the nodes on the softBody
					for (int i = 0; i < tempSoftBody->Nodes.size(); i++)
					{
						//check to make sure the nodes arent static before updating 
						if (tempSoftBody->Nodes[i]->IsStatic == false) {
							//update the node with rk4
							tempSoftBody->Nodes[i]->PreviousPosition = tempSoftBody->Nodes[i]->Position;
							integrationStepRK4(tempSoftBody->Nodes[i]->Position, 
												tempSoftBody->Nodes[i]->Velocity, 
												accel, 
												rot, angularVelocity, dt);
						}
					}
				}

				
			}
		}
		else if (this->mIntegrationype == IntegrationType::VERLET)
		{
			integrationStepVerlet(theBodies, dt);
		}
	}

	Derivative& cIntegrator::evaluate(const RK4State& initial,
		float dt,
		const Derivative & d){

		RK4State s;
		s.position = initial.position + d.deltaPosition * dt;
		s.velocity = initial.velocity + d.deltaPosition * dt;

		Derivative output;
		output.deltaPosition = s.velocity;
		output.deltaVelocity = initial.acceleration;
		return output;
	}

	//glm::vec3 cIntegrator::acceleration(cRigidBody state, double t){

	//	glm::vec3 velocity;
	//	glm::vec3 position;

	//	state.GetPosition(position);
	//	state.GetVelocity(velocity);

	//	const float k = 15.0f;
	//	const float b = 0.1f;
	//	return -k * position - b * velocity;
	//}

	void cIntegrator::integrationStepRK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& accel,glm::vec3& rot, glm::vec3& angularVelocity, float dt) {
		Derivative a, b, c, d;

		RK4State state(pos, vel, accel);
		a = evaluate(state, 0.0f, Derivative());
		b = evaluate(state, dt*0.5f, a);
		c = evaluate(state, dt*0.5f, b);
		d = evaluate(state, dt, c);

		/*glm::vec3 dxdt = (a.deltaPosition + ((b.deltaPosition + c.deltaPosition)* 2.0f) + d.deltaPosition) * (1.f / 6.f);
		glm::vec3 dvdt = (a.deltaVelocity + ((b.deltaVelocity + c.deltaVelocity)* 2.0f) + d.deltaVelocity) * (1.f / 6.f);*/
		glm::vec3 dxdt = 1.0f/6.0f * (a.deltaPosition + 2.0f * (b.deltaPosition+c.deltaPosition)+d.deltaPosition);
		glm::vec3 dvdt = 1.0f/6.0f * (a.deltaVelocity + 2.0f * (b.deltaVelocity + c.deltaVelocity) + d.deltaVelocity);

		//rot += (angularVelocity * dt);
		pos += dxdt * dt;
		vel += dvdt * dt;
	}

	void cIntegrator::integrationStepEuler(std::vector<iCollisionBody*>& theBodies, float dt) {
		return;
	}

	void cIntegrator::integrationStepVerlet(std::vector<iCollisionBody*>& theBodies, float dt) {
		cRigidBody* tempRigidBody = NULL;
		cSoftBody* tempSoftBody = NULL;
		eBodyType bodyType;

		glm::vec3 Accel = glm::vec3(0, -9.81, 0);
		glm::vec3 OldVel;
		glm::vec3 currVelocity;
		glm::vec3 position;
		glm::vec3 rot;
		glm::vec3 angularVelocity;

		for (int i = 0; i < theBodies.size(); i++){

			//check the body for type
			bodyType = theBodies[i]->GetBodyType();
			if (bodyType == eBodyType::RIGID_BODY){
				tempRigidBody = dynamic_cast<cRigidBody*>(theBodies[i]);
			}
			else {
				tempSoftBody = dynamic_cast<cSoftBody*>(theBodies[i]);
			}

			//if the collision body is a rigid body update
			if (tempRigidBody != NULL) {

				if (tempRigidBody->GetShape()->GetShapeType() == SHAPE_TYPE_PLANE)
					continue;

				tempRigidBody->GetVelocity(OldVel);
				tempRigidBody->GetPosition(position);
				tempRigidBody->GetVelocity(currVelocity);
				tempRigidBody->GetAngularVelocity(angularVelocity);
				tempRigidBody->GetRotation(rot);

				//reduce angular velocity by 1%
				angularVelocity *= 0.9f;
				tempRigidBody->SetAngularVelocity(angularVelocity);

				//set previous position
				tempRigidBody->SetPreviousPosition(position);

				//calculate new velocity and position
				currVelocity = currVelocity + Accel * dt;
				position = position + (OldVel + currVelocity) * 0.5f * dt;

				//set the new velocity and position
				//rot += (angularVelocity * dt);
				tempRigidBody->SetVelocity(currVelocity);
				tempRigidBody->SetPosition(position);
			}
			else {
				//collision body is a SoftBody
				//update all the nodes
				for (int i = 0; i < tempSoftBody->Nodes.size(); i++)
				{
					//make sure the node isnt static
					if (tempSoftBody->Nodes[i]->IsStatic == false) {
						//calculate new velocity and position
						tempSoftBody->Nodes[i]->Velocity = tempSoftBody->Nodes[i]->Velocity + Accel * dt;
						tempSoftBody->Nodes[i]->Position = tempSoftBody->Nodes[i]->Position + (OldVel + currVelocity) * 0.5f * dt;
					}
				}

				tempSoftBody->UpdateNodes(dt);
			}

		}
	}
}

