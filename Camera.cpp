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

void Camera::lookAt(BSpline* bs, double winWidth, double winHeight){
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, winWidth / winHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	Frame f = bs->nextFrame();
	float time = bs->nextTime();
	if(time == 0.0f){
		count = 0;
		// calculate initial reference frame
		ref->T = f.tangent;
		ref->T.normalize();
		ref->N = crossProduct(ControlPoint(0,1,0), ref->T);
		ref->N.normalize();
		ref->B = crossProduct(ref->T, ref->N);
		ref->B.normalize();
	}
	else if(f.isLast != 1){
		ref->T = bs->computeTangent(time);
		ref->T.normalize();
		ref->N = crossProduct(ref->N, ref->T);
		ref->N.normalize();
		ref->B = crossProduct(ref->T, ref->N);
		ref->B.normalize();
	}


	if (f.isLast == 1) {
		gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, 0, 0, 0, 0, 1, 0);
	}
	else{
		//printf("pos: %f %f %f\n", f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z);
		//printf("tan: %f %f %f\n", f.tangent.x, f.tangent.y, f.tangent.z);
		ref->N = ref->N + f.ctrlPoint;
		ref->B = ref->B + f.ctrlPoint;

		//printf("%i\nt: %f %f %f\tB: %f %f %f\n", count++, ref->T.x, ref->T.y, ref->T.z, ref->B.x, ref->B.y,ref->B.z);
		gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, ref->T.x, ref->T.y, ref->T.z, ref->B.x, ref->B.y,ref->B.z);
		//gluLookAt(f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z, 0,0,0, ref->B.x, ref->B.y,ref->B.z);
	}

}
