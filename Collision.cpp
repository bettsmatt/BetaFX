/*
 * Collision.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: caskeythom
 */

#include "Collision.h"
#include "math.h"
#include "stdio.h"
#include "G308_Geometry.h"

Collision::Collision() {}

Collision::~Collision() {}

bool Collision::checkIfCollidedBalls(Ball* b1, Ball* b2){
	return (calculateVectorDistance(b1->position, b2->position) < (b1->mass + b2->mass));
}

bool Collision::checkIfCollidedBallParticle(Ball* b, Particle* p){
	return (calculateVectorDistance(b->position, p->position) < b->mass + 0.1f);
}

float Collision::calculateVectorDistance(float *v1, float* v2)
{
	float v[3] = {
			v1[0] - v2[0],
			v1[1] - v2[1],
			v1[2] - v2[2]
	};
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void Collision::collisionBall(double cor, double mass1, double mass2, double radius1, double radius2,
		float* pos1, float* pos2, float* vel1, float* vel2){
	//	printf("Ball position is before: %f        ", pos1[0]);
	// Initialize
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


	//	printf("pos2[2]: %f  relativeDistance: %f \n", distance[2], relativeDistance);
	float st = sin(acos(distance[2] / relativeDistance));
	float ct = cos(acos(distance[2] / relativeDistance));
	float sp = sin(phi2);
	float cp = cos(phi2);

	//	printf("st: %f\n", st);
	//	printf("ct: %f\n", ct);

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

	//	printf("Ball position is after: %f\n", pos1[0]);
	return;
}

void Collision::collisionPlane(double cor, float* ballVel, float* planeNormal){
	//Project the spheres velocity on the planes normal
	// a . b
	// ----- * b
	//  |a|
	/*float velocityMag = vectorMagnitude(ballVel);
	float dotProduct = vectorDotProduct(ballVel, planeNormal);
	float projectionLength = dotProduct / velocityMag;

	float projectionVector[3] = {
			planeNormal[0] * projectionLength,
			planeNormal[1] * projectionLength,
			planeNormal[2] * projectionLength
	};

	//multiply the projection by 2
	cor += 1;
	projectionVector[0] *= cor;
	projectionVector[1] *= cor;
	projectionVector[2] *= cor;

	//subtract it from the spheres velocity
	float finalVelocity[3] = {
			ballVel[0] - projectionVector[0],
			ballVel[1] - projectionVector[1],
			ballVel[2] - projectionVector[2]
	};

	ballVel = finalVelocity;*/

	/*printf("ballVel before: %f %f %f", ballVel[0], ballVel[1], ballVel[2]);


	// if we are moving in the direction of the plane (against the normal)...
	float dotprod = (ballVel[0] * planeNormal[0] + ballVel[1] * planeNormal[1] + ballVel[2] * planeNormal[2]);
	printf("Dot product: %f", dotprod);
	if (dotprod < 0.0f)
	{
		// Calculate the projection velocity
		float velocityMag = vectorMagnitude(ballVel);
		float dotProduct = vectorDotProduct(ballVel, planeNormal);
		float projectionLength = dotProduct / velocityMag;

		float projectionVector[3] = {
				planeNormal[0] * projectionLength,
				planeNormal[1] * projectionLength,
				planeNormal[2] * projectionLength
		};


		cor += 1;
		projectionVector[0] *= cor;
		projectionVector[1] *= cor;
		projectionVector[2] *= cor;

		ballVel = projectionVector;
		printf("done");
		printf("ballVel after: %f %f %f\n", ballVel[0], ballVel[1], ballVel[2]);
	}*/
	//Vnew = b * ( -2*(V dot N)*N + V )

	float dotprod = -2 * (ballVel[0] * planeNormal[0] + ballVel[1] * planeNormal[1] + ballVel[2] * planeNormal[2]);
	float nextVector[3] = {
			cor * (planeNormal[0] * dotprod + ballVel[0]),
			cor * (planeNormal[1] * dotprod + ballVel[1]),
			cor * (planeNormal[2] * dotprod + ballVel[2])
	};
	ballVel[0] = nextVector[0];
	ballVel[1] = nextVector[1];
	ballVel[2] = nextVector[2];
}

void Collision::checkCollision(double cor, Cube *c, Ball *b)
{                                        // C->X-C->SizeX/2
	if ( ((b->position[0] + b->mass) >= (c->position[0] - c->width/2) &&
			(b->position[0] - b->mass) <= (c->position[0] + c->width/2)) &&

			((b->position[1] + b->mass) >= (c->position[1] - c->height/2) &&
					(b->position[1] - b->mass) <= (c->position[1] + c->height/2)) &&

					((b->position[2] + b->mass) >= (c->position[2] - c->height/2) &&
							(b->position[2] - b->mass) <= (c->position[2] + c->height/2)))
	{
		//Collision occured - check which direction to bounce in
		if((b->position[0] > (c->position[0] + c->width/2) && b->velocity[0] < 0) ||
				(b->position[0] < (c->position[0] - c->width/2) && b->velocity[0] > 0)){
			//Collision in x direction
			//b->position[0] -= b->velocity[0]; //Move back
			//b->velocity[0] = -b->velocity[0]*cor; //Invert velocity
			b->position[1] -= b->velocity[1]; //Move back
			b->position[0] -= b->velocity[0];
			b->position[2] -= b->velocity[2];
			float n[3] = {1, 0, 0};
			collisionPlane(cor, b->velocity, n);
		}
		else if((b->position[1] > (c->position[1] + c->height/2) && b->velocity[1] < 0) ||
				(b->position[1] < (c->position[1] - c->height/2) && b->velocity[1] > 0)){
			//printf("ballVel before: %f %f %f\n", b->velocity[0], b->velocity[1], b->velocity[2]);
			//Collision in z direction
			b->position[1] -= b->velocity[1]; //Move back
			b->position[0] -= b->velocity[0];
			b->position[2] -= b->velocity[2];
			float n[3] = {0, 1, 0};
			collisionPlane(cor, b->velocity, n);
			//b->velocity[1] = -b->velocity[1]*cor; //Invert velocity
		}
		else if((b->position[2] > (c->position[2] + c->height/2) && b->velocity[2] < 0) ||
				(b->position[2] < (c->position[2] - c->height/2) && b->velocity[2] > 0)){
			//b->position[2] -= b->velocity[2]; //Move back
			//b->velocity[2] = -b->velocity[2]*cor; //Invert velocity
			b->position[1] -= b->velocity[1]; //Move back
			b->position[0] -= b->velocity[0];
			b->position[2] -= b->velocity[2];
			float n[3] = {0, 0, 1};
			collisionPlane(cor, b->velocity, n);
		}
	}





/*
	// Get the center of the sphere relative to the center of the box
	float* sphereCenterRelBox = Sphere.center - Box.center;
	// Point on surface of box that is closest to the center of the sphere
	float* boxPoint = new float[3];

	// Check sphere center against box along the X axis alone.
	// If the sphere is off past the left edge of the box,
	// then the left edge is closest to the sphere.
	// Similar if it's past the right edge. If it's between
	// the left and right edges, then the sphere's own X
	// is closest, because that makes the X distance 0,
	// and you can't get much closer than that :)

	if (sphereCenterRelBox.x < -Box.GetWidth()/2.0)
	    boxPoint.x = -Box.GetWidth()/2.0;
	else if (sphereCenterRelBox.x > Box.GetWidth()/2.0)
	    boxPoint.x = Box.GetWidth()/2.0;
	else
	    boxPoint.x = sphereCenterRelBox.x;

	// ...same for Y axis
	if (sphereCenterRelBox.y < -Box.GetHeight()/2.0)
	    boxPoint.y = -Box.GetHeight()/2.0;
	else if (sphereCenterRelBox.y > Box.GetHeight()/2.0)
	    boxPoint.y = Box.GetHeight()/2.0;
	else
	    boxPoint.y = sphereCenterRelBox.y;

	// ... same for Z axis
	if (sphereCenterRelBox.z < -Box.GetLength()/2.0)
	    boxPoint.z = -Box.GetLength()/2.0;
	else if (sphereCenterRelBox.x > Box.GetLength()/2.0)
	    boxPoint.z = Box.GetLength()/2.0;
	else
	    boxPoint.z = sphereCenterRelBox.z;

	// Now we have the closest point on the box, so get the distance from
	// that to the sphere center, and see if it's less than the radius

	Vec3 dist = sphereCenterRelBox - boxPoint;

	if (dist.x*dist.x + dist.y*dist.y + distz*dist.z < Sphere.radius*Sphere.radius)
	    return true;
	else
	    return false;*/
}

/*void Collision::checkCollision(double cor, G308_Geometry *g, Ball *b)
{
	//x2 + y2 + z2 = r2;
	G308_Point* points = new G308_Point[6];

}*/
