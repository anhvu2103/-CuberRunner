/*
CubeRunner.cpp -  Cube runner game in which user avoids obstacles using keyboard input. Key functionality allows for pausing, new game,
camera angle change. 

A/D to move player
arrows keys to move camera
"/" to reset camera
"n" for new game
"p" to pause/unpause
spacebar to start

*/

#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>
#include <stack>
#include <time.h>
#include <algorithm>

using namespace std;

#define playerSpeed 0.1
#define boxSpeed 0.2
#define eyeXVar 0.0 
#define eyeYVar 4.0
#define eyeZVar 3.0

//Window varialbes
const GLint windowX = 1000;
const GLint windowY = 800;

//Initialize material properties
GLfloat mat_ambient[] = { 1.0 , 1.0 , 1.0 , 1.0 };
GLfloat mat_diffuse[] = { 0.01 , 0.01 , 0.01 , 1.0 };
GLfloat mat_specular[] = { 0.01 , 0.01 , 0.01 , 1.0 };
GLfloat mat_emission[] = { 1.0 , 1.0 , 1.0 , 0.0 };
GLfloat mat_shininess[] = { 32 };


//Game state variables
GLboolean paused = true;
GLboolean collisions = true;
GLboolean collide = false;
GLint score = 0;

//Board Settings
GLfloat floatHeight = 0;
GLint groundSize = 120;

//Keyboard control variables
GLboolean upPressed = false;
GLboolean downPressed = false;
GLboolean leftPressed = false;
GLboolean rightPressed = false;
GLboolean aPressed = false;
GLboolean dPressed = false;

//Player Variables
GLfloat playerWidth = 0.25;
GLfloat playerZOffset = -2.0;
GLint playerResolution = 30;
GLfloat playerShiftSpeed = playerSpeed;

//Box variables
const GLint numboxes = 80;
GLfloat boxes[numboxes][3];
GLint boxcolors[numboxes][3];
GLint xpos = 0;
GLint zpos = 2;
GLint boxLaneWidth = 60;
GLfloat boxSize = 1;
GLfloat boxShiftSpeed = boxSpeed;

//Rotate variables
GLfloat screenRot = 0;
GLfloat rotateDeg = 1.2;
GLfloat rotateTolerance = 8;

//Light position
GLfloat xl = 0, yl = 4, zl = 0;

//Initial camera position
GLfloat eyeX = eyeXVar, eyeY = eyeYVar, eyeZ = eyeZVar;
GLint zFar = -groundSize / 2;

//Camera pan tolerance
GLfloat eyeYTolerance = 10;
GLfloat eyeXTolerance = 10;

void InitializeBoxes() {
	//Seed the random generator
	srand(time(0));

	//Randomly generate horizontal position and set the vertical out of viewport accoring to box number,
	for (int i = 0; i < numboxes; i++) {
		GLfloat pos = rand() % (boxLaneWidth)-boxLaneWidth / 2;
		boxes[i][xpos] = pos;
		boxes[i][zpos] = zFar * i / numboxes + zFar / 3;
	}
}

void Fog() {

	//Fog properties
	glEnable(GL_FOG);

	GLfloat fogColor[4] = { 0.4, 0.3, 0.4, 1.0 };

	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.0455);
	
}

void myInit() {
	// clear color, it will be the "background"
	glClearColor(0.4, 0.3, 0.4, 1.0);

	// enable depth test
	glEnable(GL_DEPTH_TEST);

	// enable lighting and the first light source
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// let OpenGL normalize the normal vectors
	glEnable(GL_NORMALIZE);

	// interpolate the colors on the surface of the polygon "smoothly"
	glShadeModel(GL_SMOOTH);

	// define properties of the light source, at a point
	GLfloat light_position[] = { xl,yl,zl,1 };
	GLfloat light_diffuse[] = { 1.0,1.0,1.0,1.0 };
	GLfloat light_ambient[] = { 0.2,0.2,0.2,1.0 };
	GLfloat light_specular[] = { 1.0,1.0,1.0,1.0 };

	// assign the above properties to light source 0
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	//Loads fog
	Fog();

	//Load boxes with random posititions into boxes array
	InitializeBoxes();
}

void PlayerMaterial() {
	// set the material property for the player object

	mat_ambient[0] = 1; mat_ambient[1] = 1; mat_ambient[2] = 1;
	mat_diffuse[0] = 0.5; mat_diffuse[1] = 0.5; mat_diffuse[2] = 0.5;
	mat_specular[0] = 0.4; mat_specular[1] = 0.4; mat_specular[2] = 0.4;
	mat_emission[0] = 1; mat_emission[1] = 1; mat_emission[2] = 1;
	mat_shininess[0] = 25.6;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FALSE, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void DrawPlayer() {

	//get player material properties
	PlayerMaterial();

	//Draw player and translate towards camera
	glPushMatrix();
	glTranslatef(0, playerWidth + floatHeight, playerZOffset);
	glutSolidSphere(playerWidth, playerResolution, playerResolution);
	glPopMatrix();
}

void DrawRobot() {

	PlayerMaterial();

	glPushMatrix();

	//glTranslatef(0, 1, playerZOffset / 2);
	//glRotatef(-screenRot*1.4, 0, 0, 1);
	//glRotatef(-90, 1, 0, 0);
	//glScalef(playerWidth, playerWidth*0.75, playerWidth*0.75);

	glTranslatef(0, playerWidth + floatHeight, playerZOffset);
	glRotatef(-screenRot * 1.4, 0, 0, 1);
	
	//Draw Torso
	glPushMatrix();
	glTranslatef(0, 1.7, 0.0);
	//glScalef(6.0, 3.0, 3.0);
	glScalef(2.0, 4.0, 2.0);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();

	//Draw Head
	glPushMatrix();
	glTranslatef(0.0, 3.5, 0.0);
	glScalef(1.5, 1.5, 1.5);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();
	
	//Draw Left leg
	glPushMatrix();
	glTranslatef(-1.7, 0.0, 0.0);
	glScalef(1.5, 3.0, 1.5);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();

	//Draw right leg
	glPushMatrix();
	glTranslatef(1.7, 0.0, 0.0);
	glScalef(1.5, 3.0, 1.5);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();
	
	//Draw left arm
	glPushMatrix();
	glTranslatef(-3.0, 4.0, 0.0);
	glScalef(1.0, 3.0, 1.0);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();
	
	//Draw right arm
	glPushMatrix();
	glTranslatef(3.0, 4.0, 0.0);
	glScalef(1.0, 3.0, 1.0);
	glutSolidSphere(1.0, playerResolution, playerResolution);
	glPopMatrix();
	
	glPopMatrix();
}

void BoxMaterial() {
	//Material properties for boxes

	mat_ambient[0] = 1; mat_ambient[1] = 0.1; mat_ambient[2] = 0.8;
	mat_diffuse[0] = 0.5; mat_diffuse[1] = 0.3; mat_diffuse[2] = 0.5;
	mat_specular[0] = 0.4; mat_specular[1] = 0.3; mat_specular[2] = 0.4;
	//mat_emission[0] = 1; mat_emission[1] = 1; mat_emission[2] = 1;
	mat_shininess[0] = 25.6;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FALSE, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void DrawBoxes() {

	//get boxes material properties
	BoxMaterial();

	// draw boxes
	for (int i = 0; i < numboxes; i++) {
		//glLineWidth(4.0);
		glPushMatrix();
		//glScalef(1, 2.5, 1);
		glTranslatef(boxes[i][xpos], boxSize / 2 + floatHeight, boxes[i][zpos]);
		//glutWireCube(boxSize);
		glutSolidCube(boxSize);
		glPopMatrix();
	}

}

void GroundMaterial() {
	// set the material properties for the bottom wall

	mat_ambient[0] = 0.0; mat_ambient[1] = 0.0; mat_ambient[2] = 0.0;
	mat_diffuse[0] = 0.2; mat_diffuse[1] = 0.2; mat_diffuse[2] = 0.2;
	mat_specular[0] = 0.2; mat_specular[1] = 0.5; mat_specular[2] = 0.2;
	//mat_emission[0] = 0; mat_emission[1] = 0; mat_emission[2] = 0;
	mat_shininess[0] = 5;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void DrawGround() {

	//get ground material properties
	GroundMaterial();

	// render the ground
	glBegin(GL_POLYGON);
	glVertex3f(-groundSize, 0, zFar); //back left
	glVertex3f(-groundSize, 0, groundSize); //front left
	glVertex3f(groundSize, 0, groundSize); //front right
	glVertex3f(groundSize, 0, zFar); // back right
	glEnd();
}

void DrawAxes() {
	//axes which follow the cube used for debugging

	glLineWidth(8);
	glBegin(GL_LINES);
	//x axis
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);
	// y axis
	glVertex3f(0, 0, 0);
	glVertex3f(0, 5, 0);
	// z axis
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 5);
	glEnd();
}

void InitializePropertiesMatrix() {
	//create and prepare material properties matrix
	GLint i;
	GLfloat m[16];
	for (i = 0; i <= 15; i++) { m[i] = 0.0; }
	m[0] = 1.0;   m[5] = 1.0;   m[10] = 1.0;   m[7] = -1.0 / yl;
}

void NewBoxes(int box) {
	//Assign a new random x-position and resets z-position
	GLfloat pos = rand() % (boxLaneWidth)-boxLaneWidth / 2;
	boxes[box][xpos] = pos;
	boxes[box][zpos] = zFar;
}

void RotateScreen() {

	if (!aPressed && !dPressed) {
		if (screenRot != 0) {
			GLfloat delta = 0 + screenRot;
			screenRot -= delta/(rotateTolerance/rotateDeg);
		}
	}
	glRotatef(screenRot, 0, 0, 1);
}

void XShift(GLfloat shift) {
	//moves boxes sideways
	for (int i = 0; i < numboxes; i++) {
		boxes[i][xpos] += shift;
	}
}

void ZShift(GLfloat shift) {
	//Shifts all boxes toward user
	for (int i = 0; i < numboxes; i++) {
		boxes[i][zpos] += shift;
		if (boxes[i][zpos] > eyeZ) { NewBoxes(i); }
	}
}

void drawStrokeText(const char *string, void *font, float x, float y, float z) {
	//Draws score text on the screen
	const char *c;
	glPushMatrix();
	glTranslatef(x, y, z);

	for (c = string; *c != '\0'; c++) {
		glutStrokeCharacter(font, *c);
	}

	glPopMatrix();
}

void Display() {
	//Main display func

	InitializePropertiesMatrix(); //Loads environment properties for lighting

	//Clears buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Sets up projection matrices
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Sets up camera
	gluLookAt(eyeX, eyeY, eyeZ, 0, 0, -10, 0, 1, 0);

	//Rotates when turns
	RotateScreen();

	// Render all boxes on screen
	DrawBoxes();

	// Render Player
	DrawPlayer();
	//DrawRobot();

	// Render ground
	DrawGround();

	//Axes - used in creation
	//DrawAxes();

	//Draws score on the screen
	//glColor3f(0, 1, 0);
	//drawStrokeText("Score: ", GLUT_STROKE_ROMAN, 0, 2, 1);

	glutSwapBuffers();
}

void Collision() {
	//Checks for collisions by checking overlapping box corners
	for (int box = 0; box < numboxes; box++) {
		if (abs(boxes[box][xpos]) - boxSize / 2 < playerWidth) {
			if (boxes[box][zpos] + boxSize / 2 > playerZOffset - playerWidth
				&& boxes[box][zpos] - boxSize / 2 < playerZOffset + playerWidth) {
				collide = true;
			}
		}
	}
}

void KeyPresses() {
	//updates position variables of boxes based off user keyboard input
	if (upPressed) {
		eyeY += 0.2;
		if (eyeY > eyeYTolerance) { eyeY = eyeYTolerance; }
	}

	if (downPressed) {
		eyeY -= 0.2;
		if (eyeY < 2) { eyeY = 2; }
	}

	if (leftPressed) {
		eyeX -= 0.2;
		if (eyeX < -eyeXTolerance) { eyeX = -eyeXTolerance; }
	}

	if (rightPressed) {
		eyeX += 0.2;
		if (eyeX > eyeXTolerance) { eyeX = eyeXTolerance; }
	}

	if (dPressed) {
		XShift(-playerShiftSpeed);
		screenRot += rotateDeg;
		if (screenRot > rotateTolerance) { screenRot = rotateTolerance; }
	}

	if (aPressed) {
		XShift(playerShiftSpeed);
		screenRot -= rotateDeg;
		if (screenRot < -rotateTolerance) { screenRot = -rotateTolerance; }
	}

}

void GameOver() {
	//When collision occurs, becomes idle func so user can move 
	//camera around still and wont freeze the program
	KeyPresses();
	glutPostRedisplay();
}

void Game() {

	//Main idle game function that constantly checks game is not paused, handles key presses, 
	//moves boxes towards player, checks collision, counts score, prints score, and redisplays

	if (!paused) {

		//Handle user input, update variables
		KeyPresses();

		//Move boxes towards us
		ZShift(boxShiftSpeed);

		//Check for player/box collision
		if (collisions) {
			Collision();
			if (collide) {
				cout << "Whoops! Your score is " << score << endl;
				glutIdleFunc(GameOver);
				playerShiftSpeed = boxShiftSpeed = 0;
			}
		}

		//Increments and prints the users score
		if (!collide) {
			score++;
			if (score % 1000 == 0) { cout << "Score - " << score << endl; cout << "Increasing speed!" << endl; }
		}

		//Prints score on increments
		if (score % 1000 == 0) {
			boxShiftSpeed *= 1.2;
			playerShiftSpeed *= 1.2;
		}


		//Redisplay
		glutPostRedisplay();
	}
}

void NewGame() {
	//Rests to default game variables to create a new game, reinitializes boxes
	InitializeBoxes();
	score = 0;
	collide = false;
	collisions = true;
	boxShiftSpeed = boxSpeed;
	playerShiftSpeed = playerSpeed;
	glutIdleFunc(Game);
}

void Keyboard(unsigned char key, int x, int y) {

	//Key press function that handles when keys are pressed

	if (key == 'd') { dPressed = true; }

	if (key == 'a') { aPressed = true; }

	if (key == 'n') { NewGame(); }

	if (key == 'p') { paused = !paused; }

	if (key == '/') eyeX = eyeXVar, eyeY = eyeYVar, eyeZ = eyeZVar;

	if (key == 'c') collisions = !collisions;

	if (key == ' ') paused = false;
}

void KeyboardUp(unsigned char key, int x, int y) {

	//Keyboard func that handles when keys are released

	if (key == 'd') { dPressed = false; }

	if (key == 'a') { aPressed = false; }
}

void SpecialInput(int key, int x, int y) {
	//Handles arrow key press
	if (GLUT_KEY_UP == key)
		upPressed = true;

	if (GLUT_KEY_DOWN == key)
		downPressed = true;

	if (GLUT_KEY_LEFT == key)
		leftPressed = true;

	if (GLUT_KEY_RIGHT == key)
		rightPressed = true;
}

void SpecialInputUp(int key, int x, int y)
{
	//Handles arrow key releases
	if (GLUT_KEY_UP == key)
		upPressed = false;

	if (GLUT_KEY_DOWN == key)
		downPressed = false;

	if (GLUT_KEY_LEFT == key)
		leftPressed = false;

	if (GLUT_KEY_RIGHT == key)
		rightPressed = false;
}

void MyReshape(int w, int h)
{
	//reshape func that changes perspective based on window size
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)w / h, 1, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void main(int argc, char **argv)
{
	//Main method

	glutInit(&argc, argv);

	//double buffering and depth for smooth 3D implementation
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowX, windowY);           //window size
	glutCreateWindow("Cube Runner");

	//Passes openGL functions to glut
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(Display);

	//Regular keyboard functions
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);

	//Arrow key functions
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInputUp);

	//Idle function
	glutIdleFunc(Game);

	glEnable(GL_DEPTH_TEST); //3D depth surface test

	myInit();

	printf("Directions:\n A and D to move character\n Arrow Keys to move Camera\n \"n\" for new game \n \"p\" for pause\n \"\/\" to reset camera");
	printf("\n\nPress spacebar to start");

	glutMainLoop(); //Starts game
}