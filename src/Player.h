/*
 * Player.h
 *
 *  Created on: 19.10.2013
 *      Author: user
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "API.h"
#include "settings.h"

using namespace glm;

class Player {
	Point     _da;
	Point 	  _pos;
	glm::mat4 _rot;
	bool      _rotating;
	void doRotation();
	GLubyte   _c;
public:
	int length;
	Player() { length = EAT_VALUE; _c = 0; _rotating = false; };
	void setCam() {
		glRotatef(_da.x, 1, 0, 0);
		glRotatef(_da.y, 0, 1, 0);
		glRotatef(_da.z, 0, 0, 1);
		glMultMatrixf(glm::value_ptr(_rot));
		glTranslatef(-_pos.x, -_pos.y, -_pos.z);
	};
	void shutdown(std::string s) {
		std::cout << s << std::endl << "your final length was " << length << std::endl;
		exit(0);
	}
	void move();
	void eat() { length += EAT_VALUE; };
	void turnUp();
	void turnDown();
	void turnLeft();
	void turnRight();
	Point& getPos()   { return _pos; };
	glm::vec4 getDir() {
		glm::vec4 v(0.f, 0.f, -1.f, 1.f);
		return v * rotate(_da.x, 1.f, 0.f, 0.f) *
				   rotate(_da.y, 0.f, 1.f, 0.f) *
				   rotate(_da.z, 0.f, 0.f, 1.f) * _rot;
	};
};

class Body : Particle {
	int     lt;
	Player *player;
public:
	Body(Point pos, Player *p) : Particle(true, pos, p0, INFINITY) { lt = 0; player = p; };
	void move(float tg) {
		Point p = player->getPos() - pos;
		lt++;
		if (lt >= player->length) livetime = -1;
		else if (lt > 1 && abs(p.x) < 0.1 && abs(p.y) < 0.1 && abs(p.z) < 0.1)
			player->shutdown("Got your tail!");
	}
	void draw() {
		static Point p(1.f, 1.f, 1.f);

		glPushMatrix();
			glTranslatef(pos.x - 0.5, pos.y - 0.5, pos.z - 0.5);
			drawQuad(p, clLime);
		glPopMatrix();
	}
};

#endif /* PLAYER_H_ */
