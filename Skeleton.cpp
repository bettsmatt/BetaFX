//---------------------------------------------------------------------------
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
// Copyright (c) 2012 by Taehyun Rhee
//
// Edited by Roma Klapaukh, Daniel Atkins, and Taehyun Rhee
//
//---------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "Skeleton.h"
#include "define.h"
#include <math.h>
#include "BSpline.h"

#define NUM_FRAMES 600;
#define NUM_BONES_WITH_DOF 29


Skeleton::Skeleton() {
	numBones = 1;
	motionframe = 1;
	currentFrameNumber = 0;
	buffSize = 200;
	maxBones = 60;
	angle = 0;
	rotAxis = ControlPoint();
	root = (bone*) malloc(sizeof(bone) * maxBones);

	for (int i = 0; i < 60; i++) {
		root[i].numChildren = 0;
		root[i].dirx = 0;
		root[i].diry = 0;
		root[i].dirz = 0;
		root[i].rotx = 0;
		root[i].roty = 0;
		root[i].rotz = 0;
		root[i].dof = DOF_NONE;
		root[i].length = 0;
		root[i].name = NULL;
		root[i].children = (bone**) malloc(sizeof(bone*) * 5);

		//Challenge stuff
		root[i].currentTranslatex = 0;
		root[i].currentTranslatey = 0;
		root[i].currentTranslatez = 0;
		root[i].currentRotationx = 0;
		root[i].currentRotationy = 0;
		root[i].currentRotationz = 0;

	}
	char*name = (char*) malloc(sizeof(char) * 5);
	name[0] = 'r';
	name[1] = name[2] = 'o';
	name[3] = 't';
	name[4] = '\0';
	root[0].name = name;
	root[0].dof = DOF_ROOT;
}

Skeleton::~Skeleton() {
	deleteBones(root);
}

void Skeleton::deleteBones(bone* root) {
	for (int i = 0; i < maxBones; i++) {
		if (root[i].name != NULL) {
			free(root[i].name);
		}
		if (root[i].children != NULL) {
			free(root[i].children);
		}
	}
	free(root);
}

// [Assignment2] you may need to revise this function
void Skeleton::display() {
	if (root == NULL) {
		return;
	}
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle, rotAxis.x, rotAxis.y, rotAxis.z);

	GLUquadric* quad = gluNewQuadric(); //Create a new quadric to allow you to draw cylinders
	if (quad == 0) {
		printf("Not enough memory to allocate space to draw\n");
		exit(EXIT_FAILURE);
	}
	//Actually draw the skeleton
	display(root, quad);

	gluDeleteQuadric(quad);
	glPopMatrix();
}

// [Assignment2] you need to fill this function
void Skeleton::display(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	drawParts(root, q);

}

void Skeleton::drawParts(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	glPushMatrix();
	drawOnePart(root, q);
	for(int i = 0; i < root->numChildren; i++){
		drawParts(root->children[i], q);
	}
	glPopMatrix();


}

void Skeleton::move(BSpline* bs){
	printf("Move\n");
	Frame f = bs->nextFrame();
	printf("f: %f %f %f\n", f.ctrlPoint.x, f.ctrlPoint.y, f.ctrlPoint.z);
	position = f.ctrlPoint;
	printf("position: %f %f %f\n", position.x, position.y, position.z);
	ControlPoint zVector = ControlPoint(0, 0, 1);
	//f.tangent.normalize();

	rotAxis = crossProduct(zVector, f.tangent);
	angle = dotProduct(zVector, f.tangent);
	angle = acos(angle);
	angle = angle * (180.0 / 3.1416);
	printf("a: %f\t axis: %f %f %f\n", angle, rotAxis.x, rotAxis.y, rotAxis.z);
}

void Skeleton::doAMCrotation(bone* bone){

	bonerotation b = bone->frames[motionframe];
	if(bone->dof == 7){
		glRotatef(b.rz, 0.0, 0.0, 1.0);
		glRotatef(b.ry, 0.0, 1.0, 0.0);
		glRotatef(b.rx, 1.0, 0.0, 0.0);
//		printf("%s: %f %f %f\n", bone->name, b.rx, b.ry, b.rz);
	}
	// DOF: 6, rx = 0, ry = 2, rz = 4
	else if(bone->dof == 6){
		glRotatef(b.rz, 0.0, 0.0, 1.0);
		glRotatef(b.ry, 0.0, 1.0, 0.0);
//		printf("%s: %f %f\n", bone->name, b.ry, b.rz);
	}
	// DOF: 4, rx = 0, ry = 0, rz = 4
	else if(bone->dof == 4){
		glRotatef(b.rz, 0.0, 0.0, 1.0);
//		printf("%s: %f\n", bone->name, b.rz);
	}
	// DOF: 5, rx = 1, ry = 0, rz = 4
	else if(bone->dof == 5){
		glRotatef(b.rz, 0.0, 0.0, 1.0);
		glRotatef(b.rx, 1.0, 0.0, 0.0);
//		printf("%s: %f %f\n", bone->name, b.rx, b.rz);
	}
	// DOF: 2, rx = 0, ry = 2, rz = 0
	else if(bone->dof == 2){
		glRotatef(b.ry, 0.0, 1.0, 0.0);
//		printf("%s: %f\n", bone->name, b.ry);
	}
	// DOF: 1, rx = 1, ry = 0, rz = 0
	else if(bone->dof == 1){
		glRotatef(b.rx, 1.0, 0.0, 0.0);
//		printf("%s: %f\n", bone->name, b.rx);
	}
	else if(bone->dof == 8){
		glTranslatef(b.tx, b.ty,b.tz);
		glRotatef(b.rz, 0.0, 0.0, 1.0);
		glRotatef(b.ry, 0.0, 1.0, 0.0);
		glRotatef(b.rx, 1.0, 0.0, 0.0);
//		printf("%s: %f %f %f\n", bone->name, b.rx, b.ry, b.rz);
	}

}

void Skeleton::drawOnePart(bone* root, GLUquadric* q) {
	if (root == NULL) {
		return;
	}

	if (root == NULL) {
			return;
		}
		glPushMatrix();

			// Rotate local coordinate system
			glRotatef(root->rotz, 0, 0, 1);
			glRotatef(root->roty, 0, 1, 0);
			glRotatef(root->rotx, 1, 0, 0);

			glColor3f(0, 1, 1);
			glutSolidSphere(0.1, 3, 3);
		glPopMatrix();


		glRotatef(root->rotz, 0, 0, 1);
		glRotatef(root->roty, 0, 1, 0);
		glRotatef(root->rotx, 1, 0, 0);


		glRotatef(-root->rotx, 1, 0, 0);
		glRotatef(-root->roty, 0, 1, 0);
		glRotatef(-root->rotz, 0, 0, 1);


		GLfloat a = 0.0;
		glPushMatrix();
			// Find rotation vector normal.
			G308_Point v1 = { 0, 0, 1 };
			G308_Point v2 = { root->dirx, root->diry, root->dirz };
			G308_Point normal = { 0, 0, 0 };
			calculateCrossProduct(v1, v2, &normal);
			// Find angle of rotation from bone's direction (in degrees).
			a = calculateDotProduct(v1, v2);
			glColor3f(1, 1, 1);
			glRotatef(a, normal.x, normal.y, normal.z);
			gluCylinder(q, 0.05, 0.05, root->length, 3, 3);
		glPopMatrix();


		glTranslatef(root->length * root->dirx, root->length * root->diry,
						root->length * root->dirz);
}

void Skeleton::calculateCrossProduct(G308_Point v1, G308_Point v2, G308_Point* normal){
	normal->x = v1.y * v2.z - v1.z * v2.y;
	normal->y = v1.z * v2.x - v1.x * v2.z;
	normal->z = v1.x * v2.y - v1.y * v2.x;

	GLfloat d = sqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
	if (d == 0.0) {
		return;
	}
	normal->x /= d;
	normal->y /= d;
	normal->z /= d;
}

float Skeleton::calculateDotProduct(G308_Point v1, G308_Point v2){
	// a . b = |a|*|b|*cosA
	// a.b
	float gore = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	// |a|
	float magA = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
	// |b|
	float magB = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

	float angle = 0.0;
	angle = acos(gore / (magA * magB));
	return angle * (180.0 / 3.1416);
}
