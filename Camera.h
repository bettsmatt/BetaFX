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

class Camera {
public:
	float zoom, rotx, roty, tx, ty;

	Camera();
	virtual ~Camera();

	void setInitialCameraPosition(double winWidth, double winHeight);
	void rotateCamera();
	void resetCamera(float* zoom, float* tx, float* ty, float* rotx, float* roty);

	void lookAt(Frame, double, double);
};

#endif /* CAMERA_H_ */
