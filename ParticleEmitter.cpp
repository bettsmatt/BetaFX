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
#include <queue>
#include <string.h>
#include <vector>

#define PI 3.14159265359f
#define MAX_PARTICLES 1000

using namespace std;

ParticleEmitter::ParticleEmitter(G308_Point* c) {

	camera = c;
	orientation = new float[4];
	position = new float[3];
	vec = new float[3];

	particles = (Particle**) malloc (sizeof(Particle*) * MAX_PARTICLES);

	index = 0;
	created = 0;

	numGravity = 0;
	gravityOn = false;


}

void ParticleEmitter::removeSuns(){

	int oldNumG = numGravity;
	numGravity = 0;
	gravityOn = false;

	for (int i = 0 ; i < oldNumG ; i ++){
		free(gravity[i]);
	}


}

void ParticleEmitter::removeParticles(){
	int oldCreated = created;
	created = 0;
	index = 0;
	gravityOn = false;

	for(int i = 0 ; i < oldCreated && i < MAX_PARTICLES ; i ++)
		free(particles[i]);

}
void ParticleEmitter::spawnSuns(){
	removeSuns();
	numGravity = 3;

	float** v = (float**) malloc (sizeof (float) * numGravity);
	float** p = (float**) malloc (sizeof (float) * numGravity);

	for(int i = 0 ; i < numGravity ; i ++){
		v[i] = new float[3];
		p[i] = new float[3];
	}

	for(int i = 0 ; i < numGravity ; i ++){
		p[i][0] = 15.0f * sin((PI * 2 / numGravity) * i );
		p[i][2] = 15.0f * cos((PI * 2 / numGravity) * i );
		p[i][1] = 0;
	}


	for(int i = 0 ; i < numGravity ; i ++){

		// Up
		float* up = new float[3];
		up[1] = 1;

		// Take cross product of position and z, multiply by some number
		v[i][0] = (up[1] * p[i][2] - up[2] * p[i][1]) * 0.002f;
		v[i][1] = (up[2] * p[i][0] - up[0] * p[i][2]) * 0.002f;
		v[i][2] = (up[0] * p[i][1] - up[1] * p[i][0]) * 0.002f;

	}

	/*
	 * Test Gravity Wells
	 */
	gravity = (Particle**) malloc (sizeof(Particle*) * numGravity);
	for(int i = 0 ; i < numGravity ; i ++){
		gravity[i] = new Particle (p[i],v[i], 100, camera, false);
	}

	gravityOn = true;


}

void ParticleEmitter::spawnSun(){
	removeSuns();
	numGravity = 1;

	float** v = (float**) malloc (sizeof (float) * numGravity);
	float** p = (float**) malloc (sizeof (float) * numGravity);

	for(int i = 0 ; i < numGravity ; i ++){
		v[i] = new float[3];
		p[i] = new float[3];
	}

	p[0][0] = 10;
	p[0][1] = 2;
	p[0][2] = 1;

	gravity = (Particle**) malloc (sizeof(Particle*) * numGravity);
	for(int i = 0 ; i < numGravity ; i ++){
		gravity[i] = (Particle*) malloc (sizeof(Particle));
		gravity[i] = new Particle (p[i],v[i], 100, camera, false);
	}

	gravityOn = true;
}


ParticleEmitter::~ParticleEmitter(void) {

	free(orientation);
	free(position);
	free(vec);

	for (int i = 0 ; i < MAX_PARTICLES ; i ++)
		free(particles[i]);

	free(particles);

}

bool ParticleEmitter::isGravityOn(){
	return gravityOn;
}

void ParticleEmitter::cloud(int num, float radius){


	for(int i = 0 ; i < num ; i ++){

		float* p = new float[3];
		float* v = new float[3];



		for(int j = 0 ; j < 3 ; j ++){
			v[j] = 0;
			p[j] = 0 + sin((float)rand()/((float)RAND_MAX/(PI*2-0))) * (float)rand()/((float)RAND_MAX/(radius-0));
		}
		create(p,v,1.0f);

	}


}
void ParticleEmitter::create(float* p, float * v, float m){

	if(index >= MAX_PARTICLES)
		index = 0;

	delete particles[index];

	particles[index] = new Particle(p,v, 1, camera,true);

	index++;
	created++;


}
/*
void ParticleEmitter::create(float* p, float * v, float m){

	int start = index;


	// Make a new particle and increment the index
	if(created < MAX_PARTICLES){
		particles[index] = (Particle*) malloc (sizeof(Particle));
		particles[index++]->init(p,v, 1, camera,true);
		created++;
	}

	else{

		// Loop from current index up looking for expired particles
		for (int i = index ; i < MAX_PARTICLES ; i ++ ){

			// Replace the dead one
			if( particles[i]->isDead()){
				particles[index] = (Particle*) malloc (sizeof(Particle));
				particles[index++]->init(p, v, 1, camera,true);
				index = i;
				goto found; // I always wanted to use one of these :)
			}
		}

		// Loop from start to the expired index, if we haven't found anything yet
		for(int i = 0 ; i < start ; i ++)
			if(particles[i]->isDead()){
				particles[index] = (Particle*) malloc (sizeof(Particle));
				particles[index++]->init(p, v, 1, camera,true);
				index = i;
				goto found; // I always wanted to use one of these :)
			}
	}

	found:; // Never again...

}
 */
void ParticleEmitter::emit(){


	float XHI = 0.1f;
	float XLO = -0.1f;

	float YHI = 0.1f;
	float YLO = -0.1f;

	float ZHI = 0.4f;
	float ZLO = 0.3f;

	float* v = new float[3];
	v[0] = XLO + (float)rand()/((float)RAND_MAX/(XHI-XLO));
	v[1] = YLO + (float)rand()/((float)RAND_MAX/(YHI-YLO));
	v[2] = ZLO + (float)rand()/((float)RAND_MAX/(ZHI-ZLO));

	float* p = new float[3];
	p[0] = position[0];
	p[1] = position[1];
	p[2] = position[2];

	create(p,v,1.0f);

	free(v);
	free(p);

}

void ParticleEmitter::tick(){

	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){

		// Alive!
		if(!particles[i]->isDead()){

			//particles[i]->applyFriction(0.01f);

			if(gravityOn)
				for(int j = 0 ; j < numGravity ; j ++)
					Particle::applyAttractiveForce(particles[i], gravity[j], 0.01f, 1000);

			particles[i]->tick(); // Simulate

		}

		if(gravityOn){
			for(int i = 0 ; i < numGravity ; i ++){
				for(int j = 0 ; j < numGravity ; j ++){
					if(i != j ){
						Particle::applyAttractiveForce(gravity[i], gravity[j], 0.001f, 1000);
					}
				}


				for(int i = 0 ; i < numGravity ; i ++){
					gravity[i]->tick();
				}

			}
		}
	}
}

void ParticleEmitter::applyWind(float* wind){
	/*
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){
		if(!particles[i]->isDead()){
			particles[i]->applyForce(wind);
		}
	}
	 */
}


void ParticleEmitter::setVector(float* v){

	// Copy vec
	for(int i = 0 ; i < 3 ; i ++)
		vec[i] = v[i];

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

void ParticleEmitter::turnGravityOn(){
	gravityOn = true;
}
void ParticleEmitter::turnGravityOff(){
	gravityOn = false;
}

class ParticleComp {
public:
	bool operator()(Particle& t1, Particle& t2)
	{
		return t1.Dist() > t2.Dist();
	}
};

void ParticleEmitter::renderParticles() {

	glPushMatrix();
	glLoadIdentity();

	// Order by distance from camera.
	priority_queue<Particle, std::vector<Particle>, ParticleComp> pq;

	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){
		if(!particles[i]->isDead()){
			pq.push(*particles[i]);
		}
	}

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
	//glDepthMask(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE_ARB);

	glBindTexture(GL_TEXTURE_2D, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	int size = pq.size();
	for(int i = 0 ; i < size ; i ++){
		Particle p = pq.top();
		p.renderParticle();
		pq.pop();

	}

	/*
	// Loop through all active particles drawing them
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++)

		// Alive!
		if(!particles[i]->isDead())
			particles[i]->renderParticle(); // Render
	 */
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_TEXTURE_2D);
	//glDepthMask(1);
	for(int i = 0; i < numGravity ; i ++)
		gravity[i]->renderParticle();


	glPopMatrix();
}

void ParticleEmitter::collideWithBalls(Ball* ball, Collision* c){
	for(int i = 0; i < MAX_PARTICLES && i < created ; i ++){
		if(c->checkIfCollidedBallParticle(ball, particles[i])){
			if(ball->isSpecial){
				ball->increaseMass();
				particles[i]->lifeSpanLeft = 0;
			}
			else{
				float *ballVelocity = new float[3];
				float *ballPosition = new float[3];
				for(int j = 0; j < 3; j++){
					ballVelocity[j] = ball->velocity[j];
					ballPosition[j] = ball->position[j];
				}

				c->collisionBall(1, 100, 0.001, 1, 0.1, ballPosition, particles[i]->position, ballVelocity, particles[i]->velocity);
			}
		}
	}
}
