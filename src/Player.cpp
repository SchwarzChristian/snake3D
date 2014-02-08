/*
 * Player.cpp
 *
 *  Created on: 19.10.2013
 *      Author: user
 */

#include "Player.h"

void Player::move() {
	if (_rotating) doRotation();
	else if (_c++ > 15) {
		new Body(_pos, this);
		_c = 1;
		glm::vec4 v(0.f, 0.f, -1.f, 1.f);
		v = v * _rot;
		_pos.mod(v.x, v.y, v.z);

		if (_pos.x > FIELD_SIZE || _pos.x < -FIELD_SIZE ||
			_pos.y > FIELD_SIZE || _pos.y < -FIELD_SIZE ||
			_pos.z > FIELD_SIZE || _pos.z < -FIELD_SIZE) {
			shutdown("Game over!");
		}
		moveParticles(0);
	}
}

void Player::doRotation() {
	     if (_da.x < 0.f) _da.mod( 3.f,  0.f, 0.f);
	else if (_da.x > 0.f) _da.mod(-3.f,  0.f, 0.f);
	else if (_da.y < 0.f) _da.mod( 0.f,  3.f, 0.f);
	else if (_da.y > 0.f) _da.mod( 0.f, -3.f, 0.f);
	else {
		_da = p0;
		_rotating = false;
	}
}

void Player::turnUp() {
	if (_rotating) return;
	_rot = glm::rotate(-90.f, 1.f, 0.f, 0.f) * _rot;
	_da.set(90.f, 0.f, 0.f);
	_rotating = true;
}

void Player::turnDown() {
	if (_rotating) return;
	_rot = glm::rotate(90.f, 1.f, 0.f, 0.f) * _rot;
	_da.set(-90.f, 0.f, 0.f);
	_rotating = true;
}

void Player::turnLeft() {
	if (_rotating) return;
	_rot = glm::rotate(-90.f, 0.f, 1.f, 0.f) * _rot;
	_da.set(0.f, 90.f, 0.f);
	_rotating = true;
}

void Player::turnRight() {
	if (_rotating) return;
	_rot = glm::rotate(90.f, 0.f, 1.f, 0.f) * _rot;
	_da.set(0.f, -90.f, 0.f);
	_rotating = true;
}
