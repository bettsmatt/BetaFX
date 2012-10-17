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
	zoom = 0.0f;
	rotx = 0.0f;
	roty = 0.0f;
	tx = 0.0f;
	ty = 0.0f;
	ref = new RefFrame();
	count = 0;
}

Camera::~Camera() {
	delete ref;
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

void Camera::rotateCamera(G308_Point* pos){
	glTranslatef(tx, ty, -zoom);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	pos->x += tx;
	pos->y += ty;
	pos->z += -zoom;
}

//--------------------------------------------------------------------------------
//
void Camera::resetCamera(G308_Point* pos){
	zoom = 0;
	tx = 0;
	ty = 0;
	rotx = 0;
	roty = 0;
	pos->x = 0;
	pos->y = 0;
	pos->z = 50;
}

void Camera::lookAt(BSpline* bs, int m, double winWidth, double winHeight, G308_Point* pos){
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, winWidth / winHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	Frame f = bs->nextFrame();
	float time = bs->nextTime();
	//computeReferenceFrame(f, time, bs->computeTangent(time));
	computeReferenceFrame(f, time, f.tangent);

	pos->x = f.ctrlPoint.x;
	pos->y = f.ctrlPoint.y;
	pos->z = f.ctrlPoint.z;

	ref->N = ref->N + f.ctrlPoint;
	ref->B = ref->B + f.ctrlPoint;

	if (f.isLast == 1) {
		if(m == 1) gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, 0, 0, 0, ref->B.x, ref->B.y,ref->B.z);
		else if(m == 2) gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, ref->T.x, ref->T.y, ref->T.z, 0,1,0);
	}
	else{
		if(m == 1) gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, 0, 0, 0, ref->B.x, ref->B.y,ref->B.z);
		else if(m == 2) gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, ref->T.x, ref->T.y, ref->T.z, 0,1,0);
	}
}

void Camera::computeReferenceFrame(Frame f, float time, ControlPoint nextTangent) {
	if (time == 0.0f) {
		count = 0;
		// calculate initial reference frame
		ref->T = f.tangent;
		ref->T.normalize();
		ref->N = crossProduct(ControlPoint(0, 1, 0), ref->T);
		ref->N.normalize();
		ref->B = crossProduct(ref->T, ref->N);
		ref->B.normalize();
	} else if (f.isLast != 1) {
		ref->T = nextTangent;
		ref->T.normalize();
		ref->N = crossProduct(ref->N, ref->T);
		ref->N.normalize();
		ref->B = crossProduct(ref->T, ref->N);
		ref->B.normalize();
	}
}
