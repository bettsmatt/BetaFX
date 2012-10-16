/*
 * GJK.h
 *
 *  Created on: Oct 14, 2012
 *      Author: caskeythom
 */

#ifndef GJK_H_
#define GJK_H_

#include "define.h"

class GJK {
public:
	GJK();
	~GJK();

	int maxIterations, currentSimplexPosition;
	int s1c, s2c;

	bool shapesIntersect(G308_Point*, G308_Point*, int, int);
	int pointDotProduct(G308_Point, G308_Point);
	G308_Point pointCrossProduct(G308_Point, G308_Point);
	G308_Point pointSubtraction(G308_Point, G308_Point);
	G308_Point pointNegation(G308_Point p);
	int sign(float);

	G308_Point pointScalarMultiplication(G308_Point, float);

	bool updateSimplexAndDirection(G308_Point*, G308_Point);

	G308_Point maxPointInMinkDiffAlongDir(G308_Point*, G308_Point*, G308_Point);

	// Finds the farthest point along a given direction of a convex polyhedron
	G308_Point maxPointAlongDirection(G308_Point*, G308_Point, bool);

	void removeValue(G308_Point*, G308_Point);
};

#endif /* GJK_H_ */
