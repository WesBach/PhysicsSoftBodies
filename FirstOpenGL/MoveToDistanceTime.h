#ifndef _MoveToDistanceTime_HG_
#define _MoveToDistanceTime_HG_
#include "iCommand.h"
#include <glm\vec3.hpp>

class cGameObject;

//Distance over time
class MoveToDistanceTime : public iCommand
{
public:
	MoveToDistanceTime(glm::vec3 pointToMoveTo, cGameObject* theObjectToMove, float timeItShouldTake);
	~MoveToDistanceTime();

	virtual void update(double deltaTime);
	virtual bool getDone();

	cGameObject* theObjectToMove;
	glm::vec3 distanceBetweenSections;
	glm::vec3 posToMoveTo;

	float timeToMove;
	float accumulatedTime;

private:
	bool isDone;
	int currentSectionToProcess;
	double initTime;
};

#endif // !_MoveTo_HG_

