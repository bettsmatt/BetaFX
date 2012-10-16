/*
 * ControlPoint.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: stevananet
 */

#include "ControlPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

ControlPoint::ControlPoint() {
	x = y = z = 0.0f;

}

ControlPoint::ControlPoint(float xx, float yy, float zz){
	x = xx;
	y = yy;
	z = zz;
	time = 0;
}

ControlPoint::ControlPoint(float xx, float yy, float zz, float t){
	x = xx;
	y = yy;
	z = zz;
	time = t;
}

ControlPoint::~ControlPoint() {
	// TODO Auto-generated destructor stub
}

void ControlPoint::setColourID(float rr, float gg, float bb){
	r = rr;
	g = gg;
	b = bb;
}


void ControlPoint::multiplyScalar(float f){
	x *= f;
	y *= f;
	z *= f;
}

void ControlPoint::add(ControlPoint p){
	x += p.x;
	y += p.y;
	z += p.z;
}

void ControlPoint::draw(bool pointSelected){
	glPushMatrix();
		glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
		if (pointSelected) {
			glColor3f(0, 0, 1);
		}
		glTranslatef(x, y, z);
		glutSolidSphere(0.25, 10, 10);
	glPopMatrix();
}

void ControlPoint::showTime() {
	glPushMatrix();
		glColor3f(1, 1, 1);
//		glTranslatef(x, y, z);
//		glTranslatef(0, 1, 0);
		glRasterPos2f(x,y + 0.5);
		char s[100];
		sprintf(s, "t: %.2f", time);
		for (int i = 0; i < 100; i++) {
			char c = s[i];
			if (c == '\0')
				break;
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
	glPopMatrix();
}

void ControlPoint::normalize(){
	float mag = sqrt(x*x + y*y + z*z);
	//if(mag > 1){
	x /= mag;
	y /= mag;
	z /= mag;
	//}
}

ControlPoint operator+(const ControlPoint& p1, const ControlPoint& p2) {
	ControlPoint q(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
	return q;
}

ControlPoint operator-(const ControlPoint& p1, const ControlPoint& p2) {
	ControlPoint q(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
	return q;
}

ControlPoint operator*(const ControlPoint& p1, const float& f) {
	ControlPoint q(f * p1.x, f * p1.y, f * p1.z);
	return q;
}

ControlPoint operator/(const ControlPoint& p1, const float& f) {
	ControlPoint q(p1.x / f, p1.y / f, p1.z / f);
	return q;
}

float dotProduct(ControlPoint p, ControlPoint q) {
	float a = 0;
	a += p.x * q.x;
	a += p.y * q.y;
	a += p.z * q.z;
	return a;
}

ControlPoint crossProduct(ControlPoint p, ControlPoint q) {
	ControlPoint cross;
	cross.x = p.y * q.z - p.z * q.y;
	cross.y = p.z * q.x - p.x * q.z;
	cross.z = p.x * q.y - p.y * q.x;
	return cross;
}


