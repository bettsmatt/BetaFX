/*
 * Shape.h
 *
 *  Created on: Oct 8, 2012
 *      Author: stevananet
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "Point.h"

class Shape {
public:
	Shape();
	virtual ~Shape();

	void drawAt(Point);
};

#endif /* SHAPE_H_ */
