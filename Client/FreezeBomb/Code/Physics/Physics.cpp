#include "Physics.h"


using namespace Physics;

const PVector3 PVector3::GRAVITY = PVector3(0, -9.81, 0);
const PVector3 PVector3::HIGH_GRAVITY = PVector3(0, -19.62, 0);
const PVector3 PVector3::UP = PVector3(0, 1, 0);
const PVector3 PVector3::RIGHT = PVector3(1, 0, 0);
const PVector3 PVector3::OUT_OF_SCREEN = PVector3(0, 0, 1);
const PVector3 PVector3::X = PVector3(0, 1, 0);
const PVector3 PVector3::Y = PVector3(1, 0, 0);
const PVector3 PVector3::Z = PVector3(0, 0, 1);

/*
 * Definition of the sleep epsilon extern.
 */
real Physics::sleepEpsilon = ((real)0.3);

