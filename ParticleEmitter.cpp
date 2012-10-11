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

	gravityOn = false;
	float* v = new float[3];
	gravity = new Particle(v);

	float* p = new float[3];
	p[0] = 5;
	p[1] = 0;

	setGravity(p);
	gravityOn=true;

}

ParticleEmitter::~ParticleEmitter(void) {

	free(orientation);
	free(position);
	free(vector);

	for (int i = 0 ; i < MAX_PARTICLES ; i ++)
		free(particles[i]);

	free(particles);

}

bool ParticleEmitter::isGravityOn(){
	return gravityOn;
}

void ParticleEmitter::emit(){


	float XHI = 0.1f;
	float XLO = -0.1f;

	float YHI = 0.1f;
	float YLO = -0.1f;

	float ZHI = 0.3f;
	float ZLO = 0.2f;

	float* v = new float[3];
	v[0] = XLO + (float)rand()/((float)RAND_MAX/(XHI-XLO));
	v[1] = YLO + (float)rand()/((float)RAND_MAX/(YHI-YLO));
	v[2] = ZLO + (float)rand()/((float)RAND_MAX/(ZHI-ZLO));
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

}

void ParticleEmitter::tick(){

	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++)

		// Alive!
		if(!particles[i]->isDead()){
			//particles[i]->applyForce(force);
			particles[i]->tick(); // Simulate
			particles[i]->applyFriction(0.0001f);

			if(gravityOn)
				particles[i]->applyAttractiveForce(particles[i],gravity,-0.002f,100);

		}

}

void ParticleEmitter::applyWind(float* wind){
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){
		if(!particles[i]->isDead()){
			particles[i]->applyForce(wind);
		}
	}
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

void ParticleEmitter::setGravity(float* pos){
	gravity->setPosition(pos);
}
void ParticleEmitter::turnGravityOn(){
	gravityOn = true;
}
void ParticleEmitter::turnGravityOff(){
	gravityOn = false;
}



void ParticleEmitter::renderParticles() {

	glPushMatrix();

	// Rotate to face vector, might take this out
	glRotatef(
			orientation[0],
			orientation[1],
			orientation[2],
			orientation[3]
	);

	glutWireCone(1,1,3,3);

	glPopMatrix();
	glPushMatrix();


	/*
	 * All particles have the same texture, so we do this here.
	 */
	glEnable(GL_TEXTURE_2D);
	glDepthMask(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE_ARB);

	glBindTexture(GL_TEXTURE_2D, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++)

		// Alive!
		if(!particles[i]->isDead())
			particles[i]->renderParticle(); // Render

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_TEXTURE_2D);

	gravity->renderParticle();


	glPopMatrix();
}

void ParticleEmitter::collideWithBalls(Ball* ball, Collision* c){
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){
		if(c->checkIfCollidedBallParticle(ball, particles[i])){
			int error = 0;
			c->collision3D(1, 100, 0.001, 1, 0.1, ball->position, particles[i]->position, ball->velocity, particles[i]->velocity, error);
		}
	}
}

