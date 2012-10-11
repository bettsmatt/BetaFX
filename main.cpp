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

#include "Camera.h"
#include "BSpline.h"
#include "Point.h"

#define NUM_KNOTS 8
#define NUM_POINTS 4

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

int lastx, lasty = 0;

unsigned char buttons[3] = { 0 };

void G308_keyboardListener(unsigned char, int, int);
void G308_Reshape(int w, int h);
void G308_display();
void G308_init();
void G308_SetLight();

void mouse(int button, int state, int x, int y);
void mouseMotion (int x, int y);
void menu (int);

void tick();

float camAngle;
float camHeight;
int animate = 0;

/*
 * Particle Emitter
 */
ParticleEmitter* particeEmitter;

G308_Geometry** geometry = NULL;
Camera* camera = NULL;
BSpline* bspline = NULL;

int key_held = 0;
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

	loadTexture("sprite2.png", 1);

	camAngle = 0;
	camHeight = 0;

	bspline = new BSpline();
	bspline->init();

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

	loadTextureFromPNG(filename, t);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, t->textureData);
}

/*
 * Do stuff, in the background
 */
void tick (){

	/*
	 * Simulate a frame in the particle emitter
	 */
	particeEmitter->tick();

	/*
	 * Rotate the camera for effect
	 */


//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	float x = sin(camAngle) * 50.0f;
//	float z = cos(camAngle) * 50.0f;
//
//	gluLookAt(
//			x,camHeight,z,
//			0,0,0,
//			0,1,0
//	);

	//camAngle+= 0.005f;
	//G308_display();
	glutPostRedisplay();
}


// Init Light and Camera
void G308_init() {
	camera = new Camera();
	camera->SetInitialCameraPosition((double) g_nWinWidth, (double) g_nWinHeight);
	G308_SetLight();
}

// Display call back
void G308_display() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glShadeModel(GL_SMOOTH);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}


	glPushMatrix();

	camera->RotateCamera();

	/*
	 * Draw stuff here!
	 */

	if (bspline->hasChanged) {
		bspline->hasChanged = false;
		bspline->recalculate();
	}
	if(animate == 1){
		camera->moveTo(bspline->nextFrame(), (double)g_nWinWidth, (double)g_nWinHeight);
	}

	// Draw the spline with the control points
	bspline->drawSpline();


	/*
	 * Draw the particle emmiter and it's particles
	 */
	particeEmitter->renderParticles();

	glPopMatrix();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

/*
 * Mouse Movement
 */
void mouseMotion (int x, int y){
	int diffx = x - lastx;
		int diffy = y - lasty;
		lastx = x;
		lasty = y;

		// Changing position of selected control point.
		if(buttons[0] && key_held == MOVE_ALONG_X){
			bspline->moveSelectedPoint((float) 0.05f * diffx, 'x');
		}
		else if(buttons[0] && key_held == MOVE_ALONG_Y){
			bspline->moveSelectedPoint((float) 0.05f * diffx, 'y');
		}
		else if(buttons[0] && key_held == MOVE_ALONG_Z){
			bspline->moveSelectedPoint((float) 0.05f * diffx, 'z');
		}
		// Change the view point of the scene.
		else if (buttons[2]) {
			camera->zoom -= (float) 0.05f * diffy;
		} else if (buttons[0]) {
			camera->rotx += (float) 0.5f * diffy;
			camera->roty += (float) 0.5f * diffx;
		} else if (buttons[1]) {
			camera->tx += (float) 0.05f * diffx;
			camera->ty -= (float) 0.05f * diffy;
		}
		glutPostRedisplay();
}

/*
 * Mouse Clicks
 *
 * Left click - rotate the scene
 * Middle click - pan
 * Right click - zoom in and out
 */
void mouse (int b, int s, int x, int y){
	lastx = x;
		lasty = y;

		bspline->deselectPoint();

		// This one is for selecting a control point and changing its position.
		if(b == GLUT_LEFT_BUTTON && s == GLUT_DOWN
				&& (key_held == MOVE_ALONG_X || key_held == MOVE_ALONG_Y || key_held == MOVE_ALONG_Z)){
			bspline->selectPoint(x, y);
			buttons[0] = ((GLUT_DOWN == s) ? 1 : 0);
		}
		else if(b == GLUT_LEFT_BUTTON){
			buttons[0] = ((GLUT_DOWN == s) ? 1 : 0);
		}
		else if(b == GLUT_MIDDLE_BUTTON){
			buttons[1] = ((GLUT_DOWN == s) ? 1 : 0);
		}
		else if(b == GLUT_RIGHT_BUTTON){
			buttons[2] = ((GLUT_DOWN == s) ? 1 : 0);
		}

		glutPostRedisplay();
}

/*
 * Keys
 */
void G308_keyboardListener(unsigned char key, int x, int y) {

	/*
	 * Particle Wind Controls
	 */

	float* wind = new float [3];
	wind[0] = 0; wind[1] = 0; wind[2] = 0;

	if(key == 'i')
	{
		wind[1] = 0.1f;
		particeEmitter->applyWind(wind);

	}

	if(key == 'k')
	{
		wind[1] = -0.1f;
		particeEmitter->applyWind(wind);

	}

	if(key == 'j')
	{
		wind[0] = -0.1f;
		particeEmitter->applyWind(wind);

	}

	if(key == 'l')
	{
		wind[0] = 0.1f;
		particeEmitter->applyWind(wind);

	}

	if(key == 'g'){
		if(particeEmitter->isGravityOn())
			particeEmitter->turnGravityOff();
		else
			particeEmitter->turnGravityOn();
	}

	if(key == 'e')
		for(int i = 0 ; i < 50 ; i ++)
			particeEmitter->emit();


//	if(key == '4')
//		camAngle += 0.05f;
//
//	if(key == '6')
//		camAngle -= 0.05f;
//
//	if(key == '8')
//		camHeight += 1;
//
//	if(key == '2')
//		camHeight -= 1;

	// b,n,m decide which coordinate of the control point to change.
	key_held = 0;
	if (key == 'b') {
		animate = animate == 1? 0 : 1;
		key_held = MOVE_ALONG_X;
	}
	else if (key == 'n') {
		key_held = MOVE_ALONG_Y;
	}
	else if (key == 'm') {
		key_held = MOVE_ALONG_Z;
	}

	if(key == 'p'){
		// Prompt for coordinates for the new point
		float x,y,z;
		printf("\nAdd point: Enter coordinates x y z: ");
		if(scanf("%f %f %f", &x, &y, &z) == 3){
			bspline->addPoint(x, y, z);
			glutPostRedisplay();
		}
	}
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


// Set View Position
void G308_SetLight() {

	/*
	 * Ambient
	 */
	GLfloat ambientG[] = { 1, 1, 1, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientG);

	/*
	 * Point Light
	 */
	GLfloat positionPoint[] = { 0, 0, 0, 1 };
	float diffusePoint[] = { 1, 1, 1, 1 };
	float specularPoint[] = { 1, 1, 1, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, positionPoint);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffusePoint);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularPoint);


}

