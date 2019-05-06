#pragma once
#include <math.h>
#include "Precision.h"

namespace Physics
{
	extern real sleepEpsilon;

	void setSleepEpsilon(real value);

	real getSleepEpsilon();

	//Physics 네임 스페이스 안에 정의 되어 있어서 
	//XMFLOAT3과 충돌이 일어나지 않음
	class Vector3
	{
	public:
		real x;

		real y;

		real z;

	private:
		// padding to ensure 4 word alignment
		real pad;

	public:
		//default constructor creates a zero vector
		Vector3() : x(0), y(0), z(0) {}

		Vector3(const real x, const real y, const real z)
			:x(x), y(y), z(z) {}

		const static Vector3 GRAVITY;
        const static Vector3 HIGH_GRAVITY;
        const static Vector3 UP;
        const static Vector3 RIGHT;
        const static Vector3 OUT_OF_SCREEN;
        const static Vector3 X;
        const static Vector3 Y;
        const static Vector3 Z;
	};
}