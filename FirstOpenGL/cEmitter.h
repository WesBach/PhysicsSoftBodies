#ifndef _cEmitter_HG_
#define _cEmitter_HG_

#include <vector>
#include "cParticle.h"

class cEmitter
{
public:
	cEmitter();
	~cEmitter();
	void init( int numParticles, int maxParticleCreatedPerStep,
		      glm::vec3 minInitVel, glm::vec3 maxInitVel, 
	          float minLife, float maxLife,
	          glm::vec3 minRangeFromEmitter,
	          glm::vec3 maxRangeFromEmitter, 
		      glm::vec3 acceleration );		// SORRY!
	glm::vec3 position;
	void Update(float deltaTime);

	// returns the number of living particles
	int getLivingParticles(std::vector< cParticle* > &vecParticles);
	
private:
	int m_numParticles;
	std::vector< cParticle* > m_vecParticles;

	// 
	int m_maxParticleCreatedPerStep;
	glm::vec3 m_minInitVel;
	glm::vec3 m_maxInitVel;
	float m_minLife;
	float m_maxLife;
	glm::vec3 m_minRangeFromEmitter;
	glm::vec3 m_maxRangeFromEmitter;
	glm::vec3 m_acceleration;

};

#endif 
