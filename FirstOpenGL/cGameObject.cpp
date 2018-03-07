#include "cGameObject.h"
#include <iRigidBody.h>

cGameObject::cGameObject()
{
	this->scale = 1.0f;	// (not zero)
	this->position = glm::vec3(0.0f);
	this->previousPosition = glm::vec3(0.0f);
	this->orientation = glm::vec3(0.0f);
	this->orientation2 = glm::vec3(0.0f);

	this->vel = glm::vec3(0.0f);
	this->accel = glm::vec3(0.0f);	

	// If you aren't sure what the 4th value should be, 
	//	make it a 1.0f ("alpha" or transparency)
	this->diffuseColour = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	//Assume everything is simulated 
	this->bIsUpdatedInPhysics = true; //??? 
	this->bHasAABB = false;
	this->radius = 0.0f;	// Is this the best value??? Who knows?
	this->center = glm::vec3(0.0f);
	this->mass = 0.0f;

	this->typeOfObject = eTypeOfObject::UNKNOWN;	// Technically non-standard
	//this->typeOfObject = UNKNOWN;

	this->bIsWireFrame = false;
	this->bIsSkyBoxObject = false;
	return;
}

cGameObject::~cGameObject()
{
	return;
}

void cGameObject::synchronize() {
	nPhysics::iRigidBody* theBody = dynamic_cast<nPhysics::iRigidBody*>(this->theBody);
	theBody->GetPosition(this->position);
	theBody->GetVelocity(this->vel);
	theBody->GetRotation(this->orientation2);
}