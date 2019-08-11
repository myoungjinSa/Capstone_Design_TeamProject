#include <cstdlib>
#include <ctime>
#include "Random.h"

using namespace Physics;

Random::Random()
{
	Seed(0);
}

Random::Random(unsigned seed)
{
	Random::Seed(seed);
}


void Random::Seed(unsigned s)
{
	if(s == 0)
	{
		s = (unsigned)clock();
	}

	//버퍼를 기본적인 난수들로 채운다
	for(unsigned i =0; i<17;++i)
	{
		// Simple linear congruential generator
		s = s * 2891336453 + 1;
		buffer[i] = s;
	}

	//Initiallize Pointers into the buffer;
	p1 = 0;
	p2 = 10;

}

unsigned Random::rotl(unsigned n, unsigned r)
{
	return (n << r) | (n >> (32 - r));
}

unsigned Random::rotr(unsigned n, unsigned r)
{
	return (n >> r) | (n << (32 - r));
}

unsigned Random::RandomBits()
{
	unsigned result;

	//Rotate the buffer and store it back to itself
	result = buffer[p1] = rotl(buffer[p2], 13) + rotl(buffer[p1], 9);

	//Rotate pointers
	if (--p1 < 0) p1 = 16;
	if (--p2 < 0) p2 = 16;

	//Return result
	return result;
}

#ifdef SINGLE_PRECISION
real Random::randomReal()
{
    // Get the random number
    unsigned bits = randomBits();

    // Set up a reinterpret structure for manipulation
    union {
        real value;
        unsigned word;
    } convert;

    // Now assign the bits to the word. This works by fixing the ieee
    // sign and exponent bits (so that the size of the result is 1-2)
    // and using the bits to create the fraction part of the float.
    convert.word = (bits >> 9) | 0x3f800000;

    // And return the value
    return convert.value - 1.0f;
}
#else
real Random::RandomReal()
{
	//Get the random number
	unsigned bits = RandomBits();

	//Set up a reinterpret structure for manipulation
	union {
		real value;
		unsigned words[2];
	}convert;


	// Now assign the bits to the words. This works by fixing the ieee
    // sign and exponent bits (so that the size of the result is 1-2)
    // and using the bits to create the fraction part of the float. Note
    // that bits are used more than once in this process.

	convert.words[0] =  bits << 20; // Fill in the top 16 bits
    convert.words[1] = (bits >> 12) | 0x3FF00000; // And the bottom 20

	 // And return the value
    return convert.value - 1.0;
}


#endif


real Random::RandomReal(real min, real max)
{
	return RandomReal() * (max - min) + min;
}

real Random::RandomReal(real scale)
{
	return RandomReal() * scale;
}

unsigned Random::RandomInt(unsigned max)
{
	return RandomBits() % max;
}

real Random::RandomBinomial(real scale)
{
	return (RandomReal() - RandomReal()) * scale;
}

Quaternion Random::RandomQuaternion()
{
	Quaternion q(RandomReal(),
		RandomReal(),
		RandomReal(),
		RandomReal()
	);

	q.normalize();
	return q;
}



PVector3 Random::RandomVector(real scale)
{
	return PVector3(RandomBinomial(scale),
		RandomBinomial(scale),
		RandomBinomial(scale));

}

PVector3 Random::RandomXZVector(real scale)
{
	return PVector3(
		RandomBinomial(scale),
		0,
		RandomBinomial(scale)
	);
}

PVector3 Random::RandomVector(const PVector3 &scale)
{
	return PVector3(
		RandomBinomial(scale.x),
		RandomBinomial(scale.y),
		RandomBinomial(scale.z)
	);

}

PVector3 Random::RandomVector(const PVector3 &min,const PVector3 &max)
{
	return PVector3(
		RandomReal(min.x, max.x),
		RandomReal(min.y, max.y),
		RandomReal(min.z, max.z)
	);
}

