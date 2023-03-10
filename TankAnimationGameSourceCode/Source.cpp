/*********
   CTIS164 - Template Source Program
----------
STUDENT : AHMET EREN ARSLANOÐLU
SECTION : 02
HOMEWORK: HOMEWORK-1
----------
PROBLEMS: -
----------
ADDITIONAL FEATURES:
DYNAMIC WEATHER
RANDOM PLACED STARS
TANKS PLACED DIFFERENTLY IF LEFT HALF OF THE WINDOW CLICKED TANK WILL MOVE TO RIGHT IF RIGHT HALF CLICKED TANK WILL MOVE LEFT
ANIMATED SUN AND CLOUDS
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>


#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

#define MAX 10

#define TIMER_PERIOD  30 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int xT[MAX], yT[MAX];
int clickcnt = 0;
float xS = 1050;
float xC = 400, xC2 = 100, xC3 = -100, xC4 = -300, xC5 = -500, xC6 = 200, xC7 = 600;
//X COORDINATES OF LINES
int xL1 = -550, xL2 = -360, xL3 = -120, xL4 = 120, xL5 = 360, xL6 = 300, xL7 = 350, xL8 = 500;
//decide night or day
bool move = false;

bool drawSky = false;

bool activeTimer = false;

//decide when to exist for each object
bool draw[MAX] = { false };

//decide whether it is reverse or normal tank
bool isReverse[MAX] = { false };


int minX;
int maxX;

//random array to hold stars coordinates
int rand1[100];
int rand2[100];



//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawTank(int x, int y) {

	glColor3f(.2, .4, 0);
	glBegin(GL_QUADS);
	//draw barrel
	glVertex2f(x + 35, y + 7);
	glVertex2f(x + 110, y + 10);
	glVertex2f(x + 105, y + 5);
	glVertex2f(x + 35, y + 2);
	//draw top
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(x + 8, y + 10);
	glVertex2f(x + 35, y + 10);
	glVertex2f(x + 50, y);
	glVertex2f(x + 5, y);
	glEnd();

	//draw body


	glColor3f(.2, .5, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x - 10, y - 20);
	glVertex2f(x + 60, y);
	glVertex2f(x + 75, y - 20);
	glVertex2f(x - 10, y - 20);
	glEnd();

	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(0.1, .1, .1);
	glVertex2f(x - 10, y - 15);
	glVertex2f(x + 80, y - 15);

	glVertex2f(x - 8, y - 5);
	glVertex2f(x + 75, y - 5);
	glEnd();

	//draw bottom body

	glColor3f(.5, .2, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x - 6, y - 20);
	glVertex2f(x - 3, y - 30);
	glVertex2f(x + 60, y - 30);
	glVertex2f(x + 75, y - 20);
	glVertex2f(x - 6, y - 20);
	glEnd();

	//draw wheels
	glColor3f(0.1, 0.3, 0);
	circle(x + 5, y - 30, 7);
	circle(x + 20, y - 30, 7);
	circle(x + 35, y - 30, 7);
	circle(x + 50, y - 30, 7);

	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(x, y - 25);
	glVertex2f(x + 55, y - 25);

	glVertex2f(x, y - 36);
	glVertex2f(x + 55, y - 36);

	glVertex2f(x + 55, y - 25);
	glVertex2f(x + 60, y - 31);

	glVertex2f(x + 60, y - 31);
	glVertex2f(x + 55, y - 36);

	glVertex2f(x, y - 25);
	glVertex2f(x - 5, y - 31);

	glVertex2f(x, y - 36);
	glVertex2f(x - 5, y - 31);
	glEnd();
}

void reverseTank(int x, int y) {
	glColor3f(.2, .4, 0);
	glBegin(GL_QUADS);
	//draw barrel
	glVertex2f(x - 35, y + 7);
	glVertex2f(x - 110, y + 10);
	glVertex2f(x - 105, y + 5);
	glVertex2f(x - 35, y + 2);
	//draw top
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(x - 8, y + 10);
	glVertex2f(x - 35, y + 10);
	glVertex2f(x - 50, y);
	glVertex2f(x - 5, y);
	glEnd();

	//draw body



	glColor3f(.2, .5, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + 10, y - 20);
	glVertex2f(x - 60, y);
	glVertex2f(x - 75, y - 20);
	glVertex2f(x + 10, y - 20);
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(0.1, .1, .1);
	glVertex2f(x + 10, y - 15);
	glVertex2f(x - 80, y - 15);

	glVertex2f(x + 8, y - 5);
	glVertex2f(x - 75, y - 5);
	glEnd();

	//draw bottom body

	glColor3f(.5, .2, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x + 6, y - 20);
	glVertex2f(x + 3, y - 30);
	glVertex2f(x - 60, y - 30);
	glVertex2f(x - 75, y - 20);
	glVertex2f(x + 6, y - 20);
	glEnd();

	//draw wheels

	glColor3f(0.1, 0.3, 0);
	circle(x - 5, y - 30, 7);
	circle(x - 20, y - 30, 7);
	circle(x - 35, y - 30, 7);
	circle(x - 50, y - 30, 7);

	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(x, y - 25);
	glVertex2f(x - 55, y - 25);

	glVertex2f(x, y - 36);
	glVertex2f(x - 55, y - 36);

	glVertex2f(x - 55, y - 25);
	glVertex2f(x - 60, y - 31);

	glVertex2f(x - 60, y - 31);
	glVertex2f(x - 55, y - 36);

	glVertex2f(x, y - 25);
	glVertex2f(x + 5, y - 31);

	glVertex2f(x, y - 36);
	glVertex2f(x + 5, y - 31);
	glEnd();
}


//draw the sun in the background
void drawSun() {
#define PI 3.1415
	glLineWidth(2);
	float angle;
	glColor3f(1, 1, 0);
	circle(xS, 300, 40);
	glBegin(GL_LINES);
	for (int i = 0; i < 100; i += 10)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(xS + 40 * cos(angle), 300 + 40 * sin(angle));
		glVertex2f(xS + 70 * cos(angle), 300 + 70 * sin(angle));
	}
	glEnd();
}


//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);




	//BACKGROUND


	//draw sky
	if (drawSky) {


		//draw blue sky
		glColor3f(.1, .8, .9);
		glRectf(-600, 400, 600, -190);

		//draw clouds
		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC, 340, 20);
		circle(xC + 25, 350, 30);
		circle(xC + 50, 340, 20);

		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC2, 240, 20);
		circle(xC2 + 25, 250, 30);
		circle(xC2 + 50, 240, 20);

		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC3, 340, 20);
		circle(xC3 + 25, 350, 30);
		circle(xC3 + 50, 340, 20);

		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC4, 240, 20);
		circle(xC4 + 25, 250, 30);
		circle(xC4 + 50, 240, 20);
		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC5, 240, 20);
		circle(xC5 + 25, 250, 30);
		circle(xC5 + 50, 240, 20);
		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC6, 240, 20);
		circle(xC6 + 25, 250, 30);
		circle(xC6 + 50, 240, 20);
		glColor3f(255 / 255., 255 / 255., 255 / 255.);
		circle(xC7, 200, 20);
		circle(xC7 + 25, 210, 30);
		circle(xC7 + 50, 200, 20);

	}

	//draw the sun
	drawSun();

	//draw side roads
	glColor3f(34 / 255., 139 / 255., 34 / 255.);
	glRectf(-600, 50, 600, -400);




	//draw road
	glColor3f(0.1, 0.1, 0.1);
	glRectf(-600, -60, 600, -350);

	glLineWidth(3);
	glBegin(GL_LINES);

	glColor3f(1, 1, 1);

	glVertex2f(-1200 / 2, -80);
	glVertex2f(+1200 / 2, -80);

	glVertex2f(-1200 / 2, -330);
	glVertex2f(1200 / 2, -330);

	glEnd();

	glLineWidth(8);
	glBegin(GL_LINES);

	glVertex2f(xL1, -200);
	glVertex2f(xL2, -200);

	glVertex2f(xL3, -200);
	glVertex2f(xL4, -200);

	glVertex2f(xL5, -200);
	glVertex2f(xL6, -200);

	glVertex2f(xL7, -200);
	glVertex2f(xL8, -200);
	glEnd();

	glColor3f(1, 1, 1);
	glRectf(-60, 390, 85, 315);

	glColor3f(1.0, 0.5, 0.0);
	glRectf(-55, 385, 80, 320);

	for (int i = 0; i < clickcnt; i++)
	{
		minX = 0;
		maxX = 0;
		if (xT[i] < xT[minX])
			minX = i;
		if (xT[i] > xT[maxX])
			maxX = i;
	}
	//display complex objects
	for (int i = 0; i < clickcnt; i++)
	{

		if (draw[i]) {
			if (isReverse[i])
				reverseTank(xT[i], yT[i]);
			else
				drawTank(xT[i], yT[i]);
		}
	}
	//if last object out of window restart program
	if (draw[minX] == false)
	{
		clickcnt = 0;
		activeTimer = !activeTimer;
	}
	//draw random stars
	if (!drawSky) {
		for (int i = 0; i < 60; i++) {

			glColor3f(1, 1, 1);
			glBegin(GL_POLYGON);
			glVertex2f(rand1[i] + 2, rand2[i] + 8);
			glVertex2f(rand1[i] + 10, rand2[i] + 10);
			glVertex2f(rand1[i] + 2, rand2[i] + 12);
			glVertex2f(rand1[i], rand2[i] + 20);
			glVertex2f(rand1[i] - 2, rand2[i] + 12);
			glVertex2f(rand1[i] - 10, rand2[i] + 10);
			glVertex2f(rand1[i] - 2, rand2[i] + 8);
			glVertex2f(rand1[i], rand2[i]);
			glEnd();
		}
	}

	glColor3d(1, 1, 1);
	vprint(-50, 360, GLUT_BITMAP_8_BY_13, "Ahmet Arslanoglu");

	glColor3d(1, 1, 1);
	vprint(-20, 340, GLUT_BITMAP_8_BY_13, "22102115");

	glColor3d(1, 1, 1);
	vprint(-580, -380, GLUT_BITMAP_8_BY_13, "MOUSE LEFT BUTTON TO CREATE OBJECTS(MAX 10 OBJECTS)");

	glColor3d(1, 1, 1);
	vprint(0, -380, GLUT_BITMAP_8_BY_13, "PRESS F1 TO ANIMATE");

	glColor3d(1, 1, 1);
	vprint(280, -380, GLUT_BITMAP_8_BY_13, "%d OBJECTS ON THE SCREEN", clickcnt);
	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	//IF F1 PRESSED ANIMATE OBJECTS
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	case GLUT_KEY_F1:if (activeTimer == true) {
		activeTimer = !activeTimer;
		move = true;
	}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	if (activeTimer) {
		if (y > 470 && y < 720) {
			if (clickcnt < 10) {
				if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {


					xT[clickcnt] = x - winWidth / 2;
					yT[clickcnt] = winHeight / 2 - y;
					draw[clickcnt] = true;
					if (xT[clickcnt] > 0)
						isReverse[clickcnt] = true;


					clickcnt++;

				}
			}
		}
	}



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	/* animate the current shape until it touches the edge of the window.*/
	if (xS > (-winWidth)) {
		xS -= 2;
	}
	if (xS == -600) {
		xS = 1200;
		drawSky = false;
	}
	//determine when night when day
	if (xS - 40 == 600)
		drawSky = true;

	if (xS + 50 == -600)
		drawSky = false;

	if (!activeTimer && draw[minX])
		for (int i = 0; i < clickcnt; i++)
		{
			if (isReverse[i]) {
				xT[i] -= 2;
			}
			else {
				xT[i] += 2;
			}
			if (xT[i] > 600)
				draw[i] = false;
			else if (xT[i] < -600)
				draw[i] = false;
		}
	//animate clouds
	if (drawSky) {
		if (xC < 600) {
			xC++;
		}
		else if (xC == 600) {
			xC = -600;
		}
		if (xC2 < 600) {
			xC2++;
		}
		else if (xC2 == 600) {
			xC2 = -600;
		}
		if (xC3 < 600) {
			xC3++;
		}
		else if (xC3 == 600) {
			xC3 = -600;
		}
		if (xC4 < 600) {
			xC4++;
		}
		else if (xC4 == 600) {
			xC4 = -600;
		}
		if (xC5 < 600) {
			xC5++;
		}
		else if (xC5 == 600) {
			xC5 = -600;
		}
		if (xC6 < 600) {
			xC6++;
		}
		else if (xC6 == 600) {
			xC6 = -600;
		}
		if (xC7 < 600) {
			xC7++;
		}
		else if (xC7 == 600) {
			xC7 = -600;
		}


	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Ahmet Eren Arslanoglu Tank Animasyon");

	//CREATE RANDOM COORDINATES FOR STARS
	for (int i = 0; i < 100; i++) {
		rand1[i] = rand() % 1200 - 600;
		rand2[i] = rand() % 600 - -50;
	}

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}