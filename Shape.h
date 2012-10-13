/*
 * Shape.h
 *
 *  Created on: Oct 11, 2012
 *      Author: stevananet
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "ControlPoint.h"
#include "BSpline.h"

class Shape {

private:
	ControlPoint position;
	float angle;
	ControlPoint rotAxis;

public:
	Shape();
	virtual ~Shape();

	void move(Frame);
	void draw();
};

#endif /* SHAPE_H_ */
