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
#include "Skeleton.h"
#include "quaternion.h"
#include "G308_ImageLoader.h"
#include "ParticleEmitter.h"
#include "Collision.h"
#include "Ball.h"
#include "Cube.h"
#include "GJK.h"

#include "Camera.h"
#include "BSpline.h"
#include "ControlPoint.h"
#include "Shape.h"
#include "FileReader.h"

#define NUM_KNOTS 8
#define NUM_POINTS 4

// Menu items
enum MENU_TYPE {
	CAMERA_ORIGIN, CAMERA_TANGENT, SIMPLE, SKELETON
};

// Assign a default value
MENU_TYPE menu_choice = SIMPLE;
bool choiceChanged = false;
bool lightsOn = true;

GLuint g_mainWnd;
GLuint g_nWinWidth = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

G308_Point cameraPos = {0,0,50};
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
void createBalls();
void createCubes();

float camAngle;
float camHeight;
int animate = 0;
int maxBalls;

/*
 * Particle Emitter
 */
ParticleEmitter* particeEmitter;
Collision* collision;
G308_Geometry* geometry = NULL;
G308_Geometry* second = NULL;
Ball** balls = NULL;
int numGeo, hitCount = 0;

Camera* camera = NULL;
BSpline* bspline = NULL;
Shape* shape = NULL;
Skeleton* skeleton = NULL;
FileReader* fReader = NULL;

Cube** cubes = NULL;
float test = 6.0f;

int key_held = 0;

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

	G308_init();

	particeEmitter = new ParticleEmitter();
	float* v = new float[3];
	v[0] = 0;
	v[1] = 1;
	v[2] = 0;
	particeEmitter->setVector(v);

	collision = new Collision();

	createBalls();
	createCubes();

	geometry = new G308_Geometry();
	geometry->ReadOBJ("teddy.obj");
	geometry->CreateGLPolyGeometry();
	geometry->CreateGLWireGeometry();

	second = new G308_Geometry();
	second->ReadOBJ("cube.obj");
	second->CreateGLPolyGeometry();
	second->CreateGLWireGeometry();

	loadTexture("sprite2.png", 1);

	camAngle = 0;
	camHeight = 0;

	bspline = new BSpline();
	bspline->init();
	shape = new Shape();
	if(argc > 1){
		skeleton = new Skeleton();
		fReader = new FileReader(skeleton->root);
		fReader->readASF(argv[1]);
	}

	/* Set the menu */
	glutCreateMenu(menu);
	glutAddMenuEntry("Simple shape", SIMPLE);
	glutAddMenuEntry("Camera - origin", CAMERA_ORIGIN);
	glutAddMenuEntry("Camera - tangent", CAMERA_TANGENT);
	glutAddMenuEntry("Skeleton", SKELETON);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

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
	test -= 0.01;
	Collision* c = new Collision;
	GJK* gjk = new GJK;
	/*
	 * Simulate a frame in the particle emitter
	 */
	particeEmitter->tick();
	particeEmitter->emit();
	for(int i = 0; i < maxBalls; i ++){
		balls[i]->tick();
	}
	for(int i = 0; i < maxBalls; i++){
		for(int j = i+1; j < maxBalls; j++){
			if(i != j && c->checkIfCollidedBalls(balls[i], balls[j])){
				c->collisionBall(1, balls[i]->mass, balls[j]->mass, 2, 2, balls[i]->position, balls[j]->position, balls[i]->velocity, balls[j]->velocity);
			}
		}
	}
	for(int i = 0; i < maxBalls; i++){
		particeEmitter->collideWithBalls(balls[i], c);
	}

	for(int i = 0; i < maxBalls; i++){
		for(int j = 0; j < 5; j++){
			c->checkCollision(1.0, cubes[j], balls[i]);
		}
	}

	//
	// If animation is on, move the object one step.
	if(animate == 1){
		if(choiceChanged){
			choiceChanged = false;
			bspline->resetFrame();
		}
		switch(menu_choice){
		case SIMPLE:
			shape->move(bspline);
			break;
		case CAMERA_ORIGIN:
			camera->lookAt(bspline, 1, (double)g_nWinWidth, (double)g_nWinHeight, &cameraPos);
			break;
		case CAMERA_TANGENT:
			camera->lookAt(bspline, 2, (double)g_nWinWidth, (double)g_nWinHeight, &cameraPos);
			break;
		case SKELETON:
			if (skeleton != NULL) skeleton->move(bspline);
			break;
		}

	}

	glutPostRedisplay();
	delete c;

	G308_Point* geometryPoint = new G308_Point[geometry->m_nNumPoint];
	for(int i = 0; i < geometry->m_nNumPoint; i++){
		geometryPoint[i] = G308_Point();
		geometryPoint[i].x = geometry->m_pVertexArray[i].x + test;
		geometryPoint[i].y = geometry->m_pVertexArray[i].y;
		geometryPoint[i].z = geometry->m_pVertexArray[i].z;
	}

	G308_Point* secondPoint = new G308_Point[second->m_nNumPoint];
	for(int i = 0; i < second->m_nNumPoint; i++){
		secondPoint[i] = G308_Point();
		secondPoint[i].x = second->m_pVertexArray[i].x - 6.0f;
		secondPoint[i].y = second->m_pVertexArray[i].y;
		secondPoint[i].z = second->m_pVertexArray[i].z;
	}

	//gjk->shapesIntersect(geometryPoint, secondPoint, geometry->m_nNumPoint, second->m_nNumPoint);
}


// Init Light and Camera
void G308_init() {
	camera = new Camera();
	camera->setInitialCameraPosition((double) g_nWinWidth, (double) g_nWinHeight);
	G308_SetLight();
}

// Display call back
void G308_display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glShadeModel(GL_SMOOTH);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("%s\n", gluErrorString(err));
	}


	glPushMatrix();

	camera->rotateCamera(&cameraPos);

	/*
	 * Draw stuff here!
	 */

	if (bspline->hasChanged) {
		bspline->hasChanged = false;
		bspline->recalculate();
	}


	// Draw the spline with the control points
	bspline->draw(GL_SELECT);

	// Draw the shape that goes along the spline if animation is moving.
	if(animate == 1){
		switch (menu_choice) {
		case SIMPLE:
			shape->draw();
			break;
		case SKELETON:
			if (skeleton != NULL)
				skeleton->display();
			break;
		case CAMERA_ORIGIN:
			break;
		case CAMERA_TANGENT:
			break;
		default:
			break;
		}
	}

	/*
	 * Draw the particle emmiter and it's particles
	 */
	for(int i = 0; i < maxBalls; i ++) balls[i]->renderBall();
	glPushMatrix();
	glColor3f(0.0f, 0.5f, 0.5f);
	glTranslatef(test, 0.0f, 0.0f);
	glScalef(0.1f, 0.1f, 0.1f);
	//geometry->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.5f);
	glTranslatef(-6.0f, 0.0f, 0.0f);
	second->RenderGeometry();
	glPopMatrix();

	particeEmitter->renderParticles();

	//for(int j = 0; j < 5; j++) cubes[j]->renderCube();

	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
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

	//
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
	//
	// Change the view point of the scene (rotate and pan).
	else if (buttons[0]) {
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

	//
	// This one is for selecting a control point and changing its position.
	if(b == GLUT_LEFT_BUTTON && s == GLUT_DOWN
			&& (key_held == MOVE_ALONG_X || key_held == MOVE_ALONG_Y || key_held == MOVE_ALONG_Z)){

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		bspline->selectPoint(x, y);
		buttons[0] = ((GLUT_DOWN == s) ? 1 : 0);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	}
	// Remember that left button is pressed.
	else if(b == GLUT_LEFT_BUTTON){
		buttons[0] = ((GLUT_DOWN == s) ? 1 : 0);
	}
	// Remember that middle button is pressed.
	else if(b == GLUT_MIDDLE_BUTTON){
		buttons[1] = ((GLUT_DOWN == s) ? 1 : 0);
	}

	glutPostRedisplay();
}

// Menu items
void menu(int item) {
	menu_choice = (MENU_TYPE)item; // CAMERA_ORIGIN, CAMERA_TANGENT, SIMPLE, or SKELETON
	choiceChanged = true;
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
		for(int i = 0 ; i < 100 ; i ++)
			particeEmitter->emit();


	key_held = 0;
	//
	// b,n,m decide which coordinate of the control point to change.
	if (key == 'b') { key_held = MOVE_ALONG_X; }
	else if (key == 'n') { key_held = MOVE_ALONG_Y; }
	else if (key == 'm') { key_held = MOVE_ALONG_Z; }
	//
	// Toggle the animation of objects along a spline.
	else if(key == 'a'){ animate = animate == 1? 0 : 1; }
	//
	// Prompt for coordinates for the new point
	else if(key == 'p'){
		float x,y,z;
		printf("\nAdd point: Enter coordinates x y z: ");
		if(scanf("%f %f %f", &x, &y, &z) == 3){
			bspline->addPoint(x, y, z);
			glutPostRedisplay();
		}
	}
	//
	// Toggle the display of times for each control point.
	else if(key == 's'){
		bspline->infoDisplay = !bspline->infoDisplay;
		glutPostRedisplay();
	}
	//
	// Change the times an object passes through each control point.
	else if(key == 'x'){
		bspline->promptForNewTimes();
		bspline->readNewTimes();
		glutPostRedisplay();
	}
	//
	// Change the time interval in takes an object to move from start to finish of a spline.
	else if(key == 'z'){
		bspline->promptForNewInterval();
		bspline->readNewInterval();
		glutPostRedisplay();
	}
	else if(key == 'c'){
		bspline->printCoordinates();
	}
	//
	// Move camera back to its original position.
	else if (key == 'v') {
		camera->resetCamera(&cameraPos);
		camera->setInitialCameraPosition((double) g_nWinWidth, (double) g_nWinHeight);
		glutPostRedisplay();
	}
	//
	// Camera zoom in and out
	else if(key == '6'){
		camera->zoom -= 1;
	}
	else if(key == '9'){
		camera->zoom += 1;
	}
	else if(key == '1'){
		lightsOn = !lightsOn;
		if(lightsOn){
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHT1);
		}
		else{
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
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
	GLfloat ambientG[] = { 1, 1, 1, 0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientG);

	/*
	 * Point Light
	 */
	GLfloat positionPoint[] = { 0, 0, 0, 1 };
	float diffusePoint[] = { 0.1f, 0.1f, 0.1f, 1 };
	float specularPoint[] = { 0.1f, 0.1f, 0.1f, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, positionPoint);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffusePoint);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularPoint);

	GLfloat directionPosision[] = { 0.0f, 1.0f, 1.0f, 0.0f };
	GLfloat directionDiffuse[] = { 0.3f, 0.3f, 0.3f, 0.0f };
	GLfloat directionSpecular[] = { 0.3f, 0.3f, 0.3f, 0.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, directionPosision);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, directionDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, directionSpecular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void createBalls(){
	maxBalls = 5;
	balls = new Ball*[maxBalls];

	for(int i = 0; i < maxBalls; i++){
		if(i == 0){
			float v[3] = {-0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {3.0f, 0.0f, 1.0f};

			balls[i] = new Ball(p, v, false);
		}
		if(i == 1){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-6.0f, -4.0f, 0.0f};

			balls[i] = new Ball(p, v, true);
		}
		if(i == 2){
			float v[3] = {-0.1f, 0.1f, 0.1f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-2.0f, -4.0f, 0.0f};

			balls[i] = new Ball(p, v, false);
		}
		if(i == 3){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-6.0f, -0.0f, 0.0f};

			balls[i] = new Ball(p, v, false);
		}
		if(i == 4){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {3.0f, 2.0f, 0.0f};

			balls[i] = new Ball(p, v, true);
		}
	}
}

void createCubes(){
	cubes = new Cube*[5];
	cubes[0] = new Cube(0, -255, 0, 500, 500);
	cubes[1] = new Cube(-260, 0, 0, 500, 500);
	cubes[2] = new Cube(260, 0, 0, 500, 500);
	cubes[3] = new Cube(0, 0, -260, 500, 500);
	cubes[4] = new Cube(0, 0, 260, 500, 500);
}
