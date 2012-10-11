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

class Collision {
public:
	Collision();
	~Collision();

	bool checkIfCollidedBalls(Ball*, Ball*);
	bool checkIfCollidedBallParticle(Ball*, Particle*);
	float calculateVectorDistance(float*, float*);
	void collision3D(double, double, double, double, double, float*, float*, float*, float*, int);
};

#endif /* COLLISION_H_ */
