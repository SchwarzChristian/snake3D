/*
 * settings.h
 *
 *  Created on: 18.10.2013
 *      Author: user
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "API.h"

#define FPS        60.0
#define FIELD_SIZE 5
#define EAT_VALUE  3

//physics
const float
	lightPlayer  = 0.3,
	lightTarget  = 0.5,
	moveSpeed    = 1.0,
	fov			 = 30.0 / FIELD_SIZE * 10.0
;

//keymapping
const unsigned char
	key_up      = 'w', // turn up
	key_down    = 's', // turn down
	key_left    = 'a', // turn left
	key_right   = 'd', // turn up   ...   jk, turn right ;)

	key_light   = 'l', // turn light on/off
	key_smooth  = 'e', // switch between smooth and flat shading
	key_pause   = ' ', // stop all animations
	key_break   = '#'  // for debugging
;

#endif /* SETTINGS_H_ */
