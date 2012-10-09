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
};

#endif /* COLLISION_H_ */
