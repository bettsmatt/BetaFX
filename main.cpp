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
#include "Particle.h"

#define NUM_KNOTS 8
#define NUM_POINTS 4

// Menu items
enum MENU_TYPE {
	CAMERA, SIMPLE, SKELETON, REMOVE_PARTICLES, REMOVE_SUNS, SPAWN_SUN, SPAWN_SUNS
};


// Assign a default value
MENU_TYPE menu_choice = SIMPLE;
bool choiceChanged = false;
bool lightsOn = true;
bool splineOn = true;

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
int currentBalls;

bool alive;
int deathCount;

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

float* pos;

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


	particeEmitter = new ParticleEmitter(&cameraPos);

	G308_init();

	float* v = new float[3];
	v[0] = 0;
	v[1] = 1;
	v[2] = 0;
	particeEmitter->setVector(v);

	collision = new Collision();

	createCubes();

	alive = false;
	deathCount = 0;
	geometry = new G308_Geometry();
	geometry->ReadOBJ("testp.obj");
	geometry->CreateGLPolyGeometry();
	geometry->CreateGLWireGeometry();

	second = new G308_Geometry();
	second->ReadOBJ("testp.obj");
	second->CreateGLPolyGeometry();
	second->CreateGLWireGeometry();

	loadTexture("sprite2.png", 1);

	pos = new float[3];
	pos[0] = 0;pos[1] = 0;pos[2] = 0;

	maxBalls = 20;
	balls = new Ball*[maxBalls];
	currentBalls = 0;

	camAngle = 0;
	camHeight = 0;

	bspline = new BSpline();
	bspline->init();
	shape = new Shape();
	if(argc > 1){
		skeleton = new Skeleton();
		fReader = new FileReader(skeleton->root);
		fReader->readASF(argv[1]);
		if(argc > 2){
			fReader->readAMC(argv[2]);
			skeleton->amcFileProvided = true;
			skeleton->totalFrameNum = fReader->totalFrameNum;
		}
	}

	/* Set the menu */
	glutCreateMenu(menu);
	glutAddMenuEntry("Simple shape", SIMPLE);
	glutAddMenuEntry("Camera", CAMERA);
	glutAddMenuEntry("Skeleton", SKELETON);

	glutAddMenuEntry("Remove Particles", REMOVE_PARTICLES);
	glutAddMenuEntry("Remove Suns", REMOVE_SUNS);
	glutAddMenuEntry("Spawn Suns", SPAWN_SUNS);
	glutAddMenuEntry("Spawn Sun", SPAWN_SUN);

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
	free(t);
}

/*
 * Do stuff, in the background
 */
void tick (){
	if(alive) test -= 0.01;
	Collision* c = new Collision;
	GJK* gjk = new GJK;
	/*
	 * Simulate a frame in the particle emitter
	 */
	particeEmitter->tick();
	for(int i = 0; i < currentBalls; i ++){
		balls[i]->tick();
	}
	for(int i = 0; i < currentBalls; i++){
		for(int j = i+1; j < currentBalls; j++){
			if(i != j && c->checkIfCollidedBalls(balls[i], balls[j])){
				c->collisionBall(0.99f, balls[i]->mass, balls[j]->mass, 2, 2, balls[i]->position, balls[j]->position, balls[i]->velocity, balls[j]->velocity);
			}
		}
	}
	for(int i = 0; i < currentBalls; i++){
		particeEmitter->collideWithBalls(balls[i], c);
	}

	for(int i = 0; i < currentBalls; i++){
		for(int j = 0; j < 5; j++){
			c->checkCollision(0.9f, cubes[j], balls[i]);
		}
	}

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
		case CAMERA:
			camera->lookAt(bspline, 2, (double)g_nWinWidth, (double)g_nWinHeight, &cameraPos);
			break;
		case SKELETON:
			if (skeleton != NULL) skeleton->move(bspline);
			break;
		default:
			break;
		}

	}

	glutPostRedisplay();
	delete c;

	G308_Point* geometryPoint = new G308_Point[geometry->m_nNumPoint];
	for(int i = 0; i < geometry->m_nNumPoint; i++){
		geometryPoint[i] = G308_Point();
		geometryPoint[i].x = geometry->m_pVertexArray[i].x + test;
		geometryPoint[i].y = geometry->m_pVertexArray[i].y + test/2;
		geometryPoint[i].z = geometry->m_pVertexArray[i].z;
	}

	G308_Point* secondPoint = new G308_Point[second->m_nNumPoint];
	for(int i = 0; i < second->m_nNumPoint; i++){
		secondPoint[i] = G308_Point();
		secondPoint[i].x = second->m_pVertexArray[i].x - 5.0f;
		secondPoint[i].y = second->m_pVertexArray[i].y;
		secondPoint[i].z = second->m_pVertexArray[i].z;
	}

	bool hit = gjk->shapesIntersect(geometryPoint, secondPoint, geometry->m_nNumPoint, second->m_nNumPoint);

	G308_Point max1 = gjk->getMax1();
	max1.x += 6.0f - test;
	max1.y += 3.0f - test/2;

	delete[] pos;
	pos = new float[3]; pos[0] = max1.x; pos[1] = max1.y; pos[2] = max1.z;

	G308_Point temp = geometry->FindNormal(max1);
	float* test = new float[3];
	test[0] = -1; test[1] = 0; test[2] = 0;

	if(hit){
		alive = false;
		if(deathCount < 10) particeEmitter->cloud(1000,10);
		if(deathCount == 10) particeEmitter->removeParticles();
		deathCount++;
	}

	delete gjk;
	delete[] geometryPoint;
	delete[] secondPoint;
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
	if(splineOn) bspline->draw(GL_SELECT);

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
		case CAMERA:
			break;
		default:
			break;
		}
	}

	/*
	 * Draw the particle emmiter and it's particles
	 */
	for(int i = 0; i < currentBalls; i ++) balls[i]->renderBall();

	if(alive){
		glPushMatrix();
		glColor3f(0.0f, 0.5f, 0.5f);
		glTranslatef(test, test/2, 0.0f);
		geometry->RenderGeometry();
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.5f);
		glTranslatef(-5.0f, 0.0f, 0.0f);
		second->RenderGeometry();
		glPopMatrix();

		glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.5f);
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.1f, 5, 5);
		glPopMatrix();
	}

	glPushMatrix();
	glColor3f(0.0f, 0.5f, 0.5f);
	glTranslatef(-1000000000, -1000000000,-1000000000);
	geometry->RenderGeometry();
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

	switch(item){
	case REMOVE_PARTICLES:
		particeEmitter->removeParticles();
		break;
	case REMOVE_SUNS:
		particeEmitter->removeSuns();
		break;
	case SPAWN_SUN:
		particeEmitter->spawnSun();
		break;
	case SPAWN_SUNS:
		particeEmitter->spawnSuns();
		break;
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

	// Spawn a cloud of particles
	if(key == 'c')
	{
		particeEmitter->cloud(1000,10);
	}

	// Remove all suns from the emitter
	if(key == 'i')
	{
		particeEmitter->removeSuns();

	}

	// Remove all particles from th emitter
	if(key == 'k')
	{
		particeEmitter->removeParticles();

	}
	if(key == 'g'){
		if(particeEmitter->isGravityOn())
			particeEmitter->turnGravityOff();
		else
			particeEmitter->turnGravityOn();
	}

	if(key == 'e')
		for(int i = 0 ; i < 1 ; i ++)
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

	else if(key == '-' || key == '='){
		float XHI = 0.1f;
		float XLO = -0.1f;

		float YHI = 0.1f;
		float YLO = -0.1f;

		float ZHI = 0.3f;
		float ZLO = 0.2f;

		float* v = new float[3];
		v[0] = XLO + (float)rand()/((float)RAND_MAX/(XHI-XLO));
		v[1] = YLO + (float)rand()/((float)RAND_MAX/(YHI-YLO));
		v[2] = ZLO + (float)rand()/((float)RAND_MAX/(ZHI-ZLO));

		if(currentBalls == maxBalls) return;

		float p[3] = {0.0f, 6.0f, 0.0f};
		bool special = true;
		if(key == '-') special = false;

		balls[currentBalls] = new Ball(p, v, 1, special);
		currentBalls ++;
	}
	else if(key == '['){
		delete[] balls;
		balls = new Ball*[maxBalls];
		currentBalls = 0;
	}
	else if(key == ']'){
		float* v = new float[3];
		v[0] = 0;
		v[1] = -0.1f;
		v[2] = 0;

		if(currentBalls == maxBalls) return;

		float p[3] = {0.0f, 6.0f, 0.0f};

		balls[currentBalls] = new Ball(p, v, 2, false);
		currentBalls++;

		particeEmitter->cloud(500,2);
	}
	else if(key == '\\'){
		float* v1 = new float[3];
		v1[0] = -0.05f;
		v1[1] = 0;
		v1[2] = 0.02f;

		if(currentBalls == maxBalls) return;

		float p1[3] = {6.0f, 6.0f, -2.0f};

		balls[currentBalls] = new Ball(p1, v1, 1, false);
		currentBalls++;

		float* v2 = new float[3];
		v2[0] = 0.05f;
		v2[1] = 0;
		v2[2] = -0.005;

		float p2[3] = {-6.0f, 6.0f, 2.0f};
		balls[currentBalls] = new Ball(p2, v2, 1.5f, false);
		currentBalls++;

	}
	else if(key == ';'){
		createBalls();
	}
	else if(key == '\''){
		test = 6.0f;
		alive = true;
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
	else if(key == '2'){
		splineOn = !splineOn;
	}
	else if(key == '3'){
		bspline->printCoordinates();
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
	for(int i = 0; i < maxBalls; i++){
		if(i == 0){
			float v[3] = {-0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {3.0f, 0.0f, 1.0f};

			balls[i] = new Ball(p, v, 1, false);
		}
		if(i == 1){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-6.0f, -4.0f, 0.0f};

			balls[i] = new Ball(p, v, 1, true);
		}
		if(i == 2){
			float v[3] = {-0.1f, 0.1f, 0.1f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-2.0f, -4.0f, 0.0f};

			balls[i] = new Ball(p, v, 1, false);
		}
		if(i == 3){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {-6.0f, -0.0f, 0.0f};

			balls[i] = new Ball(p, v, 1, false);
		}
		if(i == 4){
			float v[3] = {0.1f, 0.1f, 0.0f};
			//float v[3] = {0.00f, 0.00f, 0.0f};
			float p[3] = {3.0f, 2.0f, 0.0f};

			balls[i] = new Ball(p, v, 1, true);
		}
	}
	maxBalls = 20;
	currentBalls = 5;
}

void createCubes(){
	cubes = new Cube*[5];
	cubes[0] = new Cube(0, -255, 0, 500, 500);
	cubes[1] = new Cube(-260, 0, 0, 500, 500);
	cubes[2] = new Cube(260, 0, 0, 500, 500);
	cubes[3] = new Cube(0, 0, -260, 500, 500);
	cubes[4] = new Cube(0, 0, 260, 500, 500);
}
