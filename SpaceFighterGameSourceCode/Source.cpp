/***
   CTIS164 - Template Source Program
----------
STUDENT : Ahmet Eren Arslanoglu
SECTION : 02
HOMEWORK: HOMEWORK-2
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:    STARS RANDOMLY CREATED EVERY RUN
                        DOUBLE LAZER


***/
#define GL_SILENCE_DEPRECATION
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include<time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS 20

#define  MENU 0
#define  PLAY   1
#define  PAUSE   2
#define  END  3
#define CREDIT 4




/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, rotatel = false, rotater = false, spacebar = false;
int  winWidth, winHeight, fire_rate = 0; // current Window width and height

#define MAX_FIRE 15
#define FIRE_RATE 15
#define MAXOBJ 5

//random array to hold stars coordinates
int rand1[100];
int rand2[100];

typedef struct {
    double x, y;
} point_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} fire_t;
typedef struct {
    point_t pos;
    double angle;
    double radius; // circular collision boundary
    bool hit;      // collision state
    bool fire;

} player_t;

typedef struct {
    point_t pos;
    float speed;
    float radius;
} target_t;

int state = MENU;
player_t p = { {0,-220} , 90,  20 };
fire_t   fr[MAX_FIRE];
fire_t fr2[MAX_FIRE];
target_t target[MAXOBJ];
bool timer = false;
float sec = 19, msec = 99;


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
void pie_filled(float x, float y, float r, float start, float end) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    for (float angle = start; angle < end; angle += 10) {
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
    glEnd();
}
void pie_wire(float x, float y, float r, float start, float end) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    for (float angle = start; angle < end; angle += 10) {
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
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
    vsprintf(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}
void vertex(point_t P, point_t Tr, double angle) {
    float xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    float yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2f(xp, yp);
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
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
void drawTimer() {

    glColor3f(1, 1, 1);
    vprint(-50, 250, GLUT_BITMAP_TIMES_ROMAN_24, "%.0f : %.0f", sec, msec);

}
void drawBg() {
    if (state == PLAY) {

        glColor3ub(0, 0, 0);
        glBegin(GL_POLYGON);
        glVertex2f(-winWidth / 2, winHeight / 2);
        glVertex2f(winWidth / 2, winHeight / 2);
        glVertex2f(winWidth / 2, -winHeight / 2);
        glVertex2f(-winWidth / 2, -winHeight / 2);
        glEnd();

        //stars
        for (int i = 0; i < 60; i++) {
            glColor4f(1, 1, 1, 0.8);
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

        //moon
        glColor3f(0.75, 0.75, 0.75);
        circle(250, 220, 160);
        glColor3f(.35, .35, .35);
        circle(150, 200, 50);
        circle(180, 260, 70);
        circle(160, 250, 40);
        circle(300, 160, 50);
        circle(380, 250, 60);

        //sun
#define PI 3.1415
        glLineWidth(2);
        float angle;
        glColor3f(1, 1, 0);
        circle(-320, 265, 100);
        glBegin(GL_LINES);
        for (int i = 0; i < 100; i += 10)
        {
            angle = 2 * PI * i / 100;
            glVertex2f(-320 + 1 * cos(angle), 300 + 30 * sin(angle));
            glVertex2f(-320 + 180 * cos(angle), 300 + 180 * sin(angle));
        }
        glEnd();
    }
    glColor3f(0, 0, 0);
    vprint(-375, 250, GLUT_BITMAP_TIMES_ROMAN_10, "PRESS <- TO MOVE LEFT");
    vprint(-375, 230, GLUT_BITMAP_TIMES_ROMAN_10, "PRESS -> TO MOVE RIGHT");

    vprint(-370, 210, GLUT_BITMAP_TIMES_ROMAN_10, "SPACE TO SHOOT");
    vprint(-50, 250, GLUT_BITMAP_TIMES_ROMAN_10, "%.0f : %.0f", sec, msec);

    drawTimer();
    if (state == MENU) {

        glColor3f(1, 0, 0);
        glRectf(-900, 900, 900, -900);
        glEnd();
        glColor3f(1, 1, 1);
        glRectf(-350, 250, 350, -250);
        glEnd();
        glColor3f(0, 0, 0);
        vprint(-70, 50, GLUT_BITMAP_HELVETICA_18, "START");
        vprint(-70, 0, GLUT_BITMAP_HELVETICA_18, "CREDITS");
        print(-140, -200, "PRESS F1 TO START TO GAME", GLUT_BITMAP_HELVETICA_18);
        print(-140, -230, "PRESS F2 TO SEE CREDITS", GLUT_BITMAP_HELVETICA_18);


    }
    else if (state == PAUSE) {
        glColor4f(0.3, 0.3, 0.3, 0.6);
        glBegin(GL_QUADS);
        glVertex2d(-400, -400);
        glVertex2d(400, -400);
        glVertex2d(400, 400);
        glVertex2d(-400, 400);
        glEnd();
        glColor3ub(255, 255, 255);
        vprint(-160, 0, GLUT_BITMAP_HELVETICA_18, "GAME PAUSED (Press F1 to Continue)");
    }
    else if (state == END) {
        glColor4f(0.3, 0.3, 0.3, 1);
        glBegin(GL_QUADS);
        glVertex2d(-400, -400);
        glVertex2d(400, -400);
        glVertex2d(400, 400);
        glVertex2d(-400, 400);
        glEnd();
        glColor3ub(255, 255, 255);
        glColor3ub(255, 255, 255);
        vprint(-170, 0, GLUT_BITMAP_HELVETICA_18, "GAME IS OVER (Press F1 to Go Back to The Main Menu)");
    }
    else if (state == CREDIT) {
        glColor4f(0.3, 0.3, 0.3, 1);
        glBegin(GL_QUADS);
        glVertex2d(-400, -400);
        glVertex2d(400, -400);
        glVertex2d(400, 400);
        glVertex2d(-400, 400);
        glEnd();
        glColor3f(0, 0, 0);
        glRectf(-150, 80, 150, -80);
        glEnd();
        glColor3f(1, 1, 1);
        vprint(-134, 40, GLUT_BITMAP_HELVETICA_18, "by AHMET EREN ARSLANOGLU");
        vprint(-100, 0, GLUT_BITMAP_HELVETICA_18, "BILKENT CTIS-166");
        vprint(-50, -40, GLUT_BITMAP_HELVETICA_18, "22102115");
        glColor3ub(255, 255, 255);
        glColor3ub(255, 255, 255);
        vprint(-230, -220, GLUT_BITMAP_HELVETICA_18, "PRESS F2 AGAIN TO RETURN BACK TO THE MAIN MENU");
    }

}
void Lazer() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            glColor3f(1, 0, 0);
            glPushMatrix();
            glBegin(GL_POLYGON);           //Lazer stem
            glVertex2f(fr[i].pos.x , fr[i].pos.y );
            glVertex2f(fr[i].pos.x , fr[i].pos.y + 30);
            glVertex2f(fr[i].pos.x + 4, fr[i].pos.y + 30);
            glVertex2f(fr[i].pos.x + 4, fr[i].pos.y );
            circle(fr[i].pos.x + 2, fr[i].pos.y + 30, 4);
            glEnd();

        }
    }
}
void Lazer2() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr2[i].active) {
            glColor3f(1, 0, 0);
            glPushMatrix();
            glBegin(GL_POLYGON);           //Lazer stem
            glVertex2f(fr2[i].pos.x , fr2[i].pos.y );
            glVertex2f(fr2[i].pos.x , fr2[i].pos.y + 30);
            glVertex2f(fr2[i].pos.x +4, fr2[i].pos.y +30);
            glVertex2f(fr2[i].pos.x +4, fr2[i].pos.y);
            circle(fr2[i].pos.x + 2, fr2[i].pos.y + 30, 4);
            glEnd();
        }
    }
}

void drawplayer(player_t p) {
    //glColor3f(199/255,36/255,177/255);
    glColor3f(1, 0, 1);
    glLineWidth(3);

    glBegin(GL_LINE_LOOP);
    glVertex2f(p.pos.x - 3, p.pos.y);
    glVertex2f(p.pos.x + 15, p.pos.y);
    glVertex2f(p.pos.x + 15, p.pos.y - 10);
    glVertex2f(p.pos.x + 30, p.pos.y - 10);
    glVertex2f(p.pos.x + 30, p.pos.y - 30);
    glVertex2f(p.pos.x + 40, p.pos.y - 30);
    glVertex2f(p.pos.x + 40, p.pos.y - 15);
    glVertex2f(p.pos.x + 50, p.pos.y - 15);
    glVertex2f(p.pos.x + 50, p.pos.y - 35);
    glVertex2f(p.pos.x + 57, p.pos.y - 35);
    glVertex2f(p.pos.x + 57, p.pos.y - 20);
    glVertex2f(p.pos.x + 65, p.pos.y - 20);
    glVertex2f(p.pos.x + 65, p.pos.y - 30);
    glVertex2f(p.pos.x + 73, p.pos.y - 30);
    glVertex2f(p.pos.x + 73, p.pos.y - 40);
    glVertex2f(p.pos.x + 80, p.pos.y - 40);
    glVertex2f(p.pos.x + 84, p.pos.y - 40);
    glVertex2f(p.pos.x + 84, p.pos.y - 60);
    glVertex2f(p.pos.x + 92, p.pos.y - 60);
    glVertex2f(p.pos.x + 92, p.pos.y - 40);
    glVertex2f(p.pos.x + 102, p.pos.y - 40);
    glVertex2f(p.pos.x + 102, p.pos.y - 30);
    glVertex2f(p.pos.x + 110, p.pos.y - 30);
    glVertex2f(p.pos.x + 110, p.pos.y - 20);
    glVertex2f(p.pos.x + 118, p.pos.y - 20);
    glVertex2f(p.pos.x + 118, p.pos.y - 35);
    glVertex2f(p.pos.x + 124, p.pos.y - 35);
    glVertex2f(p.pos.x + 124, p.pos.y - 15);
    glVertex2f(p.pos.x + 134, p.pos.y - 15);
    glVertex2f(p.pos.x + 134, p.pos.y - 30);
    glVertex2f(p.pos.x + 144, p.pos.y - 30);
    glVertex2f(p.pos.x + 144, p.pos.y - 10);
    glVertex2f(p.pos.x + 159, p.pos.y - 10);
    glVertex2f(p.pos.x + 159, p.pos.y);
    glVertex2f(p.pos.x + 177, p.pos.y);
    glVertex2f(p.pos.x + 149, p.pos.y + 25);
    glVertex2f(p.pos.x + 139, p.pos.y + 25);
    glVertex2f(p.pos.x + 139, p.pos.y + 60);
    glVertex2f(p.pos.x + 133, p.pos.y + 60);
    glVertex2f(p.pos.x + 133, p.pos.y + 25);
    glVertex2f(p.pos.x + 125, p.pos.y + 25);
    glVertex2f(p.pos.x + 125, p.pos.y + 35);
    glVertex2f(p.pos.x + 115, p.pos.y + 35);
    glVertex2f(p.pos.x + 115, p.pos.y + 45);
    glVertex2f(p.pos.x + 60, p.pos.y + 45);
    glVertex2f(p.pos.x + 60, p.pos.y + 35);
    glVertex2f(p.pos.x + 50, p.pos.y + 35);
    glVertex2f(p.pos.x + 50, p.pos.y + 25);
    glVertex2f(p.pos.x + 43, p.pos.y + 25);
    glVertex2f(p.pos.x + 43, p.pos.y + 60);
    glVertex2f(p.pos.x + 37, p.pos.y + 60);
    glVertex2f(p.pos.x + 37, p.pos.y + 25);
    glVertex2f(p.pos.x + 29, p.pos.y + 25);


    glEnd();

    glColor3f(0.1, .1, .1);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x - 3, p.pos.y);
    glVertex2f(p.pos.x + 30, p.pos.y);
    glVertex2f(p.pos.x + 30, p.pos.y + 25);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x + 150, p.pos.y);
    glVertex2f(p.pos.x + 174, p.pos.y);
    glVertex2f(p.pos.x + 150, p.pos.y + 25);
    glEnd();
    glRectf(p.pos.x + 61, p.pos.y + 44, p.pos.x + 114, p.pos.y - 19);
    glRectf(p.pos.x + 67, p.pos.y - 10, p.pos.x + 108, p.pos.y - 30);
    glRectf(p.pos.x + 75, p.pos.y - 30, p.pos.x + 100, p.pos.y - 40);
    glRectf(p.pos.x + 85, p.pos.y - 40, p.pos.x + 90, p.pos.y - 60);
    glRectf(p.pos.x + 30, p.pos.y + 25, p.pos.x + 70, p.pos.y - 10);
    glRectf(p.pos.x + 36, p.pos.y + 25, p.pos.x + 42, p.pos.y + 60);
    glRectf(p.pos.x + 100, p.pos.y + 25, p.pos.x + 139, p.pos.y - 10);
    glRectf(p.pos.x + 15, p.pos.y + 5, p.pos.x + 157, p.pos.y - 10);
    glRectf(p.pos.x + 100, p.pos.y + 25, p.pos.x + 150, p.pos.y);
    glRectf(p.pos.x + 133, p.pos.y + 25, p.pos.x + 138, p.pos.y + 60);
    glRectf(p.pos.x + 30, p.pos.y, p.pos.x + 40, p.pos.y - 30);
    glRectf(p.pos.x + 30, p.pos.y, p.pos.x + 143, p.pos.y - 15);
    glRectf(p.pos.x + 135, p.pos.y, p.pos.x + 143, p.pos.y - 30);
    glRectf(p.pos.x + 51, p.pos.y + 33, p.pos.x + 123, p.pos.y);
    glColor3f(1, 0, 0);
    glRectf(p.pos.x + 36, p.pos.y + 50, p.pos.x + 42, p.pos.y + 60);
    glRectf(p.pos.x + 133, p.pos.y + 50, p.pos.x + 138, p.pos.y + 60);
    glRectf(p.pos.x + 133, p.pos.y - 31, p.pos.x + 145, p.pos.y - 35);
    glRectf(p.pos.x + 29, p.pos.y - 31, p.pos.x + 40, p.pos.y - 35);
    glRectf(p.pos.x + 83, p.pos.y - 62, p.pos.x + 93, p.pos.y - 66);
    glColor3f(1, 0.5, 0);
    glRectf(p.pos.x + 135, p.pos.y - 35, p.pos.x + 143, p.pos.y - 39);
    glRectf(p.pos.x + 31, p.pos.y - 35, p.pos.x + 38, p.pos.y - 39);
    glRectf(p.pos.x + 85, p.pos.y - 66, p.pos.x + 91, p.pos.y - 70);
    glColor3f(1, 1, 0);
    glRectf(p.pos.x + 137, p.pos.y - 39, p.pos.x + 141, p.pos.y - 43);
    glRectf(p.pos.x + 33, p.pos.y - 39, p.pos.x + 36, p.pos.y - 43);
    glRectf(p.pos.x + 87, p.pos.y - 70, p.pos.x + 89, p.pos.y - 74);
    glColor3f(168 / 255., 204 / 255., 215 / 255.);
    glRectf(p.pos.x + 68, p.pos.y + 30, p.pos.x + 108, p.pos.y + 43);
    glColor3f(169 / 255., 169 / 255., 169 / 255.);
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 72, p.pos.y + 33);
    glVertex2f(p.pos.x + 76, p.pos.y + 40);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 84, p.pos.y + 33);
    glVertex2f(p.pos.x + 88, p.pos.y + 40);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 96, p.pos.y + 33);
    glVertex2f(p.pos.x + 100, p.pos.y + 40);
    glEnd();
    glColor3f(1, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 35, p.pos.y + 20);
    glVertex2f(p.pos.x + 35, p.pos.y + 5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 75, p.pos.y + 20);
    glVertex2f(p.pos.x + 75, p.pos.y - 40);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 65, p.pos.y + 20);
    glVertex2f(p.pos.x + 65, p.pos.y - 10);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 45, p.pos.y + 20);
    glVertex2f(p.pos.x + 45, p.pos.y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 55, p.pos.y + 20);
    glVertex2f(p.pos.x + 55, p.pos.y - 10);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 85, p.pos.y + 20);
    glVertex2f(p.pos.x + 85, p.pos.y - 40);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 95, p.pos.y + 20);
    glVertex2f(p.pos.x + 95, p.pos.y - 40);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 135, p.pos.y + 20);
    glVertex2f(p.pos.x + 135, p.pos.y + 5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 125, p.pos.y + 20);
    glVertex2f(p.pos.x + 125, p.pos.y);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 115, p.pos.y + 20);
    glVertex2f(p.pos.x + 115, p.pos.y - 10);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(p.pos.x + 105, p.pos.y + 20);
    glVertex2f(p.pos.x + 105, p.pos.y - 10);
    glEnd();

}
void drawTarget(target_t t[]) {
    for (int i = 0; i < MAXOBJ; i++) {
        glColor3f(1, 0, 0);
        circle(t[i].pos.x, t[i].pos.y, t[i].radius);
    }
}


//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //

    glClearColor(125. / 255, 63. / 255, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    drawBg();

    if (state == PLAY) {
        Lazer();
        Lazer2();
        drawplayer(p);

        drawTarget(target);
    }
    glutSwapBuffers();
}
int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false)
            return i;
    }
    return -1;
}
int findAvailableFire2() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr2[i].active == false)
            return i;
    }
    return -1;
}
//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = true;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == 'a' || key == 'A')
        rotatel = false;
    if (key == 'd' || key == 'D')
        rotater = false;
    if (key == ' ') {
        spacebar = false;
        p.fire = false;
    }
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
    }
    if (key == GLUT_KEY_F1) {
        switch (state)
        {
        case MENU:
            state = PLAY;
            timer = true;
            break;
        case PLAY:
            state = PAUSE;
            timer = !timer;
            break;
        case PAUSE:
            state = PLAY;
            timer = !timer;
            break;
        case END:
            state = MENU;
            timer = false;
            break;
        }
    }
    if (key == GLUT_KEY_F2) {
        switch (state)
        {
        case MENU:
            state = CREDIT;
            break;
        case CREDIT:
            state = MENU;
            break;
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

    // Write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)



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
bool testCollision(fire_t fr, target_t t) {
    float dx = t.pos.x - fr.pos.x;
    float dy = t.pos.y - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= t.radius;
}
void resetTarget(target_t* target) {
    float yPos = rand() % 300 - 100;
    float xPos = -400;
    target->radius = TARGET_RADIUS;
    target->pos = { xPos, yPos };
    target->speed = 3.0 * (rand() % 100) / 100.0 + 0.5;

}



#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    for (int i = 0; i < 60; i++) {
        rand2[i]--;
        if (rand2[i] < -300) {
            rand2[i] = 300;
            rand1[i] = rand() % 800 - 400;
        }
    }
    
    if (timer)
    {
        msec -= 1;
        if (msec <= 0 && sec != 0)
        {
            sec -= 1;
            msec = 99;
        }
        else if (msec <= 0 && sec <= 0) {
            state = END;
            timer = false;
            sec = 19;
            msec = 99;
        }

    }
    if (state == PLAY) {
        if (left && p.pos.x > -400) {
            p.pos.x = p.pos.x - 10;
        }

        if (right && p.pos.x + 174 < 400) {
            p.pos.x = p.pos.x + 10;
        }
        if (spacebar && fire_rate == 0) {
            int availFire = findAvailableFire();
            int availFire2 = findAvailableFire();
            if (availFire != -1) {
                fr[availFire].pos.x = p.pos.x+37;
                fr[availFire].pos.y = p.pos.y+60;
                fr[availFire].angle = p.angle;
                fr[availFire].active = true;
                fire_rate = FIRE_RATE;
            }
            if (availFire2 != -1) {
                fr2[availFire2].pos.x = p.pos.x+133;
                fr2[availFire2].pos.y = p.pos.y+60;
                fr2[availFire2].angle = p.angle;
                fr2[availFire2].active = true;
                fire_rate = FIRE_RATE;
            }
        }
        if (fire_rate > 0)
            fire_rate--;

        for (int i = 0; i < MAX_FIRE; i++) {
            if (fr[i].active) {
                fr[i].pos.y += 25;

            }
            if (fr2[i].active) {
                fr2[i].pos.y += 25;
            }
            if (fr2[i].pos.y > 400) {
                fr2[i].active = false;
            }
            if (fr[i].pos.y > 400) {
                fr[i].active = false;
            }
            for (int j = 0; j < MAXOBJ; j++)
            {

                if (testCollision(fr[i], target[j])) {
                    fr[i].active = false;
                    resetTarget(&target[j]);
                }
                if (testCollision(fr2[i], target[j])) {
                    fr2[i].active = false;
                    resetTarget(&target[j]);
                }

            }
            

        }
        for (int i = 0; i < MAXOBJ; i++)
        {

            target[i].pos.x += target[i].speed;

            if (target[i].pos.x > 400) {
                resetTarget(&target[i]);
            }
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

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("SPACE FIGHTER GAME BY AHMET EREN ARSLANOGLU");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {

        rand1[i] = rand() % 800 - 400;
        rand2[i] = rand() % 600 - 300;
    }

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