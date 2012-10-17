/*
 * Plane.cpp
 *
 *  Created on: Oct 14, 2012
 *      Author: caskeythom
 */

#include "Cube.h"
#include "stdio.h"

Cube::Cube(float x, float y, float z, float w, float h){
	position = new float[3];
	position[0] = x; position[1] = y; position[2] = z;

	width = w;
	height = h;
}

Cube::~Cube() {
	// TODO Auto-generated destructor stub
}

void Cube::renderCube() {
	glPushMatrix();

	glColor3f(0.0f, 0.5f, 0.5f);
	glTranslatef(
			position[0],
			position[1],
			position[2]
	);

	//glutSolidSphere(width/20, 20, 20);
	glutSolidCube(width);

	glPopMatrix();
}
