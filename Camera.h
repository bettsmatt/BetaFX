/*
 * Camera.h
 *
 *  Created on: Oct 6, 2012
 *      Author: stevananet
 */

#ifndef CAMERA_H_
#define CAMERA_H_

class Camera {
public:
	float zoom, rotx, roty, tx, ty;

	Camera();
	virtual ~Camera();

	void SetInitialCameraPosition(double winWidth, double winHeight);
	void RotateCamera();
	void ResetCamera(float* zoom, float* tx, float* ty, float* rotx, float* roty);
};

#endif /* CAMERA_H_ */
