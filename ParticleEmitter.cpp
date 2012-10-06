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

#include "ParticleEmitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PARTICLES 10000

ParticleEmitter::ParticleEmitter(void) {

	orientation = new float[4];
	position = new float[3];
	vector = new float[3];

	particles = (Particle**) malloc (sizeof(Particle*) * MAX_PARTICLES);

	index = 0;
	created = 0;

}

ParticleEmitter::~ParticleEmitter(void) {

	free(orientation);
	free(position);
	free(vector);

	for (int i = 0 ; i < MAX_PARTICLES ; i ++)
		free(particles[i]);

	free(particles);

}

void ParticleEmitter::emit(){

	/*
	 * No Initial velocity
	 */
	float HI = 0.02f;
	float LO = -0.02f;

	float* v = (float*) malloc(sizeof(float) * 3);
	v[0] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
	v[1] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
	v[2] = LO + (float)rand()/((float)RAND_MAX/(HI-LO));
	int start = index;


	// Make a new particle and increment the index
	if(created < MAX_PARTICLES){
		particles[index++] = new Particle(v);
		created++;
	}

	else{

		// Loop from current index up looking for expired particles
		for (int i = index ; i < MAX_PARTICLES ; i ++ ){

			// Replace the dead one
			if( particles[i]->isDead()){
				particles[i] = new Particle(v);
				index = i;
				goto found; // I always wanted to use one of these :)
			}
		}

		// Loop from start to the expired index, if we haven't found anything yet
		for(int i = 0 ; i < start ; i ++)
			if(particles[i]->isDead()){
				particles[i] = new Particle(v);
				index = i;
				goto found; // I always wanted to use one of these :)
			}
	}

found:; // Never again...

//free(v);


}

void ParticleEmitter::tick(){

	/*
	float* force = (float*) malloc (sizeof (float) * 3 );
	force[0] = 0.02f; force[1] = 0.01f; force[2] = 0.06f;
	 */
	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++)

		// Alive!
		if(!particles[i]->isDead()){
			//particles[i]->applyForce(force);
			particles[i]->tick(); // Simulate

		}

	//free(force);
}


void ParticleEmitter::setVector(float* v){

	// Copy vector
	for(int i = 0 ; i < 3 ; i ++)
		vector[i] = v[i];

	/*
	 * Get the eular rotations for a vector,
	 * So we can draw the emitter correctly
	 */

	// Up Vector
	G308_Point forward;
	forward.z = 1;
	forward.x = 0;
	forward.y = 0;

	// Bone Vector
	G308_Point vp;
	vp.x = v[0];
	vp.y = v[1];
	vp.z = v[2];

	// Cross
	G308_Point np;
	np.x = forward.y * vp.z - forward.z * vp.y;
	np.y = forward.z * vp.x - forward.x * vp.z;
	np.z = forward.x * vp.y - forward.y * vp.x;

	//A * B
	double AB = forward.x * vp.x + forward.y * vp.y + forward.z * vp.z;

	// Normalise
	double A = pow((forward.x * forward.x + forward.y * forward.y + forward.z * forward.z),0.5);
	double B  = pow((vp.x * vp.x + vp.y * vp.y + vp.z * vp.z), 0.5);

	float angle = AB / (A * B);

	// Inverse cosine angle
	float convertedAngle = acos(angle) * 180 / M_PI;

	// Save orientation data for drawing emitter
	orientation[0] = convertedAngle;
	orientation[1] = np.x;
	orientation[2] = np.y;
	orientation[3] = np.z;

}



void ParticleEmitter::renderParticles() {

	glPushMatrix();

	// Rotate to face vector
	glRotatef(
			orientation[0],
			orientation[1],
			orientation[2],
			orientation[3]
	);

	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++)

		// Alive!
		if(!particles[i]->isDead())
			particles[i]->renderParticle(); // Render


	glutWireCone(1,3,3,3);

	glPopMatrix();

}
