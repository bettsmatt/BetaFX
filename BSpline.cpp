/*
 * BSpline.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: stevananet
 */

#include "BSpline.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "ControlPoint.h"

#define MAX_POINTS 20

BSpline::BSpline() {
	controlPointsNum = 5;
	controlPoints = new ControlPoint[MAX_POINTS];
	controlPoints[0] = ControlPoint(15, -0.5, 0, 1.0f);
	controlPoints[1] = ControlPoint(5, -2, 14, 2.0f);
	controlPoints[2] = ControlPoint(-5, 0, 0, 3.0f);
	controlPoints[3] = ControlPoint(-10, 5, -2, 2.0f);
	controlPoints[4] = ControlPoint(-15, 5, -2, 1.0f);

}

BSpline::BSpline(ControlPoint* points, int num){
	controlPoints = points;
	controlPointsNum = num;

}

BSpline::~BSpline() {
	//if(nV != NULL) delete[] nV;
	if(controlPoints != NULL) delete[] controlPoints;
	if(positions != NULL) delete[] positions;
	if(times != NULL) delete[] times;
	if(frames != NULL) delete[] frames;
}

void BSpline::init(){
	n = 3; // Degree of the curve
	pointSelected = -1;
	frames = NULL;
	deltaTime = 0.01f;
	numSplinePieces = 0;
	currentFrame = 0;
	hasChanged = false;
	infoDisplay = false;
	pointAdded = false;
	positions = NULL;
	times = NULL;
	startTime = 0.0f;
	endTime = 1.0f;
	assignColourId();
	computeKnots();
	computeTimes();
	computeFrames();
}

// This part was done with the help from http://content.gpwiki.org/index.php/OpenGL_Selection_Using_Unique_Color_IDs
void BSpline::assignColourId() {
	if (controlPointsNum == 0)
		return;

	float nextR = 255.0f;
	float nextG = 0.0f;
	float nextB = 0.0f;

	for (int i = 0; i < controlPointsNum; i++) {
		controlPoints[i].setColourID(nextR, nextG, nextB);
		nextG += 20;
		if (nextG > 255) {
			nextG = 0;
			nextB += 20;
		}
	}
}

//****************************************************************************
// DRAWING
//****************************************************************************
void BSpline::draw(){
	drawControlPoints(GL_RENDER);
	drawCurve();
}

void BSpline::drawCurve(){
	if(controlPointsNum == 0) return;

	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	ControlPoint start = evaluate(0.0f);
	ControlPoint end = ControlPoint();
	for (float t = startTime; t < endTime; t += 0.01f) {
		end = evaluate(t);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();
}

void BSpline::drawControlPoints(GLenum mode) {
	if(controlPointsNum == 0) return;
	GLUquadric* q = gluNewQuadric();
	if (q == 0) {
		printf("Not enough memory to allocate space to draw\n");
		exit(EXIT_FAILURE);
	}
	// draw the control points
	for (int i = 0; i < controlPointsNum; i++) {
		controlPoints[i].draw(pointSelected == i);
		if(infoDisplay) controlPoints[i].showTime();
	}

	// draw the hull of the curve
	glColor3f(0,1,1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < controlPointsNum; i++) {
		glVertex3f(controlPoints[i].x,controlPoints[i].y,controlPoints[i].z);
	}
	glEnd();
	gluDeleteQuadric(q);

}

//****************************************************************************
// CALCULATING SPLINE COMPONENTS
//****************************************************************************
void BSpline::recalculate(){
	computeKnots();
	if(pointAdded){
		pointAdded = false;
		computeTimes();
	}
	computeFrames();
}

void BSpline::computeKnots(){
	if (positions != NULL)
		delete[] positions;

	positions = new ControlPoint[controlPointsNum + n + 1];
	count = controlPointsNum + n + 1;

	// Copy positions data (triplicate start and en points so that curve passes trough them.)
	positions[0] = positions[1] = controlPoints[0];

	for (int i = 0; i < controlPointsNum; ++i) {
		positions[i + 2] = controlPoints[i];
	}

	positions[count - 1] = positions[count - 2] = controlPoints[controlPointsNum - 1];
}

void BSpline::computeTimes(){
	if (times != NULL)
			delete[] times;

	times = new float[controlPointsNum + 2];
	// Setup times (subdivide interval to get arrival times at each knot location)
	float dt = (endTime - startTime) / (float) (controlPointsNum + 1);

	// Initially, make the speed constant along the spline
	times[0] = startTime;
	for (int i = 0; i < controlPointsNum; ++i) {
		times[i + 1] = times[i] + dt;
		controlPoints[i].time = times[i + 1];
	}

	times[controlPointsNum + 1] = endTime;
}

void BSpline::computeFrames(){
	if (frames != NULL)
		delete[] frames;

	numSplinePieces = 0;
	for (float t = startTime; t < endTime; t += deltaTime) {
		numSplinePieces++;
	}
	frames = new Frame[numSplinePieces];

	int k = 0;
	Frame* f = (Frame*)malloc(sizeof(Frame));
	for (float t = startTime; t <= endTime; t += deltaTime) {
		f->ctrlPoint = evaluate(t);
		f->tangent = computeTangent(t);
		f->tangent.normalize();
		frames[k++] = *f;
	}
	free(f);
}

Frame BSpline::nextFrame(){
	Frame p = frames[currentFrame];
	currentFrame++;
	if(currentFrame >= numSplinePieces){
		currentFrame = 0;
	}
	return p;
}

void BSpline::resetFrame(){
	currentFrame = 0;
}

float BSpline::nextTime(){
	return (currentFrame - 1) * deltaTime;
}

//****************************************************************************
// USER INTERACTION THROUGH COMMAND LINE
//****************************************************************************
void BSpline::promptForNewTimes(){
	printf("\nCurrent times are:\n");
	for (int i = 0; i < controlPointsNum; i++) {
		printf("%.2f ", controlPoints[i].time);
	}
	printf("\nEnter new time values separated by space.\nValues should be between 0 and 1.\n ");
}

void BSpline::readNewTimes(){
	float number = 0;
	for (int i = 0; i < controlPointsNum; i++) {
		if(scanf("%f", &number) == 1){
			times[i+1] = number * endTime;
			controlPoints[i].time = times[i + 1];
		}
	}
	printf("\nTimes after change:\n");
	for (int i = 0; i < controlPointsNum; i++) {
		printf("%.2f ", controlPoints[i].time);
	}
	printf("\n");
	hasChanged = true;
}

void BSpline::promptForNewInterval(){
	printf("\nCurrent time interval from start of the spline to the end is %.2f\n", (endTime - startTime));
	printf("\nEnter new time interval: \n ");
}
void BSpline::readNewInterval(){
	float number = 0;
	scanf("%f", &number);

	for (int i = 0; i < controlPointsNum; i++) {
		times[i+1] = times[i+1] * (number / endTime);
		controlPoints[i].time = times[i + 1];
	}
	endTime = number;
	times[controlPointsNum + 1] = endTime;
	printf("\nTime interval after change is %.2f\n", endTime);
	hasChanged = true;
}

void BSpline::printCoordinates(){
	printf("Coordinates:\n");
	for (int i = 0; i < controlPointsNum; i++){
		printf("%i %.2f %.2f %.2f\n", i, controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
	}
}

//****************************************************************************
// SELECTING AND MOVING CONTROL POINTS
//****************************************************************************
void BSpline::deselectPoint(){
	pointSelected = -1;
}

void BSpline::selectPoint(int x, int y){
	glDisable(GL_TEXTURE_2D); // turn off texturing, lighting and fog
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);

	drawControlPoints(GL_SELECT);

	GLint viewport[4];
	unsigned char pixel[3];
	glGetIntegerv(GL_VIEWPORT, viewport); // get color information from frame buffer

	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	printf("pixel: %i %i %i\n", pixel[0], pixel[1], pixel[2]);
	for (int i = 0; i < controlPointsNum; i++) {
		if((controlPoints[i].r == pixel[0]) && (controlPoints[i].g == pixel[1]) && (controlPoints[i].b) == pixel[2]){
			pointSelected = i;
			break;
		}
	}
	glutPostRedisplay();
}

void BSpline::moveSelectedPoint(float f, char c){
	switch(c){
	case 'x':
		controlPoints[pointSelected].x += f;
		// Set a flag to recompute the spline and frames
		hasChanged = true;
		break;
	case 'y':
		controlPoints[pointSelected].y += f;
		// Set a flag to recompute the spline and frames
		hasChanged = true;
		break;
	case 'z':
		controlPoints[pointSelected].z += f;
		// Set a flag to recompute the spline and frames
		hasChanged = true;
		break;
	}
}

void BSpline::addPoint(float x, float y, float z) {
	ControlPoint* newp = new ControlPoint(x, y, z);
	controlPointsNum++;

	if(controlPointsNum > MAX_POINTS){
		printf("Maximum number of control points reached.\nCannot add a new point.\n");
		return;
	}
	// Assign a colour ID to the new point`
	float lastR, lastG, lastB;
	lastR = controlPoints[controlPointsNum - 2].r;
	lastG = controlPoints[controlPointsNum - 2].g;
	lastB = controlPoints[controlPointsNum - 2].b;
	lastG += 20;
	if (lastG > 255) {
		lastG = 0;
		lastB += 20;
	}
	controlPoints[controlPointsNum - 1] = *newp;
	controlPoints[controlPointsNum - 1].setColourID(lastR, lastG, lastB);

	// Set a flag to recompute the frames
	hasChanged = true;
	pointAdded = true;
	delete newp;
}


ControlPoint BSpline::evaluate(float t) {
		if( count < 6 )
			return ControlPoint();

		// Handle boundry conditions
		if( t <= times[0] )
		{
			return positions[0];
		}
		else if ( t >= times[count - 3] )
		{
			return positions[count - 3];
		}

		// Find segment and parameter
		int segment = 0;
		while(segment < count - 3) {
			if( t <= times[segment + 1] ) break;
			segment++;
		}

		float t0 = times[segment];
		float t1 = times[segment + 1];
		float u = (t - t0) / (t1 - t0);

		// match segment index to standard B-spline terminology
		segment += 3;

		// Evaluate
		ControlPoint A = positions[segment] - (positions[segment - 1] * 3.0f) + (positions[segment - 2] * 3.0f) - positions[segment - 3];
		ControlPoint B = (positions[segment - 1] * 3.0f) - (positions[segment - 2] * 6.0f) + (positions[segment - 3] * 3.0f);
		ControlPoint C = (positions[segment - 1] * 3.0f) - (positions[segment - 3] * 3.0f);
		ControlPoint D = positions[segment - 1] + (positions[segment - 2] * 4.0f) + positions[segment - 3];

		return (D + (C + (B + A * u) * u) * u) / 6.0f;
}

ControlPoint BSpline::computeTangent(float t) {
	if (count < 6)
		return ControlPoint();

	// Handle boundry conditions
	if (t <= times[0]) {
		return positions[0];
	} else if (t >= times[count - 3]) {
		return positions[count - 3];
	}

	// Find segment and parameter
	int segment = 0;
	while (segment < count - 3) {
		if (t <= times[segment + 1])
			break;
		segment++;
	}

	float t0 = times[segment];
	float t1 = times[segment + 1];
	float u = (t - t0) / (t1 - t0);

	// match segment index to standard B-spline terminology
	segment += 3;

	// Evaluate
	ControlPoint A = positions[segment] - (positions[segment - 1] * 3.0f) + (positions[segment - 2] * 3.0f) - positions[segment - 3];
	ControlPoint B = (positions[segment - 1] * 3.0f) - (positions[segment - 2] * 6.0f) + (positions[segment - 3] * 3.0f);
	ControlPoint C = (positions[segment - 1] * 3.0f) - (positions[segment - 3] * 3.0f);
	ControlPoint D = positions[segment - 1] + (positions[segment - 2] * 4.0f) + positions[segment - 3];

	return (C + (B * 2 + (A * 3) * u) * u) / 6.0f;
}

