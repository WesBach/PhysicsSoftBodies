#ifndef _cIntegrator_HG_
#define _cIntegrator_HG_
#include <vector>
#include "iCollisionBody.h"
#include <glm\vec3.hpp>
#include "enums.h"

class cRigidBody;

struct Derivative{
	glm::vec3 deltaPosition;
	glm::vec3 deltaVelocity;
};

struct RK4State {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	RK4State() {
		position = glm::vec3(0.0f);
		velocity = glm::vec3(0.0f);
		acceleration = glm::vec3(0.0f);
	}

	RK4State(const glm::vec3& pos, const glm::vec3& vel, const glm::vec3& accel):position(pos),velocity(vel),acceleration(accel){}
};

namespace nPhysics {

	class cIntegrator {
	public:

		cIntegrator(IntegrationType type);
		~cIntegrator();

		IntegrationType getType();

		void cIntegrator::integrationStepRK4(glm::vec3& pos, glm::vec3& vel, glm::vec3& accel,glm::vec3& rot, glm::vec3& angularVelocity, float dt);
		void integrationStepEuler(std::vector<iCollisionBody*>& theBodies, float dt);
		void integrationStepVerlet(std::vector<iCollisionBody*>& theBodies, float dt);

		void integrate(std::vector<iCollisionBody*>& theBodies,double time, float dt);

		Derivative& evaluate(const RK4State& initial,float dt,const Derivative & d);
		//glm::vec3 acceleration(cRigidBody state, double t);

	private:
		IntegrationType mIntegrationype;
	};
}
#endif // !_cIntegrator_HG_
