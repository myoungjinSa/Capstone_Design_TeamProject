#pragma once
#include "../Physics.h"

namespace Physics
{
	//A particle is the simplest object that can be simulated in the
    //physics system.

	/*It has position data (no orientation data), along with
     * velocity. It can be integrated forward through time, and have
     * linear forces, and impulses applied to it. The particle manages
     * its state and allows access through a set of methods.
	 */
	class Particle
	{
	protected:
		 /**
         * @name Characteristic Data and State
         *
         * This data holds the state of the particle. There are two
         * sets of data: characteristics and state.
         *
         * Characteristics are properties of the particle
         * independent of its current kinematic situation. This
         * includes mass, moment of inertia and damping
         * properties. Two identical particles will have the same
         * values for their characteristics.
         *
         * State includes all the characteristics and also includes
         * the kinematic situation of the particle in the current
         * simulation. By setting the whole state data, a particle's
         * exact game state can be replicated. Note that state does
         * not include any forces applied to the body. Two identical
         * rigid bodies in the same simulation will not share the same
         * state values.
         *
         * The state values make up the smallest set of independent
         * data for the particle. Other state data is calculated
         * from their current values. When state data is changed the
         * dependent values need to be updated: this can be achieved
         * either by integrating the simulation, or by calling the
         * calculateInternals function. This two stage process is used
         * because recalculating internals can be a costly process:
         * all state changes should be carried out at the same time,
         * allowing for a single call.
         *
         * @see calculateInternals
         */
        /*@{*/

		//입자의 질량의 역 (inverse)을 유지합니다.
		//역 질량을 유지하는 것이 더 유용합니다. 
		//왜냐하면 통합이 더 간단하고 실시간으로 시뮬레이션을 사용하면 
		//무한 질량 (움직일 수없는)의 질량을 가진 물체 (수치 시뮬레이션에서는 완전히 불안정한 물체)를 갖는 것이 더 유용합니다.
		real inverseMass;


		 /**
         * Holds the amount of damping applied to linear
         * motion. Damping is required to remove energy added
         * through numerical instability in the integrator.
         */


		/*
		직선 운동에 적용되는 댐핑 양을 유지합니다. 
		감쇠는 적분기의 수치 적 불안정성을 통해 추가 된 에너지를 제거하는 데 필요합니다.
		*/
        real damping;

		 /**
         * Holds the linear position of the particle in
         * world space.
         */
        PVector3 position;

		 /**
         * Holds the linear velocity of the particle in
         * world space.
         */
        PVector3 velocity;


		        /*@}*/

        /**
         * @name Force Accumulators
         *
         * These data members store the current force and
         * global linear acceleration of the particle.

		 
		이 데이터 멤버는 현재 힘과 입자의 글로벌 선형 가속을 저장합니다
         */

        /*@{*/

        /**
         * Holds the accumulated force to be applied at the next
         * simulation iteration only. This value is zeroed at each
         * integration step.
		 
			누적 된 힘을 다음 시뮬레이션 반복시에만 적용합니다.
			이 값은 각 통합 단계에서 0입니다
         */


        PVector3 forceAccum;

        /**
         * Holds the acceleration of the particle.  This value
         * can be used to set acceleration due to gravity (its primary
         * use), or any other constant acceleration.
		 
		입자의 가속도를 유지합니다. 
		이 값은 중력 (주 사용) 또는 기타 일정한 가속으로 인해 가속을 설정하는 데 사용할 수 있습니다.
         */
        PVector3 acceleration;

        /*@}*/

		public:

			/**
		    * Integrates the particle forward in time by the given amount.
			* This function uses a Newton-Euler integration method, which is a
			* linear approximation to the correct integral. For this reason it
			* may be inaccurate in some cases.
			*/
			void Intergrate(real duration);

			/**
			* Sets the mass of the particle.
			*
			 * @param mass The new mass of the body. This may not be zero.
			 * Small masses can produce unstable rigid bodies under
			 * simulation.
			 *
			 * @warning This invalidates internal data for the particle.
			 * Either an integration function, or the calculateInternals
			 * function should be called before trying to get any settings
			 * from the particle.
			 */
			void SetMass(const real mass);


			 /**
			 * Gets the mass of the particle.
			*
			 * @return The current mass of the particle.
			 */
			real GetMass() const;


			  /**
			* Sets the inverse mass of the particle.
			*
			 * @param inverseMass The new inverse mass of the body. This
			 * may be zero, for a body with infinite mass
			 * (i.e. unmovable).
			 *
			 * @warning This invalidates internal data for the particle.
			 * Either an integration function, or the calculateInternals
			 * function should be called before trying to get any settings
			 * from the particle.
			 */
			void SetInverseMass(const real inverseMass);

			        /**
         * Gets the inverse mass of the particle.
         *
         * @return The current inverse mass of the particle.
         */
        real GetInverseMass() const;

	    /**
        * Returns true if the mass of the particle is not-infinite.
        */
        bool HasFiniteMass() const;


		 /**
         * Sets both the damping of the particle.
         */
        void SetDamping(const real damping);

        /**
         * Gets the current damping value.
         */
        real GetDamping() const;

		/**
        * Sets the position of the particle.
        *
        * @param position The new position of the particle.
        */
        void SetPosition(const PVector3 &position);


		 /**
         * Sets the position of the particle by component.
         *
         * @param x The x coordinate of the new position of the rigid
         * body.
         *
         * @param y The y coordinate of the new position of the rigid
         * body.
         *
         * @param z The z coordinate of the new position of the rigid
         * body.
         */
        void SetPosition(const real x, const real y, const real z);

		 /**
         * Fills the given vector with the position of the particle.
         *
         * @param position A pointer to a vector into which to write
         * the position.
         */
        void GetPosition(PVector3 *position) const;


		 /**
         * Gets the position of the particle.
         *
         * @return The position of the particle.
         */
        PVector3 GetPosition() const;


		 /**
         * Sets the velocity of the particle.
         *
         * @param velocity The new velocity of the particle.
         */
        void SetVelocity(const PVector3 &velocity);

		void SetVelocity(const real x, const real y, const real z);

		void GetVelocity(PVector3 *velocity) const;

		PVector3 GetVelocity() const;

		 /**
         * Sets the constant acceleration of the particle.
         *
         * @param acceleration The new acceleration of the particle.
         */
        void SetAcceleration(const PVector3 &acceleration);

		void SetAcceleration(const real x, const real y, const real z);

		void GetAcceleration(PVector3 *acceleration) const;

		PVector3 GetAcceleration() const;

		/**
         * Clears the forces applied to the particle. This will be
         * called automatically after each integration step.
         */
        void ClearAccumulator();

		/**
        * Adds the given force to the particle, to be applied at the
        * next iteration only.
        *
        * @param force The force to apply.
        */
        void AddForce(const PVector3 &force);

	};

}
