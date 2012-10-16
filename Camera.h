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
#include "define.h"

struct RefFrame{
	ControlPoint T;
	ControlPoint N;
	ControlPoint B;
};

class Camera {

private:
	RefFrame* ref;
	int count;

	void computeReferenceFrame(Frame, float, ControlPoint);

public:
	float zoom, rotx, roty, tx, ty;
	float posX, posY, posZ;
	Camera();
	virtual ~Camera();

	void setInitialCameraPosition(double winWidth, double winHeight);
	void rotateCamera(G308_Point*);
	void resetCamera(G308_Point* pos);

	void lookAt(BSpline*, int, double, double, G308_Point* pos);
};

#endif /* CAMERA_H_ */
