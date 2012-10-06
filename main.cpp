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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>
#include "define.h"
#include <string.h>
#include <sstream>
#include <math.h>
#include "G308_Geometry.h"
#include "quaternion.h"
#include "G308_ImageLoader.h"

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

void G308_keyboardListener(unsigned char, int, int);
void G308_Reshape(int w, int h);
void G308_display();
void G308_init();
void G308_SetCamera();
void G308_SetLight();

void mouse(int button, int state, int x, int y);
void mouseMotion (int x, int y);
void menu (int);
G308_Point getArcBallVector (int, int);

// Objects
#define BOX 0
#define BUNNY 1
#define SPHERE 2
#define TABLE 3
#define TEAPOT 4
#define TORUS 5

// Materials
#define BRICK GLuint 0
#define CUBEMAP 1
#define NORMAL 2
#define WOOD 3

#define PI 3.14159265358979323846

/*
 * Current camera position
 */
v3* camPos;
v3* camLookAt;
v3* camRot;

/*
 * Keep track of camera positions
 */
typedef struct camera {
	v3* camPos;
	v3* camLookAt;
	v3* camRot;
} camera;


int px;
int py;
bool rightMouseDown;
bool animationMode;
int menuID;

bool arcBall;
bool pan;
bool zoom;

char** menuItems;

G308_Geometry** geometry = NULL;
int numGeo;

char** geometryFiles;
v3** geometryPositions;

GLuint* textureIDs;
int numTextures;

void loadTexture(char*, GLuint);

void animate ();
int rotations;

v3 spotPosition;
v3 spotDirection;
float spotAngle;

quaternion* rot;

void loadCubemap();

int main(int argc, char** argv) {

	if (argc < 1 ) {
		//Usage instructions for core and challenge
		printf("Usage\n");
		printf("./Ass3 priman.asf poses config\n");
		exit(EXIT_FAILURE);
	}


	rot = new quaternion(1,0,0,0);

	/*
	 * Turn all controls off
	 */
	rightMouseDown = false;
	animationMode = false;
	pan = false;
	zoom = false;
	arcBall = false;

	camPos = (v3*) malloc(sizeof(v3));
	camLookAt = (v3*) malloc(sizeof(v3));
	camRot = (v3*) malloc(sizeof(v3));

	/*
	 * Set initial camera positions
	 */
	camPos->x = 0, camPos->y = 0, camPos->z = 20;
	camLookAt->x = 0, camLookAt->y = 0, camLookAt->z = 0;
	camRot->x = 0, camRot->y = 1, camRot->z = 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(g_nWinWidth, g_nWinHeight);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	g_mainWnd = glutCreateWindow("COMP308 Assignment4");

	glutKeyboardFunc(G308_keyboardListener);

	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);

	G308_init();

	glutIdleFunc(animate);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutMainLoop();

	return EXIT_SUCCESS;
}

void animate(){

	if(rotations > 0){
		rotations--;
		for(int i = 0 ; i < numGeo ; i ++){
			geometry[i]->worldRot += 5;
		}
	}

	G308_display();

}

void loadTexture (char* filename, GLuint id){

	texInfo* t = (texInfo*) malloc(sizeof(texInfo));

	loadTextureFromJPEG(filename, t);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->width, t->height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, t->textureData);
}



/*
 * Menu options
 */
 void menu (int i){

}

// Init Light and Camera
void G308_init() {
	G308_SetLight();
	G308_SetCamera();
}

// Display call back
void G308_display() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}


	glPushMatrix();

	glutWireSphere(3,100,100);

	for(int i = 0 ; i < numGeo ; i ++)
		geometry[i]->RenderGeometry();




	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}


void mouseMotion (int x, int y){

}



void mouse (int button, int state, int x, int y){

}

void G308_keyboardListener(unsigned char key, int x, int y) {

	if(key == 't' && rotations == 0)
		rotations = 72;

	// Forward
	if(key == '8'){
		spotPosition.z += 1;
	}

	// Back
	if(key == '2'){
		spotPosition.z -= 1;
	}

	// Left
	if(key == '4'){
		spotPosition.x -= 1;
	}

	// Right
	if(key == '6'){
		spotPosition.x += 1;
	}

	// Up
	if(key == '+'){
		spotPosition.y -= 1;
	}

	// Down
	if(key == '-'){
		spotPosition.y += 1;
	}

	// AngleUp
	if(key == '7'){
		spotDirection.x += 0.1;
	}

	// AngleDown
	if(key == '9'){
		spotDirection.x -= 0.1;
	}

	// AngleLeft
	if(key == '1'){
		spotDirection.z += 0.1;
	}

	// AngleRight
	if(key == '3'){
		spotDirection.z -= 0.1;
	}

	// Decrease Angle
	if(key == '/'){
		spotAngle -= 1;
	}

	// Increase Angle
	if(key == '*'){
		spotAngle += 1;
	}

	G308_SetLight();
	G308_display();

}

// Reshape function
void G308_Reshape(int w, int h) {
	if (h == 0)
		h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
}

// Set Camera Position
void G308_SetCamera() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
			camPos->x,camPos->y,camPos->z,
			camLookAt->x,camLookAt->y,camLookAt->z,
			camRot->x,camRot->y,camRot->z
	);

}

// Set View Position
void G308_SetLight() {


}

