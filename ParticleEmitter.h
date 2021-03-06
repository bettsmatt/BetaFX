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
#include "Particle.h"
#include "Ball.h"
#include "Collision.h"
#include <vector>

class ParticleEmitter
{
private:

	float* position; // x, y, z
	float* vec; // x, y, z
	float* orientation; // Angle, x, y, z

	bool on;
	float tickMult; // simulation multiplier

	Particle** particles; // particles
	int created;
	int index;

	int numGravity;
	Particle** gravity;
	bool gravityOn;

	void create(float*, float*, float);

	G308_Point* camera;


public:

	float* getPosition(); // Get the world coordinates
	float* getVector(); // Get the world vector

	void setPosition(float*); // Set the world coordinates
	void setVector(float*); // Set the world vector

	void turnOn(); // Turn on
	void turnOff(); // Turn off
	bool isOn(); // Check if on

	void tick(); // Run a single frame of simulation

	void emit(); // Emit a particle
	void cloud(int , float); // Make a cloud

	void applyWind(float*); // Apply a force to all the particles

	void setGravity(float*); // Sets the center of gravity, one beef cake particle
	void turnGravityOn();
	void turnGravityOff();
	bool isGravityOn();

	ParticleEmitter(G308_Point*); // Defaults
	ParticleEmitter(char*); // Load from *.pec file
	~ParticleEmitter(void);

	void renderParticles();     // Render
	void toggleRenderMode();	// Toggle between wire frame, solid and textured

	void collideWithBalls(Ball*, Collision*);

	void removeParticles(); // Remove all particles from the scene
	void removeSuns(); // remove all suns from the emmitter

	void spawnSuns(); // Spawn 3 suns
	void spawnSun(); // Spawn single sun

};
