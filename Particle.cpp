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
	lifeSpanLeft = 1000; // Default

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

	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	glutSolidSphere(0.1,3,3);

	glPopMatrix();

}
