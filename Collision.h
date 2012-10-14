/*
 * Collision.h
 *
 *  Created on: Oct 6, 2012
 *      Author: caskeythom
 */

#ifndef COLLISION_H_
#define COLLISION_H_

#include "Ball.h"
#include "Particle.h"
#include "Cube.h"

#define vectorDotProduct(v,q) ((v)[0] * (q)[0] + (v)[1] * (q)[1] + (v)[2] * (q)[2])
#define vectorMagnitude(v) sqrt(ballVel[0]*ballVel[0] + ballVel[1]*ballVel[1] + ballVel[2]*ballVel[2]);

class Collision {
public:
	Collision();
	~Collision();

	bool checkIfCollidedBalls(Ball*, Ball*);
	bool checkIfCollidedBallParticle(Ball*, Particle*);
	float calculateVectorDistance(float*, float*);
	void collisionBall(double, double, double, double, double, float*, float*, float*, float*);
	void collisionPlane(double, float*, float*);
	void checkCollision(double cor, Cube *c, Ball *b);
	void checkCollisionParticle(double cor, Cube *c, Particle *b);
};

#endif /* COLLISION_H_ */
