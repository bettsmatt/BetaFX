/*
 * Point.h
 *
 *  Created on: Oct 8, 2012
 *      Author: stevananet
 */

#ifndef POINT_H_
#define POINT_H_

class Point {

private:


public:
	float x,y,z;
	float r,g,b;

	Point();
	Point(float x, float y, float z);
	virtual ~Point();

	void setColourID(float rr, float gg, float bb);
	void multiplyScalar(float);
	void addPoint(Point p);
	void draw(bool pointSelected);
	void unprojectScreenCoord(int x, int y);
};

#endif /* POINT_H_ */
