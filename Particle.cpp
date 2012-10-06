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

#include "Particle.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Particle::Particle(float* initialVelocity) {

	// Zero
	position = new float[3];
	velocity = new float[3];
	acceletation = new float[3];

	// Set initial velocity
	for(int i = 0 ; i < 3 ; i ++)
		velocity[i] = initialVelocity[i];

	mass = 1; // Default


	/*
	 * Color
	 */

	color = new float [3];

	float HI = 1;
	float LO = 0;
	color[0] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
	color[1] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
	color[2] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));

	/*
	 * Lifespan
	 */
	HI = 2000;
	LO = 500;
	lifeSpanLeft = LO + (float)rand()/((float)RAND_MAX/(HI-LO));

}

Particle::~Particle(void) {
	free(position);
	free(velocity);
	free(acceletation);
}


bool Particle::isDead(){

	// Particle's timer has run down
	return lifeSpanLeft <= 0;
}


void Particle::applyForce(float* force){

	for(int i = 0 ; i < 3 ; i ++){
		acceletation[i] += force[i] / mass;
	}

}

void Particle::tick (){

	/*
	 * Apply acceleration to velocity,
	 * Update the position by the new velocity.
	 * Zero the old acceleration
	 */
	for(int i = 0 ; i < 3 ; i ++){
		velocity[i] += acceletation[i];
		position[i] += velocity[i];
		acceletation[i] = 0;
	}

	lifeSpanLeft--;

}

void Particle::renderParticle() {

	glPushMatrix();

	glColor3f(
			color[0],
			color[1],
			color[2]
	);

	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	glutSolidSphere(0.1,3,3);

	glPopMatrix();

}

void Particle::setPosition(float* pos){
	for(int i = 0 ; i < 3 ; i ++)
		position[i] = pos[i];
}

void Particle::applyFriction(float friction){
	float* f = new float [3];
	for(int i = 0 ; i < 3 ; i++)
		f[i] = velocity[i] * -friction;
	applyForce(f);
}

void Particle::applyAttractiveForce(Particle* p1, Particle* p2, float strength, float minDist){

	float* vec = (float*) malloc (sizeof(float) * 3);

	// Vector between particles
	for(int i = 0 ; i < 3 ; i ++)
		vec[i] = p1->position[i] - p2->position[i];

	// Distance
	float dist = sqrt(
			vec[0] * vec[0] +
			vec[1] * vec[1] +
			vec[2] * vec[2]
	 );

	// Close enough
	if(dist < minDist){

		float f = (strength * p1->mass * p2->mass) / (dist / dist);

		float* vecN = new float[3];

		// Normalize & apply force
		for(int i = 0 ; i < 3 ; i ++)
			vecN[i] = (vec[i] / dist) * f;

		// Apply to each particle
		p1->applyForce(vecN);

		// Invert for p2
		for(int i = 0 ; i < 3 ; i ++)
			vecN[i] = -vecN[i];

		// Apply to p2
		p2->applyForce(vecN);

	}

	free(vec);



}
