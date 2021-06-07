#pragma once


#include "particle.h"
#include "fgen.h"

class Mover
{
public:
	Mover();
	Mover::Mover(cyclone::Vector3 m_Position, cyclone::Vector3 m_Velocity, cyclone::real m_Mass);
	~Mover();

	cyclone::ParticleGravity * m_gravity;
	cyclone::ParticleDrag * m_drag;
	cyclone::Particle * m_particle;

	int run;
	float size;
	void update(float duration);
	void draw(int shadow);
	void checkEdges();
	void addAccel(float x, float y, float z);
	void setAccel(float x, float y, float z);
	void changeWindDirection(int Code);
};

