/*
 * API.h - V 1.0
 *
 * header to my openGL simplification API
 *
 *  Created on: 16.10.2013
 *      Author: Railion
 *
 * TODO particle system class to control multiple sets of particles
 * TODO writing a simpler List class
 * TODO divide this file, the .cpp is to large (nearly 1k lines, dammit) ;)
 *
 */

/*
 * you can set the following defines before including this API
 *
 * GL_API_NO_COLOR_PREDEF	- disable color predefinition (saves some memory)
 * GL_API_EXPERIMENTAL		- enable some experimental features, partly unfinished and buggy, but try it if you want ;)
 * GL_API_FOV				- field of view (default:  60.0)
 * GL_API_NEAR_CLIP			- near clipping (default:   0.1)
 * GL_API_FAR_CLIP			- far clipping  (default: 100.0)
 *
 */

#ifndef API_H_
#define API_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <png.h>

#include "List.h"

#ifndef GL_API_FOV
#define GL_API_FOV 60.0
#endif

#ifndef GL_API_NEAR_CLIP
#define GL_API_NEAR_CLIP 0.1
#endif

#ifndef GL_API_FAR_CLIP
#define GL_API_FAR_CLIP 100.0
#endif

// default color for faces
#define FILL_COLOR clWhite

// cout message and exit
void shutdown(std::string s);

// some math helpers
// some math
float dsin(float);
float dcos(float);
float rnd();						// [0.f;1.f[ | 0.f <= x < 1.f
float rnd(float max);				// [0.f;max[ | 0.f <= x < max
float rnd(float min, float max);	// [min;max[ | min <= x < max
int   rnd(int max);					// [0;max-1] | 0   <= x < max

template <class t>
void swap(t& a, t& b);

// prepare engine; see API.cpp initEngine() and handleResize() to see preset configurations
void initEngine(int argc, char** argv);

// set resize handler and start glut-loop
void startEngine(void);

// initializes view
void prepareScene(void);

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Point_<t = GLfloat>                    *
 *  (you can use 'Point' instead of              *
 *  'Point_<GLfloat>')                           *
 *                                               *
 *  A vector tweak for use as points and         *
 *  angles. Maybe you'll prefer this class       *
 *  instead of vec3's. You can convert points    *
 *  to vectors with vec3 point.toVec3() and      *
 *  vec4 point.toVec4().                         *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */
template <class t>
class Point_ {
public:
	t x;
	t y;
	t z;
	Point_(t x = 0, t y = 0, t z = 0);

	// sets the coordinates
	Point_<t>& set(t x, t y = 0, t z = 0);

	// modifies the coordinates (+=)
	Point_<t>& mod(t x, t y = 0, t z = 0);
	Point_<t>& mod(Point_<t> p);

	// change to normalized vector (with length of 1.0)
	Point_<t>& normalize(void){	return *this *= 1.0 / sqrt(x*x + y*y + z*z); };

	// convert to glm::vec*
	glm::vec3 toVec3(void);
	glm::vec4 toVec4(void);

	// operators
	Point_<t>  operator +  (Point_<t> const& op) const { Point_<t> p(x + op.x, y + op.y, z + op.z); return p; };
	Point_<t>  operator -  (Point_<t> const& op) const { Point_<t> p(x - op.x, y - op.y, z - op.z); return p; };
	Point_<t>  operator *  (float     const& op) const { Point_<t> p(x * op,   y * op,   z * op);   return p; };
	Point_<t>& operator =  (Point_<t> const& op) { x = op.x; y = op.y; z = op.z; return *this; };
	Point_<t>& operator += (Point_<t> const& op) { mod( op.x,  op.y,  op.z);     return *this; };
	Point_<t>& operator -= (Point_<t> const& op) { mod(-op.x, -op.y, -op.z);     return *this; };
	Point_<t>& operator *= (float     const& op) { x *= op; y *= op; z *= op;    return *this; };
};

// shortcut for float points
typedef Point_<GLfloat> Point;

// origin point for quick use
Point const p0;

// put a Point_<t> array into a vertex array and call glVertexPointer() for it
void setVP(Point const* pts, int count);

// the same with normal arrays
void setNP(Point const* pts, int count);

// and texture arrays
void setTP(GLfloat *tex, int count);

// draw the current vertex array (make sure to call 'glEnable(GL_NORMAL_ARRAY)' if you need normals)
void _draw(GLuint const* indices, int count, int mode);
void _draw(int count, int mode);

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Cam                                    *
 *                                               *
 *  A class to manage camera control.            *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */
class Cam {
	Point _pos;
	Point _angle;

	// stop rotating at x = +- 90°
	bool  _rotMax;

	// hold angles between +- 180°
	void _wrapAngles();
public:
	Point movespeed;
	Point rotspeed;
	Point movespeedRel;

	Cam(bool rotMax = true);

	//moves and rotates the cam by the given speeds
	Cam& update(float timegap);

	// set cam position relative to current position and local origins orientation
	Cam& move(GLdouble const& x, GLdouble const& y, GLdouble const& z);
	Cam& move(Point const& p);

	// set cam position relative to the local origins position and orientation
	Cam& moveTo(GLdouble const& x, GLdouble const& y, GLdouble const& z);
	Cam& moveTo(Point const& p);

	// set cam position relative to current position and orientation
	Cam& moveRel(GLfloat const& x, GLfloat const& y, GLfloat const& z);
	Cam& moveRel(Point const& p);

	// set the angles relative to the current orientation
	Cam& rotate(GLfloat const& x, GLfloat const& y, GLfloat const& z);
	Cam& rotate(Point const& p);

	// sets the angles relative to the local origins orientation
	void setAngle(Point p);

	// get current position and orientation
	Point& getPos(void);
	Point& getAngle(void);

	// do the transformations to move and rotate the cam relative to the local origin
	void set(void);
};


/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Color                                  *
 *                                               *
 *  A class to manage colors a bit easier then   *
 *  openGL's colors does.                        *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */
class Color {
	// check if all values are between 0.0 and 1.0
	void _check(void);
public:
	GLfloat r; // red
	GLfloat g; // green
	GLfloat b; // blue
	GLfloat a; // alpha

	// RGBA-Format; use values like '0x00FF0088' (half-transparent lime)
	Color(int cl = 0);

	// byte format; use values like '0, 255, 0, 127' (half-transparent lime)
	Color(int r, int g, int b, int a = 255);

	// openGL-like format; use values like '0.f, 1.f, 0.f, 0.5f' (half-transparent lime)
	Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0);

	// use this color for following vertices.
	void use(void) const;

	// set / modify the color values
	void set(float r, float g = 0.0, float b = 0.0, float a = 0.0);
	void mod(float r, float g = 0.0, float b = 0.0, float a = 0.0);
};

// color predefinition
#ifndef GL_API_NO_COLOR_PREDEF
//					    ---R---|---G---|---B---|---A---
Color const clBlack     (0xFF);
Color const clBlue		(0.0f,  0.0f,   1.0f);
Color const clDarkGrey  (0.75f, 0.75f,  0.75f);
Color const clGreen     (0.0f,  0.5f,   0.0f);
Color const clGrey      (0.5f,  0.5f,   0.5f);
Color const clHTWYellow (255,   148,    56);
Color const clLightGrey (0.25f, 0.25f,  0.25f);
Color const clLime      (0,     255,    0);
Color const clNone      (1.0f,  1.0f,   1.0f,   0.0f);
Color const clRed       (1.0f,  0.0f,   0.0f);
Color const clSkyBlue   (0.5f,  0.75f,  1.0f);
Color const clYellow   (1.0f,  1.0f,   0.0f);
Color const clWhite     (1.0f,  1.0f,   1.0f);
//					    ---R---|---G---|---B---|---A---
#endif

#ifdef GL_API_EXPERIMENTAL
/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Map                                    *
 *                                               *
 *  A class i'm just working on. It should be    *
 *  used to encapsulate camera control and to    *
 *  deal with terrain, fog and so on...          *
 *  Terrain generation and rendering already     *
 *  works. Define GL_API_EXPERIMENTAL to use.    *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */
// TODO class Map: unfinished
// TODO class Map: fog unfinished
class Map {
	Point   *_normals;
	Point    _offset;
	GLfloat *_map;
	GLfloat *_tex;
	GLuint  *_idx;
	GLuint   _w;
	GLuint   _h;
	Color    _cl;
	void     _calcNormal(GLuint x, GLuint y);
public:
	Cam cam;
	Map(bool rotMax = true, int w = 0, int h = 0, Point const& offset = p0, float* map = NULL, Color cl = clGreen);
	~Map(void);

	// sets the cam hover units above the ground if beyond or not allowFly
	void setCam(bool allowFly = true, float hover = 1.0);

	// changes the z value to map height + hover
	void set(Point& p, float hover = 0.f);

	// changes the given control point
	void mod(int x, int y, float value);

	// calculates vertices and index list
	void init(GLfloat* map, int w, int h, Point const& offset = p0, Color cl = clGreen);

	// generates a random map
	void generate(int w, int h, Point const& offset = p0, Color cl = clGreen);

	// draws the map
	void draw(GLuint x1 = 0, GLuint y1 = 0, GLuint x2 = 0, GLuint y2 = 0);

	// get interpolated height
	float getHeight(float x, float y);
};

class Texture {
	png_uint_32 _w;
	png_uint_32 _h;
	GLuint      _id;
	bool	    _hasA;
public:
	GLubyte *_image;

	Texture() {
		_image = NULL;
		_id = 0;
		_w = 0;
		_h = 0;
		_hasA = false;
	}

	void load(std::string const& filename) {
		png_structp png;
		png_infop   info;
		GLuint      sig = 0;
		int         colorType, interlanceType;
		FILE       *f;

		if (!(f = fopen(filename.c_str(), "rb"))) return;

		if (!(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) {
			fclose(f);
			return;
		}

		if (!(info = png_create_info_struct(png))) {
			fclose(f);
			png_destroy_read_struct(&png, NULL, NULL);
			return;
		}

		if (setjmp(png_jmpbuf(png))) {
			fclose(f);
			png_destroy_read_struct(&png, &info, NULL);
			return;
		}

		png_init_io(png, f);
		png_set_sig_bytes(png, sig);
		png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

		int bitDepth;
		png_get_IHDR(png, info, &_w, &_h, &bitDepth, &colorType, &interlanceType, NULL, NULL);
		GLuint rowb = png_get_rowbytes(png, info);
		_image = new GLubyte [rowb*_h];
		png_bytepp row = png_get_rows(png, info);
		for (GLuint i = 0; i < _h; i++)
			memcpy(_image+(rowb * (_h - 1 - i)), row[i], rowb);

		png_destroy_read_struct(&png, &info, NULL);
		fclose(f);

		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, _hasA ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, _image);
	}

	void use(void) const {
		glBindTexture(GL_TEXTURE_2D, _id);
	}
};

Texture const texNone;

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Font                                   *
 *                                               *
 *  A class to render text.                      *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */

class Font {
	int     _listbase;
	GLfloat _space;
public:
	Font() {_listbase = 0; _space = 0.f; };

	void init(GLfloat space = 0.1, GLfloat w = 0.5f, GLfloat h = 1.0f) {
		_space = space + w;
		_listbase = glGenLists(128);
		glNewList(_listbase + 'A', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,       0);
				glVertex2f(0.5  * w,       h);
				glVertex2f(       w,       0);
				glVertex2f(0.75 * w, 0.5 * h);
				glVertex2f(0.25 * w, 0.5 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'B', GL_COMPILE);
			glBegin(GL_LINE_LOOP);
				glVertex2f(       0,        0);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(       0,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(       w, 0.65 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(0.75 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'C', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(       0, 0.75 * h);
				glVertex2f(       0, 0.25 * h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.75 * w,        0);
				glVertex2f(       w, 0.15 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'D', GL_COMPILE);
			glBegin(GL_LINE_LOOP);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.75 * h);
				glVertex2f(       w, 0.25 * h);
				glVertex2f(0.75 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'F', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       0,        h);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'E', GL_COMPILE);
			glCallList(_listbase + 'F');
			glBegin(GL_LINES);
				glVertex2f(       0,        0);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'G', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(       0, 0.75 * h);
				glVertex2f(       0, 0.25 * h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(       w,        0);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(0.50 * w, 0.35 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'H', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(       w,        0);
				glVertex2f(       w,        h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(       w, 0.50 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'I', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(0.50 * w,        0);
				glVertex2f(0.50 * w,        h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'J', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(0.50 * w,        h);
				glVertex2f(0.50 * w, 0.15 * h);
				glVertex2f(0.35 * w,        0);
				glVertex2f(0.15 * w,        0);
				glVertex2f(       0, 0.15 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'K', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(       w,        h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'L', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(       0,        0);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'M', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(0.50 * w, 0.50 * h);
				glVertex2f(       w,        h);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'N', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(       w,        0);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'O', GL_COMPILE);
			glBegin(GL_LINE_LOOP);
				glVertex2f(       0, 0.25 * h);
				glVertex2f(       0, 0.75 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.75 * h);
				glVertex2f(       w, 0.25 * h);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.25 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'P', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        0);
				glVertex2f(       0,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(       w, 0.65 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       0, 0.50 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'Q', GL_COMPILE);
			glCallList(_listbase + 'O');
			glBegin(GL_LINES);
				glVertex2f(0.75 * w, 0.25 * h);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'R', GL_COMPILE);
			glCallList(_listbase + 'P');
			glBegin(GL_LINES);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'S', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.15 * h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.75 * w,        0);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(0.25 * w, 0.50 * h);
				glVertex2f(       0, 0.65 * h);
				glVertex2f(       0, 0.85 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'T', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        h);
				glVertex2f(       w,        h);
				glVertex2f(0.50 * w,        h);
				glVertex2f(0.50 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'U', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(       0, 0.25 * h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.75 * w,        0);
				glVertex2f(       w, 0.25 * h);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'V', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(0.50 * w,        0);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'W', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.50 * w, 0.50 * h);
				glVertex2f(0.75 * w,        0);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + 'X', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        h);
				glVertex2f(       w,        0);
				glVertex2f(       w,        h);
				glVertex2f(       0,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'Y', GL_COMPILE);
			glBegin(GL_LINES);
				glVertex2f(       0,        h);
				glVertex2f(0.50 * w, 0.50 * h);
				glVertex2f(       w,        h);
				glVertex2f(       0,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + 'Z', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(       w,        h);
				glVertex2f(       0,        0);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + '0', GL_COMPILE);
			glCallList(_listbase + 'O');
		glEndList();
		glNewList(_listbase + '1', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(0.25 * w, 0.75 * h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(0.75 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + '2', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.75 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.75 * h);
				glVertex2f(       w, 0.50 * h);
				glVertex2f(       0,        0);
				glVertex2f(       w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + '3', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.85 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(       w, 0.65 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(0.25 * w, 0.50 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.25 * w,        0);
				glVertex2f(       0, 0.15 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + '4', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       0, 0.25 * h);
				glVertex2f(       w, 0.25 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + '5', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.15 * h);
				glVertex2f(0.25 * w,        0);
				glVertex2f(0.75 * w,        0);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(       0,        h);
				glVertex2f(       w,        h);
			glEnd();
		glEndList();
		glNewList(_listbase + '6', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.50 * h);
				glVertex2f(0.75 * w, 0.50 * h);
				glVertex2f(       w, 0.35 * h);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.25 * w,        0);
				glVertex2f(       0, 0.15 * h);
				glVertex2f(       0, 0.85 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + '7', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0,        h);
				glVertex2f(       w,        h);
				glVertex2f(0.50 * w,        0);
			glEnd();
		glEndList();
		glNewList(_listbase + '8', GL_COMPILE);
			glCallList(_listbase + '3');
			glBegin(GL_LINE_STRIP);
				glVertex2f(       0, 0.85 * h);
				glVertex2f(       0, 0.65 * h);
				glVertex2f(0.25 * w, 0.50 * h);
				glVertex2f(       0, 0.35 * h);
				glVertex2f(       0, 0.15 * h);
			glEnd();
		glEndList();
		glNewList(_listbase + '9', GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glVertex2f(       w, 0.50 * h);
				glVertex2f(0.25 * w, 0.50 * h);
				glVertex2f(       0, 0.65 * h);
				glVertex2f(       0, 0.85 * h);
				glVertex2f(0.25 * w,        h);
				glVertex2f(0.75 * w,        h);
				glVertex2f(       w, 0.85 * h);
				glVertex2f(       w, 0.15 * h);
				glVertex2f(0.75 * w,        0);
				glVertex2f(0.25 * w,        0);
				glVertex2f(       0, 0.15 * h);
			glEnd();
		glEndList();

	};
	void render(std::string s) {
		glPushMatrix();
		for (GLuint i = 0; i < s.length(); i++) {
			int letter = s[i];
			if (letter >= 32 && letter < 128) {
				glCallList(_listbase + letter);
				glTranslatef(_space, 0, 0);
			}
		}
		glPopMatrix();
	}
};

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *                                               *
 *  class Particle                               *
 *                                               *
 *  A class to deal with particles. Just         *
 *  inherit from this and implement the          *
 *  virtuals.                                    *
 *                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * */
class Particle {
public:
	Point pos;
	Point speed;
	float livetime;
	Particle(bool autoUpdate = true, Point const& p = p0, Point const& s = p0, float const& lt = 1.f);

	// automatically called if autoUpdate was true on initialization
	// takes care of moving the particle, decreasing it's live time and deleting it if necessary
	void _move(float timegap);

	virtual void move(float timegap) = 0;
	virtual void draw(void) = 0;
	virtual ~Particle(void) {};
};

// move/draw all Particles initialized with autoUpdate = true
void moveParticles(float timegap);
void drawParticles(void);

// some drawing funcs
void drawString(std::string const& s);
void drawRect(Point len = p0, Color const& fill = FILL_COLOR);
void drawCircle(Point const& p, GLfloat r, Color edge = clBlack);
void drawQuad(Point len = p0, Color const& fill = FILL_COLOR);
void drawLine(Point const* pt, GLuint count = 1);
void drawSphere(Point m, GLdouble r, Color fill = FILL_COLOR);   // don't use this, use gluSolidSphere() instead
void drawNoSphere(Point m, GLdouble r, Color fill = FILL_COLOR); // try this one ;)
void drawFakeSphere(Point const& m, GLdouble r, Point const& camAngle, int pts = 32, Color const& fill = FILL_COLOR);
void drawWheel(GLdouble d, int s = 10, Color fill = FILL_COLOR);

// that's all for now, have fun :)

#endif /* API_H_ */

/*
 * change notes:
 *
 * v 1.0
 * - first release, feature list:
 *   - functions to initialize and run graphics engine
 *   - class Point to deal with positions and angles
 *   - class Cam for camera positioning
 *   - class Color to deal with colors
 *   - class Particle to deal with particles
 *   - experimental class Map
 *   - experimental class Font
 *   - some drawing functions
 *   - some miscellaneous helper functions
 *
 */
