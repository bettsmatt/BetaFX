/*
 * Collision.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: caskeythom
 */

#include "Collision.h"
#include "math.h"
#include "stdio.h"

Collision::Collision() {}

Collision::~Collision() {}

bool Collision::checkIfCollidedBalls(Ball* b1, Ball* b2){
	return (calculateVectorDistance(b1->position, b2->position) < 2);
}

bool Collision::checkIfCollidedBallParticle(Ball* b, Particle* p){
	return (calculateVectorDistance(b->position, p->position) < 1.1);
}

float Collision::calculateVectorDistance(float *v1, float* v2)
{
	float* v = new float[3];
	v[0] = v1[0] - v2[0];
	v[1] = v1[1] - v2[1];
	v[2] = v1[2] - v2[2];
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void Collision::collision3D(double cor, double mass1, double mass2, double radius1, double radius2,
		float* pos1, float* pos2, float* vel1, float* vel2, int error){
	printf("Ball position is before: %f        ", pos1[0]);
	// Initialize
	error = 0;
	float totalRadius = radius1 + radius2;
	float totalMass = mass2 / mass1;
	float distance[3] =  {pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]};
	float velocity[3] = {vel2[0] - vel1[0], vel2[1] - vel1[1], vel2[2] - vel1[2]};

	// Calculate relative distance and Velocity
	float relativeDistance = sqrt(distance[0] * distance[0] + distance[1] * distance[1] + distance[2] * distance[2]);
	float relativeVelocity = sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]);

	// Boost coordinate system so that ball 2 is resting
	vel1[0] = -velocity[0];
	vel1[1] = -velocity[1];
	vel1[2] = -velocity[2];

	// Find the polar coordinates of the location of ball 2
	float phi2 = 0;
	if (distance[0] != 0 || distance[1] != 0) phi2 = atan2(distance[1], distance[0]);


	printf("pos2[2]: %f  relativeDistance: %f \n", distance[2], relativeDistance);
	float st = sin(acos(distance[2] / relativeDistance));
	float ct = cos(acos(distance[2] / relativeDistance));
	float sp = sin(phi2);
	float cp = cos(phi2);

	printf("st: %f\n", st);
	printf("ct: %f\n", ct);

	// Express the velocity vector of ball 1 in a rotated coordinate
	// System where ball 2 lies on the z-axis
	float vx1r = ct * cp * vel1[0] + ct * sp * vel1[1] - st * vel1[2];
	float vy1r = cp * vel1[1] - sp * vel1[0];
	float vz1r = st * cp * vel1[0] + st * sp * vel1[1] + ct * vel1[2];
	float fvz1r = vz1r / relativeVelocity ;
	if (fvz1r>1) {fvz1r = 1;}   // fix for possible rounding errors
	else if (fvz1r<-1) {fvz1r =- 1;}

	float thetav = acos(fvz1r);


	// Calculate the normalized impact parameter
	float dr = relativeDistance * sin(thetav) / totalRadius;

	// Calculate impact angle
	float impactAngle1 = asin(-dr);
	float impactAngle2 = atan2(vy1r, vx1r);
	if (vx1r == 0 && vy1r == 0) impactAngle2 = 0;

	// Update velocities
	float a = tan(thetav + impactAngle1);
	float vz2r = 2 * (vz1r + a * (cos(impactAngle2)*vx1r+sin(impactAngle2)*vy1r))/((1+a*a)*(1+totalMass));
	float vx2r = a * cos(impactAngle2)*vz2r;
	float vy2r = a * sin(impactAngle2)*vz2r;
	vz1r = (vz1r - totalMass * vz2r);
	vx1r = vx1r - totalMass * vx2r;
	vy1r = vy1r - totalMass * vy2r;

	// Rotate the velocity vectors back and add the initial velocity vector of ball 2 to retrieve the original coordinate system
	vel1[0]= ct * cp * vx1r - sp * vy1r + st * cp * vz1r + vel2[0];
	vel1[1]= ct * sp * vx1r + cp * vy1r + st * sp * vz1r + vel2[1];
	vel1[2]= ct * vz1r - st * vx1r +vel2[2];
	vel2[0]= ct * cp * vx2r - sp * vy2r + st * cp * vz2r + vel2[0];
	vel2[1]= ct * sp * vx2r + cp * vy2r + st * sp * vz2r + vel2[1];
	vel2[2]= ct * vz2r - st * vx2r +vel2[2];

	for(int i = 0; i < 3; i++){
		pos1[i] += vel1[i];
		pos2[i] += vel2[i];
	}

	printf("Ball position is after: %f\n", pos1[0]);
	return;
}
