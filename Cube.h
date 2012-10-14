/*
 * Plane.h
 *
 *  Created on: Oct 14, 2012
 *      Author: caskeythom
 */
#pragma once

#include "define.h"
#include <GL/glut.h>

class Cube {
public:
	Cube(float, float, float, float, float);
	~Cube();
	float *position;
	float width, height;
	void renderCube();
};
