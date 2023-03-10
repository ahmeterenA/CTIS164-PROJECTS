/*********
----------
STUDENT :   AHMET EREN ARSLANOGLU
SECTION :   02
HOMEWORK:   3
----------
PROBLEMS: none
----------
ADDITIONAL FEATURES:    USER CAN CONTROL LIGHTS AND ALSO PLANET AND LIGHTING WILL BE CALCULATED ACCORDINGLY TO NEW POSITION
                        F5(RESTART) ALSO GIVES RANDOM POSITIONS TO ORBITS
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
#include "vec.h"
#include<time.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    vec_t   pos;
    color_t color;
    float angle;
    float speed;
    float radius;
    float r;
} light_t;

typedef struct {
    vec_t   pos;
    float radius;
} planet_t;

typedef struct {
    vec_t pos;

    vec_t N;
} vertex_t;
bool restartObjPressed = false;
bool activeTimer = true;
bool turnOffRed = false, turnOffBlue = false, turnOffGreen = false;
bool moveRed = false, moveBlue = false, moveGreen = false, movePlanet = false;
#define NUM 3


light_t light[3] =
{
   { {    500, 100 }, { 1, 0, 0 },30, { 1},300,15 },
    { {  100, -50 }, { 0, 1, 0 },-50, {1},200,15 },
   { { -120, 20 }, { 0, 0, 1 }, 200., {1},100,15 }
};

planet_t p = { {0,0},50 };


color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

double distanceImpact(double d) {
    return (-1.0 / 750.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}
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


void drawPlanet(planet_t p) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0, 0, 0);
    glVertex2f(p.pos.x, p.pos.y);
    for (int x = 0; x <= 360; x++) {
        vertex_t P = { {p.pos.x + p.radius * cos(x * D2R), p.pos.y + p.radius * cos(x * D2R)}, {cos(x * D2R), sin(x * D2R)} };

        color_t res = { 0, 0, 0 };
        for (int i = 0; i < NUM; i++) {
            res = addColor(res, calculateColor(light[i], P));
        }
        glColor3f(res.r, res.g, res.b);
        glVertex2f(p.pos.x + p.radius * cos(x * D2R), p.pos.y + p.radius * sin(x * D2R));
    }
    glEnd();
}

void drawLightObj(light_t light)
{
    glColor3f(light.color.r, light.color.g, light.color.b);
    circle(light.pos.x, light.pos.y, light.r);
}

void displayOrbit() {
    glColor3f(1, 1, 1);
    for (int i = 0; i < 3; i++) {
        circle_wire(0, 0, light[i].radius);
    }

}
//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0.1, 0.1, 0.1, 0.1);
    glClear(GL_COLOR_BUFFER_BIT);

    displayOrbit();
    drawPlanet(p);
    //IF F1/F2/F3 PRESSED TURN OFF THE LIGHT ACCORDINGLY
    if (!turnOffRed) {
        drawLightObj(light[0]);
        light[0].color = { 1,0,0 };
    }
    else
        light[0].color = { 0,0,0 };
    if (!turnOffBlue) {
        drawLightObj(light[2]);
        light[2].color = { 0,0,1 };
    }
    else
        light[2].color = { 0,0,0 };
    if (!turnOffGreen) {
        drawLightObj(light[1]);
        light[1].color = { 0,1,0 };
    }
    else
        light[1].color = { 0,0,0 };


    //OPTIONS PANEL
    glColor3f(.15, .15, .15);
    glRectf(-600, -350, 600, -400);

    glColor3f(1, 1, 0);
    if (!turnOffRed)
    {
        vprint(-540, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F1(Red): ON");
    }
    else
        vprint(-540, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F1(Red): OFF");
    if (!turnOffGreen)
    {
        vprint(-310, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F2(Green): ON");
    }
    else
        vprint(-310, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F2(Green): OFF");
    if (!turnOffBlue)
    {
        vprint(-70, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F3(Blue): ON");
    }
    else
        vprint(-70, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F3(Blue): OFF");
    if (activeTimer)
    {
        vprint(170, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F4(Animation): ON");
    }
    else
        vprint(170, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F4(Animation): OFF");
    if (moveRed)
    {
        vprint(-580, -20, GLUT_BITMAP_9_BY_15, "PRESS AGAIN R TO RESET");
    }
    else
        vprint(-580, -20, GLUT_BITMAP_9_BY_15, "PRESS R TO MOVE RED LIGHT");
    if (moveBlue)
    {
        vprint(-580, 20, GLUT_BITMAP_9_BY_15, "PRESS AGAIN B TO RESET");
    }
    else
        vprint(-580, 20, GLUT_BITMAP_9_BY_15, "PRESS B TO MOVE BLUE LIGHT");
    if (moveGreen)
    {
        vprint(-580, 60, GLUT_BITMAP_9_BY_15, "PRESS AGAIN G TO RESET");
    }
    else
        vprint(-580, 60, GLUT_BITMAP_9_BY_15, "PRESS G TO MOVE GREEN LIGHT");
    if (movePlanet)
    {
        vprint(-580, 100, GLUT_BITMAP_9_BY_15, "PRESS AGAIN P TO RESET");
    }
    else
        vprint(-580, 100, GLUT_BITMAP_9_BY_15, "PRESS P TO MOVE PLANET");

    vprint(450, -383, GLUT_BITMAP_TIMES_ROMAN_24, "F5:Restart");

    //STUDENT NAME LABEL
    glColor3f(255 / 255, 165 / 255, 0);
    glRectf(-550, 370, -390, 270);

    glColor3f(1, 1, 1);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-540, 360);
    glVertex2f(-400, 360);
    glVertex2f(-400, 280);
    glVertex2f(-540, 280);
    glEnd();
    glLineWidth(1);

    vprint(-530, 335, GLUT_BITMAP_HELVETICA_18, "AHMET EREN");
    vprint(-530, 315, GLUT_BITMAP_HELVETICA_18, "ARSLANOGLU");
    vprint(-515, 295, GLUT_BITMAP_HELVETICA_18, "22102115");


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
    if (activeTimer) {
        if (key == 98 || key == 66)
            moveBlue = !moveBlue;
        if (key == 114 || key == 82)
            moveRed = !moveRed;
        if (key == 103 || key == 71)
            moveGreen = !moveGreen;
        if (key == 112 || key == 80)
            movePlanet = !movePlanet;
    }
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
    // Write your codes here.

    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1: turnOffRed = !turnOffRed; break;
    case GLUT_KEY_F3: turnOffBlue = !turnOffBlue; break;
    case GLUT_KEY_F2: turnOffGreen = !turnOffGreen; break;
    case GLUT_KEY_F4:activeTimer = !activeTimer; break;
    case GLUT_KEY_F5:restartObjPressed = !restartObjPressed; break;

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
    // Write your codes here.



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
void restartObj(double* x, double* y, float* angle)
{
    int randspeed;
    light[0].radius = ((rand() % 50) + 250);
    light[1].radius = ((rand() % 45) + 170);
    light[2].radius = ((rand() % 45) + 100);
    for (int i = 0; i < NUM; i++)
    {
        int randpos = ((rand() % 360) + 1);
        *angle = 2 * PI * randpos;
        *x = cos(*angle) * light[i].radius;
        *y = sin(*angle) * light[i].radius;
        //FIND NEW SPEED AND ALSO DIRECTION WHEN F5 IS PRESSED
        randspeed = ((rand() % 8) + 1) - 5;
        while (randspeed == 0)
            randspeed = ((rand() % 8) + 1) - 5;
        light[i].speed = randspeed;
    }

}
void move(light_t* light)
{
    if (light->pos.x<600 && light->pos.y<400 && light->pos.x >-600 && light->pos.y > -350) {
        if (left)
            light->pos.x -= 5;
        if (right)
            light->pos.x += 5;

        if (up)
            light->pos.y += 5;
        if (down)
            light->pos.y -= 5;
    }
    else
    {

        if (light->pos.x >= 600)
            light->pos.x = -595;
        if (light->pos.x <= -600)
            light->pos.x = 595;
        if (light->pos.y <= -350)
            light->pos.y = 395;
        if (light->pos.y >= 400)
            light->pos.y = -345;
    }
}
void movePlanetFunc(planet_t* p)
{
    if (p->pos.x<250 && p->pos.y<250 && p->pos.x >-250 && p->pos.y > -250) {
        if (left)
            p->pos.x -= 3;
        if (right)
            p->pos.x += 3;

        if (up)
            p->pos.y += 3;
        if (down)
            p->pos.y -= 3;
    }
    else {
        p->pos.x = 0;
        p->pos.y = 0;
    }
}
#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    if (activeTimer) {
        if (restartObjPressed) {
            for (int i = 0; i < NUM; i++) {
                restartObj(&light[i].pos.x, &light[i].pos.y, &light[i].angle);
            }
            restartObjPressed = false;
        }
    }
    //IF ACTIVE TIMER IS ON AND P, R, G, OR B IS PRESSED USER CAN HAVE CONTROL
    if (activeTimer) {
        if (moveRed)
        {
            move(&light[0]);
        }
        else
        {
            light[0].pos.x = light[0].radius * cos(light[0].angle * D2R);
            light[0].pos.y = light[0].radius * sin(light[0].angle * D2R);
            light[0].angle += light[0].speed;
        }
        if (moveBlue)
        {
            move(&light[2]);
        }
        else
        {
            light[2].pos.x = light[2].radius * cos(light[2].angle * D2R);
            light[2].pos.y = light[2].radius * sin(light[2].angle * D2R);
            light[2].angle += light[2].speed;
        }
        if (moveGreen)
        {
            move(&light[1]);
        }
        else
        {
            light[1].pos.x = light[1].radius * cos(light[1].angle * D2R);
            light[1].pos.y = light[1].radius * sin(light[1].angle * D2R);
            light[1].angle += light[1].speed;
        }
        if (movePlanet)
            movePlanetFunc(&p);
        else {
            p.pos.x = 0;
            p.pos.y = 0;
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
    //FIND NEW RANDOM VALUES FOR POSITIONS OF THE LIGHTS AND THE SPEED
    srand(time(NULL));
    for (int i = 0; i < NUM; i++) {
        restartObj(&light[i].pos.x, &light[i].pos.y, &light[i].angle);
    }
}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("AHMET EREN ARSLANOGLU PLANET LIGHTING SIMULATION HW 3");

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