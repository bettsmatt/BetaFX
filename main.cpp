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
#include "ParticleEmitter.h"

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

void tick();

/*
 * Particle Emitter
 */
ParticleEmitter* particeEmitter;

G308_Geometry** geometry = NULL;
int numGeo;

void loadTexture(char*, GLuint);

int main(int argc, char** argv) {


	if (argc < 0 ) {
		// Print message for usage here
		exit(EXIT_FAILURE);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(g_nWinWidth, g_nWinHeight);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	g_mainWnd = glutCreateWindow("BetaFX");

	glutKeyboardFunc(G308_keyboardListener);

	glutDisplayFunc(G308_display);
	glutReshapeFunc(G308_Reshape);


	particeEmitter = new ParticleEmitter();
	float* v = new float[3];
	v[0] = 0;
	v[1] = 1;
	v[2] = 0;
	particeEmitter->setVector(v);

	G308_init();
	glutIdleFunc(tick);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutMainLoop();

	return EXIT_SUCCESS;
}

/*
 * Load a texture and bind to an ID
 */
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
 * Do stuff, in the background
 */
void tick (){
	particeEmitter->tick();
	particeEmitter->emit();
	G308_display();
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

	/*
	 * Draw stuff here!
	 */

	//glutWireSphere(3,100,100);

	/*
	 * Particle Emitter
	 */
	particeEmitter->renderParticles();



	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

/*
 * Mouse Movement
 */
void mouseMotion (int x, int y){

}

/*
 * Mouse Clicks
 */
void mouse (int button, int state, int x, int y){

}

/*
 * Keys
 */
void G308_keyboardListener(unsigned char key, int x, int y) {

}

/*
 * Reshape function
 */
void G308_Reshape(int w, int h) {
	if (h == 0)
		h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

	glViewport(0, 0, g_nWinWidth, g_nWinHeight);
}

/*
 * Set Camera Position
 */
void G308_SetCamera() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
			0, 0, 200,
			0, 0, 0,
			0, 1, 0
	);

}

// Set View Position
void G308_SetLight() {


}

