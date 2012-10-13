/*
 * Camera.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: stevananet
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include "Camera.h"
#include "define.h"
#include "ControlPoint.h"
#include "BSpline.h"


Camera::Camera() {
	zoom, rotx, roty, tx, ty = 0.0f;

}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

void Camera::setInitialCameraPosition(double winWidth, double winHeight) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, winWidth / winHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
				0, 0, 50,
				0, 0, 0,
				0, 1, 0
		);
}

void Camera::rotateCamera(){
	glTranslatef(tx, ty, -zoom);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
}

//--------------------------------------------------------------------------------
//
void Camera::resetCamera(float* zoom, float* tx, float* ty, float* rotx, float* roty){
	*zoom = 0;
	*tx = 0;
	*ty = 0;
	*rotx = 0;
	*roty = 0;
}

void Camera::lookAt(Frame f, double winWidth, double winHeight){
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, winWidth / winHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	if (f.isLast == 1) {
		gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, 0, 0, 0, 0, 1, 0);
	}
	else{
//		Point p = Point();
//		p.x = f.ctrlPoint.y * f.nextPoint.z - f.ctrlPoint.z * f.nextPoint.y;
//		p.y = f.ctrlPoint.z * f.nextPoint.x - f.ctrlPoint.x * f.nextPoint.z;
//		p.z = f.ctrlPoint.x * f.nextPoint.y - f.ctrlPoint.y * f.nextPoint.x;
//
//		GLfloat d = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
//		if (d == 0.0) {
//			return;
//		}
//		p.x /= d;
//		p.y /= d;
//		p.z /= d;

		gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, f.nextCtrlPoint.x, f.nextCtrlPoint.y, f.nextCtrlPoint.z, 0, 1, 0);
	}

}
