/*
 * GJK.cpp
 *
 *  Created on: Oct 14, 2012
 *      Author: caskeythom
 */

#include "GJK.h"
#include "stdio.h"

GJK::GJK() {
	maxIterations = 20;
	currentSimplexPosition = 0;
}

GJK::~GJK() {
}

// Given the vertices (in any order) of two convex 3D bodies, calculates whether they intersect
bool GJK::shapesIntersect(G308_Point* shape1, G308_Point* shape2, int shape1Count, int shape2Count){
	s1c = shape1Count;
	s2c = shape2Count;
	// Take the difference between any two vertices
	G308_Point initialPoint = G308_Point();
	initialPoint.x = shape1[0].x - shape2[0].x;
	initialPoint.y = shape1[0].y - shape2[0].y;
	initialPoint.z = shape1[0].z - shape2[0].z;


	G308_Point S = maxPointInMinkDiffAlongDir(shape1, shape2, initialPoint);

	G308_Point D = pointNegation(S);

	G308_Point* simplex = new G308_Point[10000];
	simplex[currentSimplexPosition] = S;
	currentSimplexPosition++;

	for (int i = 0; i < maxIterations; i++)
	{
		G308_Point A = maxPointInMinkDiffAlongDir(shape1, shape2, D);
		if (pointDotProduct(A, D) < 0)
		{
			delete[] simplex;
			return false;
		}

		simplex[currentSimplexPosition] = A;
		currentSimplexPosition++;

		if (updateSimplexAndDirection(simplex, D))
		{
			delete[] simplex;
			return true;
		}
	}
	delete[] simplex;
	return false;
}

G308_Point GJK::getMax1(){
	return max1;
}

// Updates the current simplex and the direction in which to look for the origin. Called DoSimplex in the video lecture.
bool GJK::updateSimplexAndDirection(G308_Point* simplex, G308_Point direction)
{
	if (currentSimplexPosition == 2)//if the simplex is a line
	{
		G308_Point A = simplex[1];
		G308_Point B = simplex[0];
		G308_Point AB = pointSubtraction(B, A);

		G308_Point AO = pointNegation(A);

		if (pointDotProduct(AB, AO) > 0)
		{
			direction = pointCrossProduct(pointCrossProduct(AB, AO), AB);
		}
		else
		{
			direction = AO;
		}
	}
	else if (currentSimplexPosition == 3)//if the simplex is a triangle
	{
		G308_Point A = simplex[2];
		G308_Point B = simplex[1];
		G308_Point C = simplex[0];
		G308_Point AO = pointNegation(A);

		G308_Point AB = pointSubtraction(B, A);
		G308_Point AC = pointSubtraction(C, A);
		G308_Point ABC = pointCrossProduct(AB, AC);

		if (pointDotProduct(pointCrossProduct(ABC, AC), AO) > 0)
		{
			if (pointDotProduct(AC, AO) > 0)
			{
				delete[] simplex;
				simplex = new G308_Point[10000];
				currentSimplexPosition = 0;


				simplex[currentSimplexPosition] = C;
				currentSimplexPosition++;
				simplex[currentSimplexPosition] = A;
				currentSimplexPosition++;
				direction = pointCrossProduct( pointCrossProduct(AC, AO), AC);
			}
			else
				if (pointDotProduct(AB, AO) > 0)
				{
					delete[] simplex;
					simplex = new G308_Point[10000];
					currentSimplexPosition = 0;

					simplex[currentSimplexPosition] = B;
					currentSimplexPosition++;
					simplex[currentSimplexPosition] = A;
					currentSimplexPosition++;

					direction = pointCrossProduct( pointCrossProduct(AB, AO), AB);

				}
				else
				{
					delete[] simplex;
					simplex = new G308_Point[10000];
					currentSimplexPosition = 0;
					simplex[currentSimplexPosition] = A;
					currentSimplexPosition++;
					direction = AO;
				}
		}
		else
		{
			if (pointDotProduct(pointCrossProduct(AB, ABC), AO) > 0)
			{
				if (pointDotProduct(AB, AO) > 0)
				{
					delete[] simplex;
					simplex = new G308_Point[10000];
					currentSimplexPosition = 0;

					simplex[currentSimplexPosition] = B;
					currentSimplexPosition++;
					simplex[currentSimplexPosition] = A;
					currentSimplexPosition++;

					direction = pointCrossProduct( pointCrossProduct(AB, AO), AB);

				}
				else
				{
					delete[] simplex;
					simplex = new G308_Point[10000];
					currentSimplexPosition = 0;
					simplex[currentSimplexPosition] = A;
					currentSimplexPosition++;
					direction = AO;
				}
			}
			else
			{
				if (pointDotProduct(ABC, AO) > 0)
				{
					//the simplex stays A, B, C
					direction = ABC;
				}
				else
				{
					delete[] simplex;
					simplex = new G308_Point[10000];
					currentSimplexPosition = 0;

					simplex[currentSimplexPosition] = B;
					currentSimplexPosition++;
					simplex[currentSimplexPosition] = C;
					currentSimplexPosition++;

					simplex[currentSimplexPosition] = A;
					currentSimplexPosition++;

					direction = pointNegation(ABC);
				}
			}
		}
	}
	else//if the simplex is a tetrahedron
	{
		G308_Point A = simplex[3];
		G308_Point B = simplex[2];
		G308_Point C = simplex[1];
		G308_Point D = simplex[0];

		G308_Point AO = pointNegation(A);
		G308_Point AB = pointSubtraction(B, A);
		G308_Point AC = pointSubtraction(C, A);
		G308_Point AD = pointSubtraction(D, A);
		G308_Point ABC = pointCrossProduct(AB, AC);
		G308_Point ACD = pointCrossProduct(AC, AD);
		G308_Point ADB = pointCrossProduct(AD, AB);

		//the side (positive or negative) of B, C and D relative to the planes of ACD, ADB and ABC respectively
		int BsideOnACD = sign(pointDotProduct(ACD, AB));
		int CsideOnADB = sign(pointDotProduct(ADB, AC));
		int DsideOnABC = sign(pointDotProduct(ABC, AD));

		//whether the origin is on the same side of ACD/ADB/ABC as B, C and D
		bool ABsameAsOrigin = sign(pointDotProduct(ACD, AO)) == BsideOnACD;
		bool ACsameAsOrigin = sign(pointDotProduct(ADB, AO)) == CsideOnADB;
		bool ADsameAsOrigin = sign(pointDotProduct(ABC, AO)) == DsideOnABC;

		//if the origin is on the same side as all B, C and D, the origin is inside the tetrahedron and there is a collision
		if (ABsameAsOrigin && ACsameAsOrigin && ADsameAsOrigin)
		{
			return true;
		}
		//if the origin is not on the side of B relative to ACD
		else if (!ABsameAsOrigin)
		{
			//B is farthest from the origin among all of the tetrahedron's points, so remove it
			removeValue(simplex, B);
			direction = pointScalarMultiplication(ACD, -BsideOnACD);
		}
		//if the origin is not on the side of C relative to ADB
		else if (!ACsameAsOrigin)
		{
			//C is farthest from the origin among all of the tetrahedron's points, so remove it
			removeValue(simplex, C);
			direction = pointScalarMultiplication(ADB, -CsideOnADB);
		}
		//if the origin is not on the side of D relative to ABC
		else
		{
			//D is farthest from the origin among all of the tetrahedron's points, so remove it
			removeValue(simplex, D);
			direction = pointScalarMultiplication(ABC, -DsideOnABC);
		}
		return updateSimplexAndDirection(simplex, direction);
	}

	//no intersection found on this iteration
	return false;
}

// Finds the farthest point along a given direction of the Minkowski difference of two convex polyhedra.
G308_Point GJK::maxPointInMinkDiffAlongDir(G308_Point* shape1, G308_Point* shape2, G308_Point direction)
{
	G308_Point negation = G308_Point();
	negation.x = -direction.x;
	negation.y = -direction.y;
	negation.z = -direction.z;

	max1 = maxPointAlongDirection(shape1, direction, false);
	max2 = maxPointAlongDirection(shape2, negation, true);

	return pointSubtraction(max1, max2);
}

// Finds the farthest point along a given direction of a convex polyhedron
G308_Point GJK::maxPointAlongDirection(G308_Point* shape, G308_Point direction, bool is2)
{
	G308_Point max = shape[0];

	int size = s1c;
	if(is2) size = s2c;

	for(int i = 0; i < size; i++){
		G308_Point point = shape[i];
		if(pointDotProduct(max, direction) < pointDotProduct(point, direction)) max = point;
	}

	return max;
}

int GJK::pointDotProduct(G308_Point A, G308_Point D){
	return (A.x * D.x + A.y * D.y + A.z * D.z);
}

G308_Point GJK::pointSubtraction(G308_Point p, G308_Point q){
	G308_Point sub;
	sub.x = p.x - q.x;
	sub.y = p.y - q.y;
	sub.z = p.z - q.z;
	return sub;
}

G308_Point GJK::pointNegation(G308_Point p){
	G308_Point neg;
	neg.x = -p.x;
	neg.y = -p.y;
	neg.z = -p.z;
	return neg;
}

G308_Point GJK::pointCrossProduct(G308_Point p, G308_Point q){
	G308_Point cross;
	cross.x = p.y * q.z - p.z * q.y;
	cross.y = p.z * q.x - p.x * q.z;
	cross.z = p.x * q.y - p.y * q.x;
	return cross;
}

G308_Point GJK::pointScalarMultiplication(G308_Point p, float s){
	G308_Point mult;
	mult.x = p.x * s;
	mult.y = p.y * s;
	mult.z = p.z * s;
	return mult;
}

int GJK::sign(float value){
	if(value < 0) return -1;
	else if(value > 0) return 1;
	else return 0;
}

void GJK::removeValue(G308_Point* array, G308_Point value){
	int place = 0;
	for(int i = 0; i < currentSimplexPosition; i++){
		if(array[i].x == value.x && array[i].y == value.y && array[i].z == value.z){
			place = i;
			break;
		}
	}

	for(int i = place; i < currentSimplexPosition - 1; i++){
		array[i] = array[i + 1];
	}
	currentSimplexPosition--;
}
