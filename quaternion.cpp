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


#include "quaternion.h"
#include <math.h>

float dotproduct(G308_Point p, G308_Point q) {
	float a = 0;
	a += p.x * q.x;
	a += p.y * q.y;
	a += p.z * q.z;
	return a;
}

G308_Point crossproduct(G308_Point p, G308_Point q) {
	G308_Point cross;
	cross.x = p.y * q.z - p.z * q.y;
	cross.y = p.z * q.x - p.x * q.z;
	cross.z = p.x * q.y - p.y * q.x;
	return cross;
}

quaternion::quaternion(float r, float i, float j, float k) :
		a(r), b(i), c(j), d(k) {
}

quaternion::quaternion(float angle, const G308_Point& p) {
	float ang = angle / (2 * 57.2957795); //convert into radians and halve
	a = cos(ang);
	float s = sin(ang);
	b = s * p.x;
	c = s * p.y;
	d = s * p.z;
}

//Out by a factor of 2  like in the arc ball paper
quaternion::quaternion(const G308_Point &p1, const G308_Point& p2) {
	a = dotproduct(p1, p2);
	G308_Point cross = crossproduct(p1, p2);
	b = cross.x;
	c = cross.y;
	d = cross.z;
}

quaternion::quaternion(float mat[16]) {
	float tr, s, q[4];
	int i, j, k;
	int nxt[3] = { 1, 2, 0 };
	tr = mat[0] + mat[5] + mat[10];
	if (tr > 0.0) {
		s = sqrt(tr + 1.0);
		a = s / 2.0;
		s = 0.5 / s;
		b = (mat[4 + 2] - mat[2 * 4 + 1]) * s;
		c = (mat[4 * 2 + 0] - mat[2]) * s;
		d = (mat[1] - mat[4]) * s;
	} else {
		i = 0;
		if (mat[5] > mat[0]) {
			i = 1;
		}
		if (mat[10] > mat[4 * i + i]) {
			i = 2;
		}
		j = nxt[i];
		k = nxt[j];
		s = sqrt((mat[4 * i + i] - (mat[4 * j + i] + mat[4 * k + k])) + 1.0);
		q[i] = s * 0.5;
		if (s != 0.0) {
			s = 0.5 / s;
			q[3] = (mat[4 * j + k] - mat[4 * k + j]) * s;
			q[j] = (mat[4 * i + j] - mat[4 * j + i]) * s;
			q[k] = (mat[4 * i + k] - mat[4 * k + i]) * s;
			b = q[0];
			c = q[1];
			d = q[2];
			a = q[3];
		}
	}
}

quaternion::quaternion(const quaternion& q) :
		a(q.a), b(q.b), c(q.c), d(q.d) {

}

quaternion::~quaternion() {

}

G308_Point quaternion::vector() const{
	G308_Point v;
	v.x = b;
	v.y = c;
	v.z = d;
	return v;
}

float quaternion::length() const {
	float l = a * a + b * b + c * c + d * d;
	l = sqrt(l);
	return l;
}

quaternion quaternion::conjugate() const {
	quaternion q(a, -b, -c, -d);
	return q;
}

quaternion quaternion::multiplicativeInverse() const {
	float len = length();
	len = len * len;
	quaternion c = conjugate();
	quaternion inverse = c / len;
	return inverse;
}

quaternion quaternion::normalise() const {
	float l = length();
	return (*this) / l;
}

void quaternion::print() const{
	printf("(%.2f,%.2f.%.2f,%.2f)",a,b,c,d);
}

void quaternion::toMatrix(float* matrix) const {
	//OpenGL friendly => by column!
	quaternion q = normalise();

	//column 1
	matrix[0] = q.a * q.a + q.b * q.b - q.c * q.c - q.d * q.d;
	matrix[1] = 2 * q.b * q.c + 2 * q.a * q.d;
	matrix[2] = 2 * q.b * q.d - 2 * q.a * q.c;
	matrix[3] = 0;

	//column 2
	matrix[4] = 2 * q.b * q.c - 2 * q.a * q.d;
	matrix[5] = q.a * q.a - q.b * q.b + q.c * q.c - q.d * q.d;
	matrix[6] = 2 * q.c * q.d + 2 * q.a * q.b;
	matrix[7] = 0;

	//column 3
	matrix[8] = 2 * q.b * q.d + 2 * q.a * q.c;
	matrix[9] = 2 * q.c * q.d - 2 * q.a * q.b;
	matrix[10] = q.a * q.a - q.b * q.b - q.c * q.c + q.d * q.d;
	matrix[11] = 0;

	//column 4
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = q.a * q.a + q.b * q.b + q.c * q.c + q.d * q.d;
}

quaternion& quaternion::operator=(const quaternion& q) {
	a = q.a;
	b = q.b;
	c = q.c;
	d = q.d;
	return *this;
}

quaternion operator+(const quaternion& q1, const quaternion& q2) {
	quaternion q(q1.a + q2.a, q1.b + q2.b, q1.c + q2.c, q1.d + q2.d);
	return q;
}

quaternion operator-(const quaternion& q1, const quaternion& q2) {
	quaternion q(q1.a - q2.a, q1.b - q2.b, q1.c - q2.c, q1.d - q2.d);
	return q;
}

quaternion operator*(const quaternion& q1, const quaternion& q2) {
	float a = q1.a * q2.a - q1.b * q2.b - q1.c * q2.c - q1.d * q2.d;
	float b = q1.a * q2.b + q1.b * q2.a + q1.c * q2.d - q1.d * q2.c;
	float c = q1.a * q2.c - q1.b * q2.d + q1.c * q2.a + q1.d * q2.b;
	float d = q1.a * q2.d + q1.b * q2.c - q1.c * q2.b + q1.d * q2.a;

	quaternion q(a, b, c, d);
	return q;
}

quaternion operator*(const quaternion& q1, const float& f) {
	quaternion q(f * q1.a, f * q1.b, f * q1.c, f * q1.d);
	return q;
}

quaternion operator/(const quaternion& q1, const quaternion& q2) {
	quaternion q2inv = q2.multiplicativeInverse();
	quaternion r = q1 * q2inv;
	return r;
}

quaternion operator/(const quaternion& q1, const float& f) {
	quaternion q(q1.a / f, q1.b / f, q1.c / f, q1.d / f);
	return q;
}

float dotproduct(const quaternion& q1, const quaternion& q2) {
	float f = q1.a * q2.a + q1.b * q2.b + q1.c * q2.c + q1.d * q2.d;
	return f;
}

quaternion slerp(const quaternion& p1, const quaternion& q1, float t) {
	quaternion q = q1.normalise();
	quaternion p = p1.normalise();
	float epsilon = 0.0001;
	if (dotproduct(p, q) < 0) {
		q = q * -1;
	}
	float dpq = dotproduct(p, q);
	if ((1.0 - dpq) > epsilon) {
		float w = acos(dpq);
		return ((sin((1 - t) * w) * p) + (sin(t * w) * q)) / sin(w);
	} else {
		return (1 - t) * p + t * q;
	}

}
