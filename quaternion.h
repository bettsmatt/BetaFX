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

#ifndef QUATERNIONH
#define QUATERNIONH

#include <stdio.h>
#include <GL/glut.h>
#include "define.h"

class quaternion {

private:
	float a,b,c,d;

public:
	quaternion(const G308_Point&,const G308_Point&);
	quaternion(float, const G308_Point&);
	quaternion(float=0,float=0,float=0,float=0);
	quaternion(float[16]);
	quaternion(const quaternion&);
	~quaternion();
	quaternion& operator=(const quaternion&);
	float length() const;
	quaternion conjugate() const;
	void toMatrix(float*) const;
	quaternion normalise() const;
	quaternion multiplicativeInverse() const;
	G308_Point vector() const;
	void print()const;

friend quaternion operator+(const quaternion&, const quaternion&);
friend quaternion operator-(const quaternion&, const quaternion&);
friend quaternion operator*(const quaternion&, const quaternion&);
friend quaternion operator*(const quaternion&, const float&);
friend quaternion operator/(const quaternion&, const quaternion&);
friend quaternion operator/(const quaternion&, const float&);
friend float dotproduct(const quaternion&, const quaternion&);
friend quaternion slerp(const quaternion&, const quaternion&, float);


};

float dotproduct(G308_Point,G308_Point);
G308_Point crossproduct(G308_Point,G308_Point);

#endif
