//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Matthew Betts
//
// Written by Matthew Betts
//
// This software is provided 'as-is' for the BetaFX C308 project
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#pragma once

#include "define.h"
#include <GL/glut.h>

class Ball
{
private:

	float* velocity; // x, y, z
	float* acceletation; // x, y, z
	float mass;


public:

	Ball(float*); // Initial velocity
	~Ball(void);

	void tick(); // Simulate one frame
	bool isDead(); // Particle has expired
	void renderBall(); // Draw the particle
	void applyForce(float*); // Apply force to the particle, x, y, z

	float* position; // x, y, z
};
