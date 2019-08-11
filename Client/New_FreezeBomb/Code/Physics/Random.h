#pragma once

#include "Physics.h"

namespace Physics
{
	 /**
     * Keeps track of one random stream: i.e. a seed and its output.
     * This is used to get random numbers. Rather than a funcion, this
     * allows there to be several streams of repeatable random numbers
     * at the same time. Uses the RandRotB algorithm.
     */

	class Random
	{
	public:

		//left bitwise rotation
		unsigned rotl(unsigned n, unsigned r);

		//right bitwise rotation
		unsigned rotr(unsigned n, unsigned r);

		/*
			Creates a new random number stream with a seed based on
			timing data.
		*/
		Random();

		/**
        * Creates a new random stream with the given seed.
        */
		Random(unsigned seed);


		/*
			Sets the seed value for the random stream.
		*/
		void Seed(unsigned seed);


		/**
        * Returns the next random bitstring from the stream. This is
        * the fastest method.
        */
		unsigned RandomBits();

		/**
        * Returns a random floating point number between 0 and 1.
        */
		real RandomReal();

		 /**
         * Returns a random floating point number between 0 and scale.
         */
		real RandomReal(real scale);


		 /**
         * Returns a random floating point number between min and max.
         */
		real RandomReal(real min, real max);

		/**
         * Returns a random integer less than the given value.
         */
		unsigned RandomInt(unsigned max);

		 /**
         * Returns a random binomially distributed number between -scale
         * and +scale.
         */

		real RandomBinomial(real scale);

		
		
        /**
        * Returns a random vector where each component is binomially
        * distributed in the range (-scale to scale) [mean = 0.0f].
        */
		PVector3 RandomVector(real scale);


		/**
        * Returns a random vector where each component is binomially
        * distributed in the range (-scale to scale) [mean = 0.0f],
        * where scale is the corresponding component of the given
        * vector.
        */
		PVector3 RandomVector(const PVector3 &scale);

		
        /**
         * Returns a random vector in the cube defined by the given
         * minimum and maximum vectors. The probability is uniformly
         * distributed in this region.
         */
		PVector3 RandomVector(const PVector3 &min, const PVector3 &max);

		/**
         * Returns a random vector where each component is binomially
         * distributed in the range (-scale to scale) [mean = 0.0f],
         * except the y coordinate which is zero.
         */
		PVector3 RandomXZVector(real scale);


		/**
         * Returns a random orientation (i.e. normalized) quaternion.
         */
		Quaternion RandomQuaternion();

	private:

		//Internal mechanics
		int p1, p2;
		unsigned buffer[17];

	};

}