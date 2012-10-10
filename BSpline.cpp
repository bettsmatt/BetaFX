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
#include "Point.h"

BSpline::BSpline() {
	n = 2; // Degree of the curve
	controlPointsNum = 0;
	pointSelected = -1;
	nV = NULL;
	numSplinePieces = 0;
	currentFrame = 0;
	hasChanged = true;
}

BSpline::BSpline(Point* points, int num){
	n = 2;
	pointSelected = -1;
	controlPoints = points;
	controlPointsNum = num;
	numSplinePieces = 0;
	currentFrame = 0;
	hasChanged = true;
	nV = NULL;
	assignColourId();
	createNodeVector();
}

BSpline::~BSpline() {
	delete[] nV;
}

void BSpline::setControlPoints(Point* points, int n){
	controlPoints = points;
	controlPointsNum = n;
	assignColourId();
}

int BSpline::getPointSelected(){
	return pointSelected;
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

void BSpline::createNodeVector(){
	if (nV != NULL)
		delete[] nV;

	nV = new int[controlPointsNum + 5];
	int knoten = 0;
	for (int i = 0; i < (n + controlPointsNum + 1); i++){ // n+m+1 = nr of nodes
		if (i > n){
			if (i <= controlPointsNum){
				nV[i] = ++knoten;
			}
			else{
				nV[i] = knoten;
			}
		}
		else {
			nV[i] = knoten;
		}
	}
}

// Recursive deBoor algorithm.
Point BSpline::deBoor(int r, int i, float u) {
	if (r == 0) {
		return controlPoints[i];
	} else {
		float pre = (u - nV[i + r]) / (nV[i + n + 1] - nV[i + r]); // Precalculation

		Point p1 = deBoor(r - 1, i, u);
		p1.multiplyScalar(1-pre);

		Point p2 = deBoor(r - 1, i + 1, u);
		p2.multiplyScalar(pre);

		p1.addPoint(p2);

		return p1;
	}
}

void BSpline::drawSpline(){
	drawControlPoints(GL_RENDER);
	drawCurve();
}

void BSpline::drawCurve(){
	if(controlPointsNum == 0) return;

	numSplinePieces = 0;
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);

	Point vertex = Point();

	for(float i = 0.0f; i < nV[n + controlPointsNum]; i += 0.1f){
		for(int j = 0; j < controlPointsNum; j++){
            if(i >= j){
                vertex = deBoor(n, j, i);
            }
        }
		// specify the point
		glVertex3f(vertex.x, vertex.y, vertex.z);
		numSplinePieces++;
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

void BSpline::drawReferenceSystem(int winWidth, int winHeight) {
	if (controlPointsNum == 0)
		return;
	GLUquadric* q = gluNewQuadric();
	if (q == 0) {
		printf("Not enough memory to allocate space to draw\n");
		exit(EXIT_FAILURE);
	}

	glPushMatrix();
	glTranslatef(-10, -10, 0);

		// Z-axis
		glPushMatrix();
			glColor3f(0, 0, 1);
			gluCylinder(q, 0.2, 0.2, 1.5, 10, 10);
			glTranslatef(0, 0, 1.5);
			glutSolidCone(0.5, 0.5, 10, 10);
		glPopMatrix();

		// Y-axis
		glPushMatrix();
			glColor3f(0, 1, 0);
			glRotatef(-90, 1, 0, 0);
			gluCylinder(q, 0.2, 0.2, 1.5, 10, 10);
			glTranslatef(0, 0, 1.5);
			glutSolidCone(0.5, 0.5, 10, 10);
		glPopMatrix();

		// X-axis
		glPushMatrix();
			glColor3f(1, 0, 0);
			glRotatef(90, 0, 1, 0);
			gluCylinder(q, 0.2, 0.2, 1.5, 10, 10);
			glTranslatef(0, 0, 1.5);
			glutSolidCone(0.5, 0.5, 10, 10);
		glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(q);
}

void BSpline::computeFrames(){
	if (frames != NULL)
			delete[] frames;

	frames = new Point[numSplinePieces];
	Point point = Point();
	int k = 0;
	for(float i = 0.0f; i < nV[n + controlPointsNum]; i += 0.1f){
		for(int j = 0; j < controlPointsNum; j++){
	        if(i >= j){
	        	point = deBoor(n, j, i);
	        }
	    }
		frames[k++] = point;
	}
}

Point BSpline::nextFrame(){
	Point p = frames[currentFrame];
	currentFrame++;
	if(currentFrame >= numSplinePieces){
		currentFrame = 0;
	}
	return p;
}

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
	for (int i = 0; i < controlPointsNum; i++) {
		if(controlPoints[i].r == pixel[0] && controlPoints[i].g == pixel[1] && controlPoints[i].b == pixel[2]){
			pointSelected = i;
			break;
		}
	}
	glutPostRedisplay();
}

void BSpline::printArray(float* a, int size){
	for(int i = 0; i < size; i++){
		printf("%f ", a[i]);
	}
	printf("\n");
}
