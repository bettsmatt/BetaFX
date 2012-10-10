/*
 * Collision.h
 *
 *  Created on: Oct 6, 2012
 *      Author: caskeythom
 */

#ifndef COLLISION_H_
#define COLLISION_H_

#include "Ball.h"

class Collision {
public:
	Collision();
	~Collision();

	bool checkIfCollided(Ball*, Ball*);
	float calculateVectorDistance(float*, float*);
	void collision3D(double, double, double, double, double, float*, float*, float*, float*, int);
};

#endif /* COLLISION_H_ */
