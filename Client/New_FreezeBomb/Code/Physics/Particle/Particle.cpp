
#include <assert.h>
#include "Particle.h"

using namespace Physics;

void Particle::Intergrate(real duration)
{
	//질량이 0이면 return
	if (inverseMass <= 0.0f) return;


	assert(duration > 0.0);

	//선형 위치를 업뎃
	position.addScaledVector(velocity, duration);

	//힘으로부터 가속도를 계산한다.
	PVector3 resultingAcc = acceleration;
	resultingAcc.addScaledVector(forceAccum, inverseMass);

	//가속에서 선형 속도를 업데이트한다.
	velocity.addScaledVector(resultingAcc, duration);

	//마찰계수 적용
	velocity *= real_pow(damping, duration);

	
	ClearAccumulator();
}

void Particle::SetMass(const real mass)
{
	assert(mass != 0);
	Particle::inverseMass = ((real)1.0) / mass;

}

real Particle::GetMass() const
{
	if (inverseMass == 0)
		return REAL_MAX;
	else
		return ((real)1.0) / inverseMass;
}

void Particle::SetInverseMass(const real inverseMass)
{
	Particle::inverseMass = inverseMass;
}


real Particle::GetInverseMass() const
{
	return inverseMass;
}

bool Particle::HasFiniteMass() const
{
	return inverseMass >= 0.0f;
}

void Particle::SetDamping(const real damping)
{
	Particle::damping = damping;
}

real Particle::GetDamping() const
{
	return damping;
}

void Particle::SetPosition(const PVector3 &position)
{
	Particle::position = position;
}


void Particle::SetPosition(const real x,const real y,const real z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Particle::GetPosition(PVector3 *position) const
{
	*position = Particle::position;
}

PVector3 Particle::GetPosition() const
{
	return position;
}

void Particle::SetVelocity(const PVector3 &velocity)
{
	Particle::velocity = velocity;
}

void Particle::SetVelocity(const real x, const real y,const real z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void Particle::GetVelocity(PVector3 *velocity) const
{
	*velocity = Particle::velocity;
}

PVector3 Particle::GetVelocity() const
{
	return velocity;
}

void Particle::SetAcceleration(const PVector3 &acceleration)
{
	Particle::acceleration = acceleration;
}

void Particle::SetAcceleration(const real x, const real y, const real z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

void Particle::GetAcceleration(PVector3 *acceleration) const
{
	*acceleration = Particle::acceleration;
}

PVector3 Particle::GetAcceleration() const
{
	return acceleration;
}


void Particle::ClearAccumulator()
{
	forceAccum.clear();
}


void Particle::AddForce(const PVector3 &force)
{
	forceAccum += force;
}







