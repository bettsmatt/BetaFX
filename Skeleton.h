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

#ifndef SKELETONH
#define SKELETONH

#include <stdio.h>
#include <GL/glut.h>
#include "define.h"
#include "BSpline.h"
#include "ControlPoint.h"

using namespace std;

// Using bitmasking to denote different degrees of freedom for joint motion
typedef int DOF;

#define DOF_NONE 0
#define DOF_RX 1
#define DOF_RY 2
#define DOF_RZ 4
#define DOF_ROOT 8 // Root has 6, 3 translation and 3 rotation

//Type to represent a AMC bone rotation
typedef struct bonerotation {
	//Challenge stuff
	float tx, ty, tz;
	float rx, ry, rz;

} bonerotation;

//Type to represent a bone
typedef struct bone {
	int id;
	char* name;
	float dirx, diry, dirz;
	float rotx, roty, rotz;
	DOF dof;
	float length;
	bone** children;
	int numChildren;

	bonerotation frames[800];
	//Challenge stuff
	float currentTranslatex, currentTranslatey, currentTranslatez;
	float currentRotationx, currentRotationy, currentRotationz;

} bone;




void trim(char**);

class Skeleton {

private:
	ControlPoint position;
	float angle;
	ControlPoint rotAxis;

	int buffSize, maxBones;
	void deleteBones(bone*);
	void display(bone*, GLUquadric*);
	void drawParts(bone*, GLUquadric*);
	void drawOnePart(bone*, GLUquadric*);
	void doAMCrotation(bone*);

	void calculateCrossProduct(G308_Point v1, G308_Point v2, G308_Point* normal);
	float calculateDotProduct(G308_Point v1, G308_Point v2);

public:
	int numBones;
	int motionframe;
	int amcFileProvided;
	int currentFrameNumber;
	bone* root;
	Skeleton();
	~Skeleton();
	void display();
	void move(BSpline* bs);
};

#endif
