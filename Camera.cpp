/*
 * Camera.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: stevananet
 */

#include "Camera.h"
#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

Camera::Camera() {
	zoom, rotx, roty, tx, ty = 0.0f;

}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

void Camera::SetInitialCameraPosition(double winWidth, double winHeight) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, winWidth / winHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
				30, 30, 30,
				0, 0, 0,
				0, 1, 0
		);
	//gluLookAt(0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void Camera::RotateCamera(){
	glTranslatef(tx, ty, -zoom);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
}

//--------------------------------------------------------------------------------
//
void Camera::ResetCamera(float* zoom, float* tx, float* ty, float* rotx, float* roty){
	*zoom = 0;
	*tx = 0;
	*ty = 0;
	*rotx = 0;
	*roty = 0;
}
