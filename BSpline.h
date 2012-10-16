/*
 * BSpline.h
 *
 *  Created on: Oct 4, 2012
 *      Author: stevananet
 */

#ifndef BSPLINE_H_
#define BSPLINE_H_

#include "ControlPoint.h"
#include <GL/glut.h>


struct Frame {
	ControlPoint ctrlPoint;
	ControlPoint tangent;
	int isLast;
};

class BSpline {

private:
	int n; // Degree of the curve

	ControlPoint* controlPoints; // The control points.
    Frame* frames;
    int controlPointsNum;
    int numSplinePieces;
    int pointSelected;
    int currentFrame;



    float startTime;
	float endTime;

	int count;
	float* times;
	ControlPoint* positions;

    void drawCurve();
   	void drawControlPoints(GLenum mode);
   	void computeFrames();
   	void computeKnots();
   	void computeTimes();


public:
    bool hasChanged;
    bool pointAdded;
    bool infoDisplay;
    float deltaTime;

	BSpline();
	BSpline(ControlPoint* points, int num);
	virtual ~BSpline();

	void init();
	void printArray(float* a, int size);
	void assignColourId();

	void draw();

	void selectPoint(int x, int y);
	void deselectPoint();

	void moveSelectedPoint(float f, char c);
	void addPoint(float x, float y, float z);

	Frame nextFrame();
	void resetFrame();
	float nextTime();

	void recalculate();

	ControlPoint evaluate(float t);
	void promptForNewTimes();
	void readNewTimes();
	void promptForNewInterval();
	void readNewInterval();
	void printCoordinates();
	ControlPoint computeTangent(float t);
};

#endif /* BSPLINE_H_ */
