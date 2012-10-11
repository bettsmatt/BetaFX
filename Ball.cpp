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

#include "Ball.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Ball::Ball(float* initialPosition, float* initialVelocity) {

	// Zero
	position = new float[3];
	velocity = new float[3];
	acceletation = new float[3];

	// Set initial velocity
	for(int i = 0 ; i < 3 ; i ++)
		velocity[i] = initialVelocity[i];

	for(int i = 0 ; i < 3 ; i ++)
		position[i] = initialPosition[i];

	mass = 1; // Default

}

Ball::~Ball(void) {
	free(position);
	free(velocity);
	free(acceletation);
}


void Ball::applyForce(float* force){
	for(int i = 0 ; i < 3 ; i ++){
		acceletation[i] += force[i] / mass;
	}

}

void Ball::tick (){
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
}

void Ball::renderBall() {

	glPushMatrix();

	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	glutSolidSphere(1,20,20);

	glPopMatrix();

}
