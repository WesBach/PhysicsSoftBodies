#include "MoveToDistanceTime.h"
#include "cGameObject.h"
#include <glm\glm.hpp>

MoveToDistanceTime::MoveToDistanceTime(glm::vec3 pointToMoveTo, cGameObject* theObjectToMove,float timeItShouldTake) {
	//set the data
	this->theObjectToMove = theObjectToMove;
	this->posToMoveTo = pointToMoveTo;

	this->isDone = false;
	this->currentSectionToProcess = 0;
	this->timeToMove = timeItShouldTake;
	this->accumulatedTime = 0.0f;

	distanceBetweenSections = pointToMoveTo;

	if (pointToMoveTo.x > 0.0f)
		theObjectToMove->vel.x = 0.1f;
	else
		theObjectToMove->vel.x = -0.1f;

	if (pointToMoveTo.y > 0.0f)
		theObjectToMove->vel.y = 0.1f;
	else
		theObjectToMove->vel.y = -0.1f;

	if (pointToMoveTo.z > 0.0f)
		theObjectToMove->vel.z = 0.1f;
	else
		theObjectToMove->vel.z = -0.1f;

	this->initTime = 0.0;
}

MoveToDistanceTime::~MoveToDistanceTime() {

}

void MoveToDistanceTime::update(double deltaTime) {
	if (initTime == 0.0)
		initTime = deltaTime;

	accumulatedTime += deltaTime;

		//get the distance 
		if (currentSectionToProcess == 0)
		{
			if (accumulatedTime >= timeToMove / 3)
				currentSectionToProcess = 1;
			else {

				if (distanceBetweenSections.x > 0.0f)
					this->theObjectToMove->vel.x += glm::smoothstep(this->theObjectToMove->position.x, distanceBetweenSections.x, accumulatedTime + timeToMove);
				else if(distanceBetweenSections.x < 0.0f)
					this->theObjectToMove->vel.x -= glm::smoothstep(abs(this->theObjectToMove->position.x), abs(distanceBetweenSections.x), accumulatedTime + timeToMove);

				if (distanceBetweenSections.y > 0.0f)
					this->theObjectToMove->vel.y += glm::smoothstep(this->theObjectToMove->position.y, distanceBetweenSections.y, accumulatedTime + timeToMove);
				else if(distanceBetweenSections.y < 0.0f)
					this->theObjectToMove->vel.y -= glm::smoothstep(abs(this->theObjectToMove->position.y), abs(distanceBetweenSections.y), accumulatedTime + timeToMove);

				if (distanceBetweenSections.z >0.0f)
					this->theObjectToMove->vel.z += glm::smoothstep(this->theObjectToMove->position.z, distanceBetweenSections.z, accumulatedTime + timeToMove);
				else if (distanceBetweenSections.z < 0.0f)
					this->theObjectToMove->vel.z -= glm::smoothstep(abs(this->theObjectToMove->position.z), abs(distanceBetweenSections.z), accumulatedTime + timeToMove);

			}
		}

		if (currentSectionToProcess == 1)
		{
			if (accumulatedTime >= (timeToMove / 3) * 2)
				currentSectionToProcess = 2;
		}

		if (currentSectionToProcess == 2)
		{
			if (accumulatedTime >= timeToMove)
			{
				this->isDone = true;
				currentSectionToProcess = 3;
				this->theObjectToMove->vel = glm::vec3(0.0f);
			}
			else {

				if (distanceBetweenSections.x > 0.0f)
					this->theObjectToMove->vel.x -= glm::smoothstep(this->theObjectToMove->position.x, distanceBetweenSections.x, timeToMove - accumulatedTime);
				else if (distanceBetweenSections.x < 0.0f)
					this->theObjectToMove->vel.x += glm::smoothstep(this->theObjectToMove->position.x, distanceBetweenSections.x, timeToMove - accumulatedTime);

				if (distanceBetweenSections.y > 0.0f)
					this->theObjectToMove->vel.y -= glm::smoothstep(this->theObjectToMove->position.y, distanceBetweenSections.y, timeToMove - accumulatedTime);
				else if (distanceBetweenSections.y < 0.0f)
					this->theObjectToMove->vel.y += glm::smoothstep(this->theObjectToMove->position.y, distanceBetweenSections.y, timeToMove - accumulatedTime);

				if (distanceBetweenSections.z >0.0f)
					this->theObjectToMove->vel.z -= glm::smoothstep(this->theObjectToMove->position.z, distanceBetweenSections.z, timeToMove - accumulatedTime);
				else if (distanceBetweenSections.z < 0.0f)
					this->theObjectToMove->vel.z += glm::smoothstep(this->theObjectToMove->position.z, distanceBetweenSections.z, timeToMove - accumulatedTime);

			}
		}
}

bool MoveToDistanceTime::getDone() {
	return this->isDone;
}