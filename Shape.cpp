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
		//glRotatef(angle, rotAxis.x, rotAxis.y, rotAxis.z);
		gluCylinder(quad, 0.2, 0.2, 1.5, 10, 10);
	glPopMatrix();

	gluDeleteQuadric(quad);
}

void Shape::move(Frame f){
	position = f.ctrlPoint;
		// Find rotation vector normal.
		ControlPoint normal = ControlPoint();
		ControlPoint direction = f.nextCtrlPoint - f.ctrlPoint;
		ControlPoint right = ControlPoint(0,1,0);
		rotAxis = crossProduct(direction, right);
		angle = dotProduct(direction, right);
		angle = angle * (180.0 / 3.1416);

//		p.x = f.point.y * f.nextPoint.z - f.point.z * f.nextPoint.y;
//		p.y = f.point.z * f.nextPoint.x - f.point.x * f.nextPoint.z;
//		p.z = f.point.x * f.nextPoint.y - f.point.y * f.nextPoint.x;
//
//		GLfloat d = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
//		if (d == 0.0) {
//			return;
//		}
//		p.x /= d;
//		p.y /= d;
//		p.z /= d;
//
//			// Find angle of rotation from bone's direction (in degrees).
//		float gore = f.point.x * f.nextPoint.x + f.point.y * f.nextPoint.y + f.point.z * f.nextPoint.z;
//			// |a|
//			float magA = sqrt(f.point.x * f.point.x + f.point.y * f.point.y + f.point.z * f.point.z);
//			// |b|
//			float magB = sqrt(f.nextPoint.x * f.nextPoint.x + f.nextPoint.y * f.nextPoint.y + f.nextPoint.z * f.nextPoint.z);
//
//			float angle = 0.0;
//			angle = acos(gore / (magA * magB));
//			angle = angle * (180.0 / 3.1416);
//
//			rotAxis = p;
		//	glRotatef(angle, p.x, p.y, p.z);
}
