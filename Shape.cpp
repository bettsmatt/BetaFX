/*
 * Shape.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: stevananet
 */

#include "Shape.h"
#include "ControlPoint.h"
#include "BSpline.h"
#include <stdio.h>
#include <math.h>

Shape::Shape() {
	// TODO Auto-generated constructor stub

}

Shape::~Shape() {
	// TODO Auto-generated destructor stub
}

void Shape::draw(){
	GLUquadric* quad = gluNewQuadric(); //Create a new quadric to allow you to draw cylinders
	glPushMatrix();
		glColor3f(1,1,1);
		glTranslatef(position.x, position.y, position.z);
		glRotatef(angle, rotAxis.x, rotAxis.y, rotAxis.z);
		gluCylinder(quad, 0.2, 0.2, 1.5, 10, 10);
	glPopMatrix();

	gluDeleteQuadric(quad);
}

void Shape::move(BSpline* bs){
	Frame f = bs->nextFrame();
	position = f.ctrlPoint;
	ControlPoint zVector = ControlPoint(0, 0, 1);
	//f.tangent.normalize();

	rotAxis = crossProduct(zVector, f.tangent);
	angle = dotProduct(zVector, f.tangent);
	angle = acos(angle);
	angle = angle * (180.0 / 3.1416);
}
