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

void Particle::init(float* pos, float* initialVelocity, float m, Camera* cam) {

	position = new float[3];
	velocity = new float[3];
	acceletation = new float[3];


	for (int i = 0 ; i < 3 ; i ++){
		position[i] = pos[i];
		velocity[i] = initialVelocity[i];
	}

	mass = m; // Default

	/*
	 * Lifespan
	 */
	float HI = 20000;
	float LO = 18000;
	lifeSpanLeft = lifeSpan = LO + (float)rand()/((float)RAND_MAX/(HI-LO));

	camera = cam;

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

float Particle::Dist(){
	return sqrt(
			(position[0] * position[0]) - (camera->rotx * camera->rotx) +
			(position[1] * position[1]) - (camera->rotx * camera->rotx) +
			(position[2] * position[2]) - (camera->rotx * camera->rotx)
   );
}

void Particle::RenderMe(){
	renderParticle();
}

void Particle::renderParticle() {

	glPushMatrix();

	// Fade color from red to blue over time
	glColor3f(
			(float)lifeSpanLeft / (float)lifeSpan,
			0,
			1 - ((float)lifeSpanLeft / (float)lifeSpan)
	);

	// Translate to where the particle is
	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	/*
	 * Since we always want the particles to face the camera we need to billboard
	 * This is the cheapest implementation, and will serve for now,
	 * We just remove the rotation information from the modelview matrix
	 */

	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	// Undo all rotations, scaling lost too
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ ) {
			if ( i==j )
				modelview[i*4+j] = 1.0;
			else
				modelview[i*4+j] = 0.0;
		}

	// Set the modelview with no rotations and scaling
	glLoadMatrixf(modelview);



	float size = 0.1f;

	// Draw Quad
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex3f(-1 * size, 1 * size, 0.0f);

		glTexCoord2f(1, 1);
		glVertex3f( 1 * size, 1 * size, 0.0f);

		glTexCoord2f(1, 0);
		glVertex3f( 1 * size,-1 * size, 0.0f);

		glTexCoord2f(0,0);
		glVertex3f(-1 * size,-1 * size, 0.0f);
	glEnd();




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
		vec[i] = p2->position[i] - p1->position[i];

	// Distance
	float dist = sqrt(
			vec[0] * vec[0] +
			vec[1] * vec[1] +
			vec[2] * vec[2]
	);

	// Close enough
	if(dist < minDist){

		float f = (strength * p1->mass * p2->mass) / (dist * dist);

		float* vecN = new float[3];

		// Normalize & apply force
		for(int i = 0 ; i < 3 ; i ++)
			vecN[i] = (vec[i] / dist) * f;

		// Apply to each particle
		p1->applyForce(vecN);

		/* Limited to one way
		// Invert for p2
		for(int i = 0 ; i < 3 ; i ++)
			vecN[i] = -vecN[i];

		// Apply to p2
		p2->applyForce(vecN);
		 */
	}

	free(vec);



}
