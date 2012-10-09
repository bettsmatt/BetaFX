/*
 * Collision.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: caskeythom
 */

#include "Collision.h"
#include "math.h"

Collision::Collision() {}

Collision::~Collision() {}

bool Collision::checkIfCollided(Ball* b1, Ball* b2){
	if(calculateVectorDistance(b1->position, b2->position) < 4) return true;
	return false;
}

float Collision::calculateVectorDistance(float *v1, float* v2)
{
	float* v = new float[3];
	v[0] = v1[0] - v2[0];
	v[1] = v1[1] - v2[1];
	v[2] = v1[2] - v2[2];
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void collisiond3D(double R, double m1, double m2, double r1, double r2,
		float* pos1, float* pos2, float* vel1, float* vel2, int error)     {

//     **** initialize some variables ****
       float pi=acos(-1.0E0);
       error=0;
       float r12=r1+r2;
       float m21 = m2 / m1;
       float x21 = pos2[0] - pos1[0];
       float y21 = pos2[1] - pos1[1];
       float z21 = pos2[2] - pos1[2];
       float vx21 = vel2[0] - vel1[0];
       float vy21 = vel2[1] - vel1[1];
       float vz21 = vel2[2] - vel1[2];

       float vx_cm = (m1*vel1[0]+m2*vel2[0])/(m1+m2) ;
       float vy_cm = (m1*vel1[1]+m2*vel2[1])/(m1+m2) ;
       float vz_cm = (m1*vel1[2]+m2*vel2[2])/(m1+m2) ;


//     **** calculate relative distance and relative speed ***
       float d = sqrt(x21*x21 +y21*y21 +z21*z21);
       float v = sqrt(vx21*vx21 +vy21*vy21 +vz21*vz21);

//     **** return if distance between balls smaller than sum of radii ****
       if (d<r12) {error=2; return;}

//     **** return if relative speed = 0 ****
       if (v==0) {error=1; return;}


//     **** shift coordinate system so that ball 1 is at the origin ***
       pos2[0]=x21;
       pos2[1]=y21;
       pos2[2]=z21;

//     **** boost coordinate system so that ball 2 is resting ***
       vel1[0]=-vx21;
       vel1[1]=-vy21;
       vel1[2]=-vz21;

//     **** find the polar coordinates of the location of ball 2 ***
       float phi2 = 0;
       if (pos2[0]==0 && pos2[1]==0) {phi2=0;} else {phi2=atan2(pos2[1],pos2[0]);}
       float st=sin(acos(pos2[2]/d));
       float ct=cos(acos(pos2[2]/d));
       float sp=sin(phi2);
       float cp=cos(phi2);


//     **** express the velocity vector of ball 1 in a rotated coordinate
//          system where ball 2 lies on the z-axis ******
       float vx1r=ct*cp*vel1[0]+ct*sp*vel1[1]-st*vel1[2];
       float vy1r=cp*vel1[1]-sp*vel1[0];
       float vz1r=st*cp*vel1[0]+st*sp*vel1[1]+ct*vel1[2];
       float fvz1r = vz1r/v ;
       if (fvz1r>1) {fvz1r=1;}   // fix for possible rounding errors
          else if (fvz1r<-1) {fvz1r=-1;}



       float thetav=acos(fvz1r);



//     **** calculate the normalized impact parameter ***
       float dr=d*sin(thetav)/r12;


//     **** return old positions and velocities if balls do not collide ***
       if (thetav>pi/2 || fabs(dr)>1) {
           pos2[0]=pos2[0]+pos1[0];
           pos2[1]=pos2[1]+pos1[1];
           pos2[2]=pos2[2]+pos1[2];
           vel1[0]=vel1[0]+vel2[0];
           vel1[1]=vel1[1]+vel2[1];
           vel1[2]=vel1[2]+vel2[2];
           error=1;
           return;
        }

//     **** calculate impact angles if balls do collide ***
       float alpha=asin(-dr);


       float beta = atan2(vy1r,vx1r);
       if (vx1r==0 && vy1r==0) beta = 0;



//     **** calculate time to collision ***
       float t=(d*cos(thetav) -r12*sqrt(1-dr*dr))/v;


//     **** update positions and reverse the coordinate shift ***
       pos2[0]=pos2[0]+vel2[0]*t +pos1[0];
       pos2[1]=pos2[1]+vel2[1]*t +pos1[1];
       pos2[2]=pos2[2]+vel2[2]*t +pos1[2];
       pos1[0]=(vel1[0]+vel2[0])*t +pos1[0];
       pos1[1]=(vel1[1]+vel2[1])*t +pos1[1];
       pos1[2]=(vel1[2]+vel2[2])*t +pos1[2];



//  ***  update velocities ***
       float a = tan(thetav+alpha);

       float dvz2=2*(vz1r+a*(cos(beta)*vx1r+sin(beta)*vy1r))/((1+a*a)*(1+m21));

       float vz2r=dvz2;
       float vx2r=a*cos(beta)*dvz2;
       float vy2r=a*sin(beta)*dvz2;
       vz1r=vz1r-m21*vz2r;
       vx1r=vx1r-m21*vx2r;
       vy1r=vy1r-m21*vy2r;


//     **** rotate the velocity vectors back and add the initial velocity
//           vector of ball 2 to retrieve the original coordinate system ****

       vel1[0]=ct*cp*vx1r-sp*vy1r+st*cp*vz1r +vel2[0];
       vel1[1]=ct*sp*vx1r+cp*vy1r+st*sp*vz1r +vel2[1];
       vel1[2]=ct*vz1r-st*vx1r               +vel2[2];
       vel2[0]=ct*cp*vx2r-sp*vy2r+st*cp*vz2r +vel2[0];
       vel2[1]=ct*sp*vx2r+cp*vy2r+st*sp*vz2r +vel2[1];
       vel2[2]=ct*vz2r-st*vx2r               +vel2[2];


//     ***  velocity correction for inelastic collisions ***

       vel1[0]=(vel1[0]-vx_cm)*R + vx_cm;
       vel1[1]=(vel1[1]-vy_cm)*R + vy_cm;
       vel1[2]=(vel1[2]-vz_cm)*R + vz_cm;
       vel2[0]=(vel2[0]-vx_cm)*R + vx_cm;
       vel2[1]=(vel2[1]-vy_cm)*R + vy_cm;
       vel2[2]=(vel2[2]-vz_cm)*R + vz_cm;

       return;
}
