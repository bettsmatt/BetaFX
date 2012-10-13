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
	ControlPoint nextCtrlPoint;
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

    float deltaTime;

    float startTime;
	float endTime;

	int count;
	float* times;
	ControlPoint* positions;

    void drawCurve();
   	void drawControlPoints(GLenum mode);

public:
    bool hasChanged;
    bool infoDisplay;

	BSpline();
	BSpline(ControlPoint* points, int num);
	virtual ~BSpline();

	int getControlPointsNum();

	void init();

	void printArray(float* a, int size);

	void assignColourId();

	void draw();

	void selectPoint(int x, int y);
	void deselectPoint();

	void moveSelectedPoint(float f, char c);
	void addPoint(float x, float y, float z);

	void computeFrames();
	void computeKnots();
	void computeTimes();
	Frame nextFrame();

	void recalculate();

	ControlPoint evaluate(float t);
	void promptForNewTimes();
	void readNewTimes();
	void promptForNewInterval();
	void readNewInterval();
};

#endif /* BSPLINE_H_ */
