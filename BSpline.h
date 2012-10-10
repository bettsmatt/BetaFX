/*
 * BSpline.h
 *
 *  Created on: Oct 4, 2012
 *      Author: stevananet
 */

#ifndef BSPLINE_H_
#define BSPLINE_H_

#include "Point.h"
#include <GL/glut.h>

class BSpline {

private:
	int n; // Degree of the curve

    Point* controlPoints; // The control points.
    Point* frames;
    int controlPointsNum;
    int* nV; // Node vector
    int numSplinePieces;
    int pointSelected;
    int currentFrame;

    void drawCurve();
   	void drawControlPoints(GLenum mode);

public:
    bool hasChanged;

	BSpline();
	BSpline(Point* points, int num);
	virtual ~BSpline();

	void setControlPoints(Point* points, int num);
	int getPointSelected();

	void printArray(float* a, int size);

	void createNodeVector();
	Point deBoor(int r, int i, float u);
	void drawSpline();

	void drawReferenceSystem(int w, int h);
	void selectPoint(int x, int y);
	void deselectPoint();
	void assignColourId();

	void computeFrames();
	Point nextFrame();
};

#endif /* BSPLINE_H_ */
