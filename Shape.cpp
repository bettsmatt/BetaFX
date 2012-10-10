/*
 * Shape.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: stevananet
 */

#include "Shape.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Point.h"

Shape::Shape() {
	// TODO Auto-generated constructor stub

}

Shape::~Shape() {
	// TODO Auto-generated destructor stub
}

void Shape::drawAt(Point p){
	glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(p.x, p.y, p.z);
		glutSolidSphere(1, 10, 10);
	glPopMatrix();
}
