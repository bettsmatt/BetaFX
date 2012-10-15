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

Ball::Ball(float* initialPosition, float* initialVelocity, bool sp) {

	// Zero
	position = new float[3];
	velocity = new float[3];
	acceletation = new float[3];
	isSpecial = sp;

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
	// Friction
	/*float friction = 0.00001f;
		for(int i = 0 ; i < 3 ; i ++){
			if(velocity[i] < -0.001) acceletation[i] += friction;
			else if(velocity[i] > 0.001) acceletation[i] += -friction;
			else{
				velocity[i] = 0.0f;
				acceletation[i] = 0.0f;
			}
		}

		printf("Velocity x: %f  y: %f  z: %f\n", velocity[0], velocity[1], velocity[2]);*/
	/*
	 * Apply acceleration to velocity,
	 * Update the position by the new velocity.
	 * Zero the old acceleration
	 */
	acceletation[1] = -0.003f;
	for(int i = 0 ; i < 3 ; i ++){
		velocity[i] += acceletation[i];
		position[i] += velocity[i];
		acceletation[i] = 0;

		if(velocity[i] > 5) velocity[i] = 5;
	}
}

void Ball::renderBall() {

	glPushMatrix();

	if(isSpecial) glColor3f(0.5f, 0.0f, 0.5f);
	else glColor3f(0.5f, 0.5f, 0.0f);
	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	glutSolidSphere(mass,20,20);

	glPopMatrix();

}

void Ball::increaseMass() {
	mass += 0.001;

	if(mass > 2) isSpecial = false;
}
