/*
 * ControlPoint.h
 *
 *  Created on: Oct 13, 2012
 *      Author: stevananet
 */

#ifndef CONTROLPOINT_H_
#define CONTROLPOINT_H_

class ControlPoint {
public:
	float x,y,z;
	float r,g,b;
	float time;

	ControlPoint();
	ControlPoint(float x, float y, float z);
	ControlPoint(float x, float y, float z, float t);
	virtual ~ControlPoint();

	void setColourID(float rr, float gg, float bb);
	void multiplyScalar(float);
	void add(ControlPoint p);
	void draw(bool pointSelected);
	void showTime();
	void unprojectScreenCoord(int x, int y);
	void normalize();


friend ControlPoint operator+(const ControlPoint&, const ControlPoint&);
friend ControlPoint operator-(const ControlPoint&, const ControlPoint&);
friend ControlPoint operator*(const ControlPoint&, const float&);
friend ControlPoint operator/(const ControlPoint&, const float&);

};

float dotProduct(ControlPoint,ControlPoint);
ControlPoint crossProduct(ControlPoint,ControlPoint);

#endif /* CONTROLPOINT_H_ */
