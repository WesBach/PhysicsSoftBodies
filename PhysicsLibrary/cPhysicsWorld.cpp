#include "cPhysicsWorld.h"
#include "collision.h"
#include "collisionResponse.h"
#include "cRigidBody.h"
#include "cSoftBody.h"
#include "shapes.h"
#include <algorithm>

namespace nPhysics
{

	cPhysicsWorld::~cPhysicsWorld() {

	}

	void cPhysicsWorld::TimeStep(double time, float deltaTime) {

		//set the delta time each time the TimeStep is called
		this->mDeltaTime = deltaTime;

		//std::vector<iCollisionBody*>::iterator itCollisionBody = mCollisionBody.begin();
		//while (itCollisionBody != mCollisionBody.end())
		//{
		//	cRigidBody* rb = dynamic_cast<cRigidBody*>(*itCollisionBody);
		//	rb->mPosition += rb->mVelocity * deltaTime;

		//	itCollisionBody++;
		//}

		//do the integration
		this->mIntegrator->integrate(this->mCollisionBody, this->mTime, this->mDeltaTime);

		//check for collisions 
		for (int i = 0; i < this->mCollisionBody.size() - 1; i++) {
			for (unsigned int index = i + 1; index < this->mCollisionBody.size(); index++)
			{
				this->Collide(mCollisionBody[i], mCollisionBody[index]);
			}
		}

		//re-integrate
		this->mIntegrator->integrate(this->mCollisionBody, this->mTime, this->mDeltaTime);

		//update the soft body nodes
		for (int i = 0; i < this->mCollisionBody.size(); i++) {
			if (this->mCollisionBody[i]->GetBodyType() == eBodyType::SOFT_BODY) {
				cSoftBody* theSoftBody = dynamic_cast<cSoftBody*>(this->mCollisionBody[i]);
				theSoftBody->UpdateNodes(deltaTime);
			}
		}
	}

	void cPhysicsWorld::AddCollisionBody(iCollisionBody* collisionBody) {
		eBodyType type = collisionBody->GetBodyType();
		cRigidBody* rb = NULL;
		cSoftBody* sb = NULL;

		if (type == eBodyType::RIGID_BODY) {
			rb = dynamic_cast<cRigidBody*>(collisionBody);
			std::vector<iCollisionBody*>::iterator itCollisionBody;
			itCollisionBody = std::find(mCollisionBody.begin(), mCollisionBody.end(), rb);
			if (itCollisionBody == mCollisionBody.end()) {
				mCollisionBody.push_back(rb);
			}
		}
		else {
			sb = dynamic_cast<cSoftBody*>(collisionBody);
			std::vector<iCollisionBody*>::iterator itCollisionBody;
			itCollisionBody = std::find(mCollisionBody.begin(), mCollisionBody.end(), sb);
			if (itCollisionBody == mCollisionBody.end())
			{
				mCollisionBody.push_back(sb);
			}
		}

	}

	void cPhysicsWorld::RemoveCollisionBody(iCollisionBody* rigidBody) {

		std::vector<iCollisionBody*>::iterator itCollisionBody;

		itCollisionBody = std::remove(mCollisionBody.begin(), mCollisionBody.end(), rigidBody);
		if (itCollisionBody != mCollisionBody.end())
		{
			mCollisionBody.resize(mCollisionBody.size() - 1);
		}
	}

	void cPhysicsWorld::setIntegrationType(IntegrationType theType) {
		this->mIntegrator = new cIntegrator(theType);
	}

	void cPhysicsWorld::Collide(iCollisionBody* collisionBodyA, iCollisionBody* collisionBodyB) {
		//necessary variables for collision
		cRigidBody* rigidBodyA = NULL;
		cRigidBody* rigidBodyB = NULL;
		cSoftBody* softBody = NULL;
		eShapeType shapeA;
		eShapeType shapeB;

		//get the collision body types 
		eBodyType bodyTypeA = collisionBodyA->GetBodyType();
		if (bodyTypeA == eBodyType::RIGID_BODY) {
			rigidBodyA = dynamic_cast<cRigidBody*>(collisionBodyA);
			shapeA = rigidBodyA->GetShape()->GetShapeType();
		}
		else {
			softBody = dynamic_cast<cSoftBody*>(collisionBodyA);
		}

		//get the collisionBodyB body type
		eBodyType bodyTypeB = collisionBodyB->GetBodyType();
		if (bodyTypeB == eBodyType::RIGID_BODY) {
			rigidBodyB = dynamic_cast<cRigidBody*>(collisionBodyB);
			shapeB = rigidBodyB->GetShape()->GetShapeType();
		}
		else {
			softBody = dynamic_cast<cSoftBody*>(collisionBodyB);
		}


		//Collision response
		if (shapeA == SHAPE_TYPE_SPHERE){
			//check to see if the soft body is populated
			if (softBody != NULL) {

				glm::vec3 rigidBodyPos;
				float rigidBodyRadius;
				glm::vec3 rigidBodyVelocity;

				rigidBodyA->GetPosition(rigidBodyPos);	
				rigidBodyA->GetShape()->GetSphereRadius(rigidBodyRadius);
				rigidBodyA->GetVelocity(rigidBodyVelocity);
				float time = 0.f;
				//Do the rigid body soft body interaction
				//check to see if the node is within the radius of the sphere and if it is move it to outside of the sphere
				//use a direction vector from sphere to point to get the final direction
				for (int i = 0; i < softBody->Nodes.size(); i++) {

					if (softBody->Nodes[i]->IsStatic != true) {

						//check to see if colliding
						int collided = glm::intersect_moving_sphere_sphere(rigidBodyPos, rigidBodyRadius, rigidBodyVelocity, softBody->Nodes[i]->Position, 0.25f, softBody->Nodes[i]->Velocity, time);

						if (collided > 0) { // already colliding
							//apply velocity to the node based on the velocity of the sphere 
							//softBody->Nodes[i]->Velocity += rigidBodyVelocity * mDeltaTime;
							//SphereNodeCollisionResponse(rigidBodyA, softBody->Nodes[i], time);
							glm::vec3 direction = rigidBodyPos - softBody->Nodes[i]->Position;
							direction = glm::normalize(direction);
							//take the distance to from the node to the center of the sphere
							float distanceFromNodeToSphere = glm::distance(rigidBodyPos, softBody->Nodes[i]->Position);
							//distance to travel to get to the edge of the sphere
							distanceFromNodeToSphere = rigidBodyRadius - distanceFromNodeToSphere;
							//add the node radius
							distanceFromNodeToSphere += 0.5f;
							//apply that to the position of the node in the correct direction
							softBody->Nodes[i]->Position += (direction * distanceFromNodeToSphere) * mDeltaTime;
						}
						else if (collided < 0) {
							//TODO::fix this for the already colliding
							//direction from the node to the sphere
							glm::vec3 direction = rigidBodyPos - softBody->Nodes[i]->Position;
							direction = glm::normalize(direction);
							//take the distance to from the node to the center of the sphere
							float distanceFromNodeToSphere = glm::distance(rigidBodyPos, softBody->Nodes[i]->Position);
							//distance to travel to get to the edge of the sphere
							distanceFromNodeToSphere = rigidBodyRadius - distanceFromNodeToSphere;
							//add the node radius
							distanceFromNodeToSphere += 0.5f;
							//apply that to the position of the node in the correct direction
							softBody->Nodes[i]->Position += (direction * distanceFromNodeToSphere) * mDeltaTime;
						}
					}

				}

				//whatever its looking at minus its position(direction)


			}
			else if (shapeB == SHAPE_TYPE_SPHERE) {
				//do sphere sphere collision
				glm::vec3 centerA;
				glm::vec3 centerB;
				float radiusA;
				float radiusB;
				float tempTime = this->mDeltaTime;
				glm::vec3 velA;
				glm::vec3 velB;

				rigidBodyA->GetPosition(centerA);
				rigidBodyB->GetPosition(centerB);

				rigidBodyA->GetShape()->GetSphereRadius(radiusA);
				rigidBodyB->GetShape()->GetSphereRadius(radiusB);

				rigidBodyA->GetVelocity(velA);
				rigidBodyB->GetVelocity(velB);

				//intersect_moving_sphere_plane(const vec3& c, float r, const vec3& v, const vec3& n, float d, float& t, vec3& q)
				int collided = glm::intersect_moving_sphere_sphere(centerA, radiusA, velA, centerB, radiusB, velB, tempTime);

				if (collided > 0)
				{
					SphereSphereCollisionResponse(rigidBodyA, rigidBodyB, tempTime);
				}
				if (collided == -1)
				{
					glm::vec3 prevPos;

					rigidBodyA->GetPreviousPosition(prevPos);
					rigidBodyA->SetPosition(prevPos);

					glm::vec3 prevPosB;

					rigidBodyB->GetPreviousPosition(prevPosB);
					rigidBodyB->SetPosition(prevPosB);

					//float dist = glm::distance(prevPos, prevPosB);
					////get the direction vector
					//glm::vec3 direction = prevPos - prevPosB;
					//direction = glm::normalize(direction);
					//
					////get the dist to travel;
					//dist = dist - radiusA;
					//
					//if (direction.x > 0.0f) {
					//	direction.x = dist;
					//}
					//if (direction.y > 0.0f) {
					//	direction.y = dist;
					//}
					//if (direction.z > 0.0f) {
					//	direction.z = dist;
					//}
					//
					//rigidBodyA->SetPosition(prevPos + direction);
					//rigidBodyB->SetPosition(prevPosB - direction);

				}
			}
			else if (shapeB == SHAPE_TYPE_PLANE) {
				//do sphere plane collision
				glm::vec3 centerA;
				float radiusA;
				glm::vec3 velA;

				rigidBodyA->GetPosition(centerA);
				rigidBodyA->GetVelocity(velA);
				rigidBodyA->GetShape()->GetSphereRadius(radiusA);

				glm::vec3 planeNormal;
				rigidBodyB->GetShape()->GetPlaneNormal(planeNormal);
				float planeDot;
				rigidBodyB->GetShape()->GetPlaneConst(planeDot);

				//	inline vec3 closest_point_on_plane(const vec3& point, const vec3& n, float d)
				glm::vec3 closetsPoint = glm::closest_point_on_plane(centerA, planeNormal, planeDot);

				//inline int intersect_moving_sphere_plane(const vec3& c, float r, const vec3& v, const vec3& n, float d, float& t, vec3& q)
				glm::vec3 collisionPoint;
				float collision;
				int collided = glm::intersect_moving_sphere_plane(centerA, radiusA, velA, planeNormal, planeDot, collision, collisionPoint);

				glm::vec3 finalVelocity;

				if (collided > 0)
				{
					//do the reflection
					glm::vec3 prevPos;
					glm::vec3 normal;
					glm::vec3 currPos;
					glm::vec3 rotation;

					rigidBodyA->GetPosition(currPos);
					rigidBodyA->GetPreviousPosition(prevPos);
					//set the new position to the collision point

					//get the direction vector(facing towards the sphere)

					rigidBodyA->SetPosition(prevPos);
					rigidBodyA->GetRotation(rotation);

					//rotate the sphere in the opposite direction of the impact
					//rotation += -glm::reflect(velA, planeNormal);

					finalVelocity = glm::reflect(velA, planeNormal);
					rigidBodyA->ApplyImpulseAtPoint(finalVelocity, collisionPoint);

					finalVelocity *= 0.5f;
					rigidBodyA->SetVelocity(finalVelocity);
				}
				else if (collided < 0)
				{
					glm::vec3 prevPos;
					glm::vec3 zero(0.0f);
					finalVelocity = glm::reflect(velA, planeNormal);
					finalVelocity *= 0.9f;
					rigidBodyA->SetVelocity(finalVelocity);
					rigidBodyA->GetPreviousPosition(prevPos);
					rigidBodyA->SetPosition(prevPos);
					rigidBodyA->SetAngularVelocity(zero);

					glm::vec3 directionVec = prevPos - collisionPoint;
					//add the radius to the correct direction
					directionVec = glm::normalize(directionVec);
					if (directionVec.x > 0.0f) {
						directionVec.x = radiusA;
					}
					if (directionVec.y > 0.0f) {
						directionVec.y = radiusA;
					}
					if (directionVec.z > 0.0f) {
						directionVec.z = radiusA;
					}
					//set the new position of the ball
					rigidBodyA->SetPosition(collisionPoint + directionVec);
				}
			}

		}
		else if (shapeA == SHAPE_TYPE_PLANE) {
			if (softBody != NULL) {
				//Do the soft body plane interaction

				//go through the nodes and check to see if they are colliding with the plane
				for (int i = 0; i < softBody->Nodes.size(); i++) {
					glm::vec3 planeNormal;
					rigidBodyA->GetShape()->GetPlaneNormal(planeNormal);
					float planeDot;
					rigidBodyA->GetShape()->GetPlaneConst(planeDot);
					float collision;
					glm::vec3 collisionPoint(0.0f);
					float radius = 0.25f;
					//check to see if the node is intersecting the plane
					int outcome = glm::intersect_moving_sphere_plane(softBody->Nodes[i]->Position,
						radius,
						softBody->Nodes[i]->Velocity,
						planeNormal,
						planeDot,
						collision,
						collisionPoint);

					if (outcome > 0) {
						//TODO::
						//the reflection??
						softBody->Nodes[i]->Velocity.y = 0.0f;
						softBody->Nodes[i]->Position.y = collisionPoint.y + radius;
					}
					else if (outcome == -1) {
						//move the node back to outside of the plane
						//glm::vec3 directionVec = prevPos - collisionPoint;
						////add the radius to the correct direction
						//directionVec = glm::normalize(directionVec);
						//if (directionVec.x > 0.0f) {
						//	directionVec.x = radiusA;
						//}
						//if (directionVec.y > 0.0f) {
						//	directionVec.y = radiusA;
						//}
						//if (directionVec.z > 0.0f) {
						//	directionVec.z = radiusA;
						//}
					}
				}
			}
		}
	}
}