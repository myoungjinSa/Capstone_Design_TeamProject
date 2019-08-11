#include "../../Physics.h"
#include "../../Random.h"
#include "../Particle.h"
#include <stdio.h>

static Physics::Random crandom;

/**
 * Fireworks are particles, with additional data for rendering and
 * evolution.
 */
class Firework : public Physics::Particle
{
public:
	 /** Fireworks have an integer type, used for firework rules. */
	unsigned type;

	/**
     * The age of a firework determines when it detonates. Age gradually
     * decreases, when it passes zero the firework delivers its payload.
     * Think of age as fuse-left.
     */
	Physics::real age;
	/**
     * Updates the firework by the given duration of time. Returns true
     * if the firework has reached the end of its life and needs to be
     * removed.
     */

	bool Update(Physics::real duration)
	{
		//Update our physical state
		Intergrate(duration);

		//We work backwards from our age to zero.
		age -= duration;
		return (age < 0) || (position.y < 0);
	}
};


/**
 * Firework rules control the length of a firework's fuse and the
 * particles it should evolve into.0
 */

struct FireworkRule
{
	//The type of firework that is managed by this rule
	unsigned type;

	//The minimum length of the fuse
	Physics::real minAge;

	//The Maximum length of the fuse
	Physics::real maxAge;

	//The minimum relative velocity of this firework
	Physics::PVector3 minVelocity;

	//The maximum relative velocity of this firework
	Physics::PVector3 maxVelocity;

	//The damping of this firework type
	Physics::real damping;

	
    /**
     * The payload is the new firework type to create when this
     * firework's fuse is over.
     */
	struct Payload
	{
		//The type of the new particle to create
		unsigned type;

		//The number of particles in this payload
		unsigned count;

		//Sets the payload properties in one go
		void Set(unsigned type,unsigned count)
		{
			Payload::type = type;
			Payload::count = count;
		}

	};

	// The number of payloads for this firework type
	unsigned payloadCount;

	//The set of payloads
	Payload *payloads;
public:
	FireworkRule()
		:payloadCount{0},payloads{nullptr}
	{
	
	}

	void Init(unsigned payloadCount)
	{
		FireworkRule::payloadCount = payloadCount;
		payloads = new Payload[payloadCount];
	}


	~FireworkRule()
	{
		if (payloads != nullptr)
			delete[] payloads;
	}


	//Set all the rule parameters in one go

	void SetParameters(unsigned type, Physics::real minAge, Physics::real maxAge
		, const Physics::PVector3 &minVelocity,const Physics::PVector3 &maxVelocity,
		Physics::real damping)
	{
		FireworkRule::type = type;
		FireworkRule::minAge = minAge;
		FireworkRule::maxAge = maxAge;
		FireworkRule::minVelocity = minVelocity;
		FireworkRule::maxVelocity = maxVelocity;
		FireworkRule::damping = damping;
	}

	 /**
     * Creates a new firework of this type and writes it into the given
     * instance. The optional parent firework is used to base position
     * and velocity on.
     */

	void Create(Firework *firework,const Firework *parent = NULL) const
	{
		firework->type = type;
		firework->age = crandom.RandomReal(minAge, maxAge);

		Physics::PVector3 vel;

		if(parent)
		{
			//The position and velocity are based on the parent.
			firework->SetPosition(parent->GetPosition());
			vel += parent->GetVelocity();
		}
		else
		{
			Physics::PVector3 start;
			int x = (int)crandom.RandomInt(3) - 1;
			start.x = 5.0 * Physics::real(x);
			firework->SetPosition(start);
		}

		vel += crandom.RandomVector(minVelocity, maxVelocity);
		firework->SetVelocity(vel);

		// We use a mass of one in all cases (no point having fireworks
        // with different masses, since they are only under the influence
        // of gravity).

		firework->SetMass(1);

		firework->SetDamping(damping);

		firework->SetAcceleration(Physics::PVector3::GRAVITY);

		firework->ClearAccumulator();

	}

};