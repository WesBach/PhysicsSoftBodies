#include "cEmitter.h"

cEmitter::cEmitter()
{
	// Maybe set some defaults?? 
	// (Or just be careful, right?)
	this->m_numParticles = 0;
	this->m_maxParticleCreatedPerStep = 0;
	this->m_minLife = 0.0f;
	this->m_maxLife = 0.0f;
	// 
	this->m_maxInitVel = glm::vec3(0.0f);
	this->m_minInitVel = glm::vec3(0.0f);
	this->m_minRangeFromEmitter = glm::vec3(0.0f);
	this->m_maxRangeFromEmitter = glm::vec3(0.0f);
	return;
}

cEmitter::~cEmitter()
{
	// Delete the particles
	return;
}

double getRandInRange( double min, double max )
{

	double value = min + static_cast <double> (rand()) / ( static_cast <double> (RAND_MAX/(max-min)));
	return value;
}

// Call this only to start of if you want to change the initial values
// (NOT to be called every frame or anything)
// (Also, call IN ADVANCE of "expensive" operations, like explosions, 
//  or other things that require a lot of particles)
void cEmitter::init(
	int numParticles, int maxParticleCreatedPerStep,
	glm::vec3 minInitVel, glm::vec3 maxInitVel,
	float minLife, float maxLife,
	glm::vec3 minRangeFromEmitter,
	glm::vec3 maxRangeFromEmitter, 
	glm::vec3 acceleration )
{
	// Do I already have this many particles in my vector? 
	int numParticlesToCreate = numParticles - this->m_vecParticles.size();
	// If we DON'T have enough, then create some
	for (int count = 0; count <= numParticlesToCreate; count++)
	{	// 
		this->m_vecParticles.push_back( new cParticle() );
	}
	// At this point, we have enough particles

	// Reset all the particles to the default position and life
	this->m_numParticles = numParticles;
	for (int index = 0; index != this->m_numParticles; index++)
	{	// reset ONLY the lifetime
		this->m_vecParticles[index]->lifetime = 0.0f;	// He's dead, Jim.
	}

	// Save all the init values we passed
	this->m_maxParticleCreatedPerStep = maxParticleCreatedPerStep;
	this->m_minInitVel = minInitVel;
	this->m_maxInitVel = maxInitVel;
	this->m_minLife = minLife;
	this->m_maxLife = maxLife;
	this->m_minRangeFromEmitter = minRangeFromEmitter;
	this->m_maxRangeFromEmitter = maxRangeFromEmitter;
	this->m_acceleration = acceleration;
	return;
}

int cEmitter::getLivingParticles(std::vector< cParticle* > &vecParticles)
{
	// Resize if too small	
	if (vecParticles.size() < this->m_numParticles)
	{
		// Resize and fill with NULL (or whatever)
		vecParticles.resize(this->m_numParticles, NULL);
	}
	// Copy all the alive particles
	int indexAliveParticles = 0;
	for (int index = 0; index != this->m_numParticles; index++)
	{
		// Is it alive?!
		if (this->m_vecParticles[index]->lifetime > 0.0f)
		{	// yes
			vecParticles[indexAliveParticles] = this->m_vecParticles[index];
			indexAliveParticles++;
		}
	}//for (int index

	// 1 more than the number of alive particles
	// HACK: o-rama!
	indexAliveParticles -= 50;
	if (indexAliveParticles < 0) indexAliveParticles = 0;
	//
	return indexAliveParticles;
}


void cEmitter::Update(float deltaTime)
{
	// 1. Calculate how many particles from m_maxParticleCreatedPerStep
	// 2. Find 'dead' particles in the vector
	// 3. IF you find them, 'create' new particles (copy init values)
	
	int particlesToStillMake = this->m_maxParticleCreatedPerStep;

	// Scan through the vector, looking for "dead" particles
	for (int index = 0; 
		 (index != this->m_numParticles) && (particlesToStillMake > 0);
		 index++)
	{
		// Is this particle 'dead'
		if (this->m_vecParticles[index]->lifetime <= 0.0f)
		{	// "create" a particle
			// TODO: Calculate some values between the ranges...
			// 
			// Assign values to particles
			
			// HACK: Just give them max values.
			this->m_vecParticles[index]->lifetime = 
				getRandInRange( this->m_minLife, this->m_maxLife );
			this->m_vecParticles[index]->pos = this->position;
			this->m_vecParticles[index]->vel.x = 
				getRandInRange( this->m_minRangeFromEmitter.x, this->m_maxRangeFromEmitter.x );
			this->m_vecParticles[index]->vel.y = 
				getRandInRange( this->m_minRangeFromEmitter.y, this->m_maxRangeFromEmitter.y );
			this->m_vecParticles[index]->vel.z = 
				getRandInRange( this->m_minRangeFromEmitter.z, this->m_maxRangeFromEmitter.z );

			// Update the count
			particlesToStillMake--;

			//// You could also exit the for loop here.
			//if (particlesToStillMake < 0)
			//{	// Exit the for loop
			//	break;
			//}
		}
	}//for (int index

	// 4. Perform integration step (aka Euler, or use physics, etc.)
	//    (ALSO: decrease the 'life' of the particle by deltaTime)
	for (int index = 0; index != this->m_numParticles; index++)
//	for (int index = 0; index != this->m_vecParticles.size()-1; index++)
	{
		// Forward explicit Euler...		
		// Velocity comes from accleration 
		this->m_vecParticles[index]->vel += (this->m_acceleration * deltaTime);

		// Position comes from velocity
		this->m_vecParticles[index]->pos +=
                         (this->m_vecParticles[index]->vel * deltaTime);

		// Update the lifetime
		this->m_vecParticles[index]->lifetime -= deltaTime;
	}

	return;
}

