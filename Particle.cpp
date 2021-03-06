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

#define TRAILMAX 10

Particle::Particle(float* pos, float* initialVelocity, float m, G308_Point* cam, bool d) {

	position = new float[3];
	velocity = new float[3];
	acceletation = new float[3];

	for (int i = 0 ; i < 3 ; i ++){
		acceletation[i] = 0;
		position[i] = 0;
		velocity[i] = 0;
		position[i] = pos[i];
		velocity[i] = initialVelocity[i];
	}

	mass = m; // Default

	/*
	 * Life span
	 */
	float HI = 2000;
	float LO = 1500;

	lifeSpanLeft = lifeSpan = LO + (float)rand()/((float)RAND_MAX/(HI-LO));

	camera = cam;
	dies = d;


	trail = (float**) malloc(sizeof(float*) * TRAILMAX);

	for (int i = 0 ; i < TRAILMAX ; i ++){
		trail[i] = new float[3];
		trail[i][0] = pos[0];
		trail[i][1] = pos[1];
		trail[i][2] = pos[2];
	}

}

Particle::~Particle(){
	//delete position;
	//delete velocity;
	//delete acceletation;

	/*
	for (int i = 0 ; i < TRAILMAX ; i ++){
		free(trail[i]);
	}*/

	//free(trail);

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

	if(dies){
		lifeSpanLeft--;
	}

	for(int i = TRAILMAX - 1 ; i > 0 ; i --){
		trail[i][0] = trail[i - 1][0];
		trail[i][1] = trail[i - 1][1];
		trail[i][2] = trail[i - 1][2];
	}

	trail[0][0] = position[0];
	trail[0][1] = position[1];
	trail[0][2] = position[2];

}

/*
 * Order based on dist from the camera
 */
float Particle::Dist(){

	return sqrt(
			(position[0] * position[0]) - (camera->x * camera->x) +
			(position[1] * position[1]) - (camera->y * camera->y) +
			(position[2] * position[2]) - (camera->z * camera->z)
	);
}

void Particle::RenderMe(){
	renderParticle();
}

void Particle::renderParticle() {

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE_ARB);

	glBindTexture(GL_TEXTURE_2D, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


// SUN
	if(!dies){
		glColor3f(
				0,
				1,
				0
		);
	}	// Fade color from red to blue over time
	else{
		glColor3f(
				(float)lifeSpanLeft / (float)lifeSpan,
				0,
				1 - ((float)lifeSpanLeft / (float)lifeSpan)
		);
	}

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


	float size = 0.3f;

	if(!dies)
		size = 1;

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

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glBegin(GL_LINE);

	for(int i = 0 ; i < TRAILMAX - 1 ; i ++){

		glVertex3f(
				trail[i][0],
				trail[i][1],
				trail[i][2]
		);

		glVertex3f(
				trail[i + 1][0],
				trail[i + 1][1],
				trail[i + 1][2]
		);


	}
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
	delete[] f;
}

void Particle::applyAttractiveForce(Particle* p1, Particle* p2, float strength, float minDist){

	float* vec = (float*) malloc (sizeof(float) * 3);

	// Vector between particles
	for(int i = 0 ; i < 3 ; i ++){
		vec[i] = 0;
		vec[i] = p2->position[i] - p1->position[i];
	}

	// Distance
	float dist = sqrt(vec[0] * vec[0] +	vec[1] * vec[1] +  vec[2] * vec[2]);

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
		delete[] vecN;
	}

	free(vec);



}
