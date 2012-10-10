/*
 * Point.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: stevananet
 */

#include "Point.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

Point::Point() {
	x = y = z = 0.0f;
}

Point::Point(float xx, float yy, float zz){
	x = xx;
	y = yy;
	z = zz;
}

Point::~Point() {
	// TODO Auto-generated destructor stub
}

void Point::setColourID(float rr, float gg, float bb){
	r = rr;
	g = gg;
	b = bb;
}


void Point::multiplyScalar(float f){
	x *= f;
	y *= f;
	z *= f;
}

void Point::addPoint(Point p){
	x += p.x;
	y += p.y;
	z += p.z;
}

void Point::draw(bool pointSelected){
	glPushMatrix();
		glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
		if (pointSelected) {
			glColor3f(0, 0, 1);
		}
		glTranslatef(x, y, z);
		glutSolidSphere(0.25, 10, 10);
	glPopMatrix();
}

// Taken from http://nehe.gamedev.net/article/using_gluunproject/16013/
void Point::unprojectScreenCoord(int sx, int sy){
	GLint viewport[4];                  // Where The Viewport Values Will Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);           // Retrieves The Viewport Values (X, Y, Width, Height)

	GLdouble modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix

//	for(int i = 0; i < 16; i++){
//		printf("%f ", modelview[i]);
//		if((i + 1)%4 == 0){
//			printf("\n");
//		}
//	}


	GLdouble projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
	glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix

	GLfloat winX, winY, winZ;               // Holds Our X, Y and Z Coordinates
	GLdouble posX, posY, posZ;

	winX = (float)sx;                  // Holds The Mouse X Coordinate
	winY = (float)sy;                  // Holds The Mouse Y Coordinate

	winY = (float)viewport[3] - winY;           // Subtract The Current Mouse Y Coordinate From The Screen Height.
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	printf("winZ = %f\n", winZ);
	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	x = posX;
	y = posY;
	z = posZ;
	printf("posX = %f, posY = %f, posZ = %f\n", x, y, z);
}
// This function will find 2 points in world space that are on the line into the screen defined by screen-space( ie. window-space ) point (x,y)

   /*gluUnProject ((double) x, dClickY, 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
   ClickRayP1 = Vector3 ( (float) dX, (float) dY, (float) dZ );
   gluUnProject ((double) x, dClickY, 1.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
   ClickRayP2 = Vector3 ( (float) dX, (float) dY, (float) dZ );*/
