/*
 * main.cpp
 *
 * main code for snake3D, the best game in the world! ever! thrust me! its awesome! you'll never see something like this! if you can read this without scrolling, your display is larger than mine :(
 *
 *  Created on: 18.10.2013
 *      Author: Railion
 */

#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>

#include <math.h>

#include "API.h"
#include "settings.h"
#include "Player.h"

//globals

float lightAmbient = 0.7;
Color  bg(clBlack);
Player player;
Point  target;
Cam    freecam;
GLuint walls;
unsigned int frameCount = 0;


//states
bool
	light  = true,
	smooth = true,
	paused = true
;

template <class t>
void swap(t& a, t& b) {
	 t tmp = a;
	 a = b;
	 b = tmp;
}

void handleMouse(int x, int y) {
	static bool set = true;

	if (set) {
		if (paused) {
			freecam.rotate(-y + 300, -x + 300, 0);
		    glutPostRedisplay();
		}
		glutWarpPointer(300, 300);
	    set = false;
	} else set = true;


}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			player.shutdown("quit by player");
			break;
		case key_up:
			player.turnUp();
			break;
		case key_down:
			player.turnDown();
			break;
		case key_left:
			player.turnLeft();
			break;
		case key_right:
			player.turnRight();
			break;

		case key_smooth:
			smooth = !smooth;
			if (smooth) glShadeModel(GL_SMOOTH);
			else glShadeModel(GL_FLAT);
			break;
		case key_light:
			light = !light;
			if (light) glEnable(GL_LIGHT0);
			else glDisable(GL_LIGHT0);
			break;
		case key_pause:
			paused = !paused;
			lightAmbient = paused ? 0.7 : 0.1;

			break;
	}
}

void update(int x) {
	Point p;
	if (x > 10000) x = 0;

	if (!paused) {
		player.move();
		p = player.getPos() - target;
		if (abs(p.x) < 0.1 && abs(p.y) < 0.1 && abs(p.z) < 0.1) {
			player.eat();
			target.set(round(rnd(-FIELD_SIZE, FIELD_SIZE)),
					   round(rnd(-FIELD_SIZE, FIELD_SIZE)),
					   round(rnd(-FIELD_SIZE, FIELD_SIZE)));
		}
	}

    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, update, x++);
}

void initWorld() {
	float x, y, z;
	target.set(round(rnd(-FIELD_SIZE, FIELD_SIZE)),
			   round(rnd(-FIELD_SIZE, FIELD_SIZE)),
			   round(rnd(-FIELD_SIZE, FIELD_SIZE)));
	glPolygonMode(GL_BACK, GL_POINT);
    glutWarpPointer(300, 300);
    glutSetCursor(GLUT_CURSOR_NONE);
    walls = glGenLists(1);

    // player light
	glEnable(GL_LIGHT0);
    GLfloat lightColor0[] = { lightPlayer, lightPlayer, lightPlayer, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightColor0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, fov);

    // target light
	glEnable(GL_LIGHT1);
    GLfloat lightColor1[] = { lightTarget, lightTarget, lightTarget, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  lightColor1);

    glNewList(walls, GL_COMPILE);
    glBegin(GL_QUADS);
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++) {
			x = 2 * i + 0.5 - FIELD_SIZE;
			y = 2 * j + 0.5 - FIELD_SIZE;
			z = FIELD_SIZE + 0.75;

			clRed.use();
			glNormal3f(0, 0, 1);
			glVertex3f(x,     y,     -z);
			glVertex3f(x + 1, y,     -z);
			glVertex3f(x + 1, y + 1, -z);
			glVertex3f(x,     y + 1, -z);

			glNormal3f(0, 0, -1);
			glVertex3f(x,     y,      z);
			glVertex3f(x,     y + 1,  z);
			glVertex3f(x + 1, y + 1,  z);
			glVertex3f(x + 1, y,      z);

			swap(x, z);

			clBlue.use();
			glNormal3f(-1, 0, 0);
			glVertex3f( x, y,     z);
			glVertex3f( x, y,     z + 1);
			glVertex3f( x, y + 1, z + 1);
			glVertex3f( x, y + 1, z);

			glNormal3f(1, 0, 0);
			glVertex3f(-x, y,     z);
			glVertex3f(-x, y + 1, z);
			glVertex3f(-x, y + 1, z + 1);
			glVertex3f(-x, y,     z + 1);

			swap(x, y);

			clYellow.use();
			glNormal3f(0, 1, 0);
			glVertex3f(x,     -y, z);
			glVertex3f(x,     -y, z + 1);
			glVertex3f(x + 1, -y, z + 1);
			glVertex3f(x + 1, -y, z);

			glNormal3f(0, -1, 0);
			glVertex3f(x,     y, z);
			glVertex3f(x + 1, y, z);
			glVertex3f(x + 1, y, z + 1);
			glVertex3f(x,     y, z + 1);
		}
	glEnd();
	glEndList();
}

void drawScene() {
	Point lightPos = player.getPos();
    Point p(1, 1, 1);
    Color fruit(1.f, 0.f, 0.f, 1.0f);
    Color head(0.5f, 1.0f, 0.5f, 1.0f);
	frameCount++;
	prepareScene();
	glClearColor (bg.r, bg.g, bg.b, 1);

	// set Cam
	if (paused) {
		freecam.moveTo(player.getPos());
		glTranslatef(0.0, 0.0, -10);
		freecam.set();
	} else player.setCam();

    // Add lights
    GLfloat ambientColor[] = {lightAmbient, lightAmbient, lightAmbient, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightPos0[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, glm::value_ptr(player.getDir()));

    GLfloat lightPos1[] = { target.x, target.y, target.z, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    // drawing
    glTranslatef(-0.5, -0.5, -0.5);
    drawQuad(p, head);
    glTranslatef(0.5, 0.5, 0.5);

    glCallList(walls);

    drawSphere(target, 0.25, fruit);

	drawParticles();

	glutSwapBuffers(); //Send the 3D scene to the screen
}

int main(int argc, char** argv) {
	initEngine(argc, argv);

	glutKeyboardFunc(handleKeypress);
	glutPassiveMotionFunc(handleMouse);
	glutMotionFunc(handleMouse);
	glutDisplayFunc(drawScene);
    update(0);

    initWorld();

    startEngine();
	return 0;
}
