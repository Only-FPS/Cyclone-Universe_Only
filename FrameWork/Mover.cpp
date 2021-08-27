#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "Mover.h"
#include "gl/glut.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/matrix_transform_2d.hpp"

const float m_pi = glm::pi<float>();

Mover::Mover()
{
	m_particle = new cyclone::Particle();
	std::random_device rd;
	std::uniform_real_distribution<double> dist(-500.0, 500.0);

	//double x, y, z;
	//x = dist(rd);
	//y = dist(rd);
	//z = dist(rd);

	//cyclone::Vector3 InitVec(x, y, z);

	//m_particle->setPosition(InitVec);
	//InitVec.normalise();
	//cyclone::Vector3 unitVec = InitVec;
	//m_particle->setVelocity(unitVec);

	m_particle->setPosition(dist(rd), dist(rd), dist(rd));
	m_particle->setVelocity(0, 0, 0);
	m_particle->setMass(100.0f);
	m_particle->setDamping(1.0f);
	size = glm::pow((m_particle->getMass() * 3.0 / 4.0 / m_pi), 0.3333);
	//m_particle->setPosition(2, 10, 2);
	//m_particle->setDamping(0.99f);
	//m_particle->setAcceleration(cyclone::Vector3::);
			
	
	//m_drag = new cyclone::ParticleDrag(0.1, 0.1);
	
}

Mover::Mover(cyclone::Vector3 m_Position, cyclone::Vector3 m_Velocity, cyclone::real m_Mass)
{
	m_particle = new cyclone::Particle();
	m_particle->setPosition(m_Position);
	m_particle->setVelocity(m_Velocity);
	m_particle->setMass(m_Mass);
	m_particle->setDamping(1.0f);
	size = m_Mass / 100.0;
}


Mover::~Mover()
{
}

float fx(int timer) {
	return (0.06 * (sin(32) * timer / 12) + pow(m_pi, cos(timer)) - 2 * cos(4 * timer)) / sin(32);
}
float gx(int x) {
	return sin(fx(x)) * x + cos(x);
}

float rad2deg(double radian)
{
	return radian * 180 / m_pi;
}
float deg2rad(double degree)
{
	return degree * m_pi / 180;
}
glm::vec3 rad2normal(float rad) {
	glm::vec3 velocity = glm::vec3(0, 1, 1) * glm::rotate(glm::mat3(1.0f), rad);
	velocity = glm::vec3(velocity.x, 0, velocity.y);
	return glm::normalize(velocity);
}

//double size = 2.0f;
//double changeScalar = -1.0f;
//cyclone::Vector3 wind(1.0f, 0, 0);
void Mover::update(float duration)
{
 	//m_particle->addForce(wind);
	size = glm::pow((m_particle->getMass() * 3.0 / 4.0 / m_pi), 0.3333);
	m_particle->integrate(duration);

	//checkEdges();
}

/*void Mover::checkEdges()
{

	cyclone::Vector3  pos;
	cyclone::Vector3  vel;
	m_particle->getPosition(&pos);
	m_particle->getVelocity(&vel);
	
	if (pos.x > 100 - size) {
		pos.x = 100 - size;
		//addAccel(vel.x + 10, 0, 0);
		//setAccel(1, 0, 0);
		changeWindDirection(1);
		vel.x *= changeScalar;
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}
	else if (pos.x - size < 0) {
		pos.x = size;
		//addAccel(vel.x + 10, 0, 0);
		//setAccel(1, 0, 0);
		vel.x *= changeScalar;
		changeWindDirection(2);
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}

	if (pos.y > 100 - size) {
		pos.y = 100 - size;
		//setAccel(0, 1, 0);
		//addAccel(0, vel.y + 10, 0);
		changeWindDirection(3);
		vel.y *= changeScalar;
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}
	else if (pos.y < size) {
		pos.y = size;
		//setAccel(0, 1, 0);
		//addAccel(0, vel.y + 10, 0);
		vel.y *= changeScalar;
		changeWindDirection(4);
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}

	if (pos.z > 100 - size)
	{
		pos.z = 100 - size;
		//setAccel(0, 0, 1);
		//addAccel(0, 0, vel.z + 10);
		changeWindDirection(5);
		vel.z *= changeScalar;
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}
	else if (pos.z < size)
	{
		pos.z = size;
		//setAccel(0, 0, 1);
		//addAccel(0, 0, vel.z + 10);
		changeWindDirection(6);
		vel.z *= changeScalar;
		m_particle->setPosition(pos.x, pos.y, pos.z);
		m_particle->setVelocity(vel.x, vel.y, vel.z);
	}
}*/

void Mover::draw(int shadow)
{
	
	cyclone::Vector3 pos;
	m_particle->getPosition(&pos);

	if (shadow)
		glColor4f(0.1,0.1,0.1,0.5f);
	else
		glColor3f(0.1f * m_particle->getMass()/100.0, 0.0f, 0.0f);

	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glutSolidSphere(size, 4, 4);

	glPopMatrix();

}

void Mover::addAccel(float x, float y, float z)
{
	m_particle->addForce(cyclone::Vector3(x, y, z));
}

void Mover::setAccel(float x, float y, float z)
{
	m_particle->setAcceleration(cyclone::Vector3(x, y, z));
}

/*void Mover::changeWindDirection(int Code)
{

	switch (Code)
	{
	case 1:
		wind = cyclone::Vector3(0, 1, 0);
		break;
	case 2:
		wind = cyclone::Vector3(0, 1, 0);
		break;
	case 3:
		wind = cyclone::Vector3(-1, 0, 0);
		break;
	case 4:
		wind = cyclone::Vector3(0, -1, 0);
		break;
	case 5:
		wind = cyclone::Vector3(0, 0, -1);
		break;
	default:
		wind = cyclone::Vector3(1, 0, 0);
		break;
	}
}*/