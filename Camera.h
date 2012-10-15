/*
 * Camera.h
 *
 *  Created on: Oct 6, 2012
 *      Author: stevananet
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "ControlPoint.h"
#include "BSpline.h"

struct RefFrame{
	ControlPoint T;
	ControlPoint N;
	ControlPoint B;
};

class Camera {

private:
	RefFrame* ref;
	int count;

public:
	float zoom, rotx, roty, tx, ty;
	Camera();
	virtual ~Camera();

	void setInitialCameraPosition(double winWidth, double winHeight);
	void rotateCamera();
	void resetCamera(float* zoom, float* tx, float* ty, float* rotx, float* roty);

	void lookAt(BSpline*, double, double);
};

#endif /* CAMERA_H_ */
