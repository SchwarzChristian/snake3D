/*
 * API.cpp
 *
 * implementation of my openGL simplification API
 *
 *  Created on: 16.10.2013
 *      Author: Railion
 */

#include <GL/glut.h>
#include <math.h>

#include "API.h"

List<Particle> _particles;

int _w = 0, _h = 0;

void shutdown(std::string s) {
	std::cout << s << std::endl;
	exit(0);
}

//----- class Point_<t> -----

template <class t>
Point_<t>::Point_(t x, t y, t z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

template <class t>
Point_<t>& Point_<t>::set(t x, t y, t z) {
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
}

template <class t>
Point_<t>& Point_<t>::mod(t x, t y, t z) {
	this->x += x;
	this->y += y;
	this->z += z;
	return *this;
}

template <class t>
Point_<t>& Point_<t>::mod(Point_<t> p) {
	return mod(p.x, p.y, p.z);
}

template <class t>
glm::vec3 Point_<t>::toVec3() {
	glm::vec4 v(x, y, z);
	return v;
}

template <class t>
glm::vec4 Point_<t>::toVec4() {
	glm::vec4 v(x, y, z, 1.f);
	return v;
}

//----- class Map -----
#ifdef GL_API_EXPERIMENTAL

Map::Map(bool rotMax, int w, int h, Point const& offset, float* map, Color cl) : cam(rotMax) {
	if (w > 0 && h > 0 && map) init(map, w, h, offset, cl);
}

Map::~Map() {
	delete [] _map;
	delete [] _idx;
	delete [] _normals;
}

void Map::setCam(bool allowFly, float hover) {
	Point& p = cam.getPos();
	if (!allowFly || p.z < getHeight(p.x, p.y) + hover)
		set(p, hover);
}

void Map::set(Point& p, float hover) {
	p.z = hover + getHeight(p.x, p.y);
}


//*/ changes the given control point
void Map::mod(int x, int y, float value) {
	x += _offset.x; y += _offset.y;
	_map[3*(_w*x+y)+2] += value;
	_calcNormal(x, y);
	_calcNormal(x + 1, y + 1);
	_calcNormal(x - 1, y - 1);
	_calcNormal(x + 1, y - 1);
	_calcNormal(x - 1, y + 1);
}
//*
void Map::_calcNormal(GLuint i, GLuint j) {
	if (i < _w - 1 && i > 0 && j > 0 && j < _h - 1) {
		Point
			p1(i - 1, j - 1, _map[3*(_w*(i-1)+j-1)+2]),
			p2(i + 1, j - 1, _map[3*(_w*(i+1)+j-1)+2]),
			p3(i - 1, j + 1, _map[3*(_w*(i-1)+j+1)+2]),
			p4(i + 1, j + 1, _map[3*(_w*(i+1)+j+1)+2]),
			v2 = p4 - p1,
			v1 = p2 - p3
		;
		_normals[(i*_w+j)].set(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, 8).normalize();
	} else _normals[(i*_w+j)].set(0, 0, 1);
	std::cout << i << "," << j << ": " <<
			_normals[(i*_w)+j].x << "|" << _normals[(i*_w)+j].y << "|" << _normals[(i*_w)+j].z << std::endl;
}//*/

void Map::init(GLfloat* map, int w, int h, Point const& offset, Color cl) {
	_w = w;
	_h = h;
	_map = new GLfloat[3*w*h];
	_tex = new GLfloat[w*h*2];
	_idx = new GLuint[(w-1)*(h-1)*6];
	_normals = new Point[w*h];
	_cl = cl;
	_offset = p0 - offset;

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++) {
			_map[3*(w*i+j)  ] = i;
			_map[3*(w*i+j)+1] = j;
			_map[3*(w*i+j)+2] = map[w*i+j];
			_tex[(w*i+j)*2  ] = i * 2;
			_tex[(w*i+j)*2+1] = j * 2;
		}

//* polygonal
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			_calcNormal(i, j);

	for (int i = 0; i < h - 1; i++)
		for (int j = 0; j < w - 1; j++) {
			_idx[(i*(h-1)+j)*6  ] =  i      * h + j;
			_idx[(i*(h-1)+j)*6+1] = (i + 1) * h + j;
			_idx[(i*(h-1)+j)*6+2] =  i      * h + j + 1;

			_idx[(i*(h-1)+j)*6+3] = (i + 1) * h + j;
			_idx[(i*(h-1)+j)*6+4] = (i + 1) * h + j + 1;
			_idx[(i*(h-1)+j)*6+5] =  i      * h + j + 1;
		}
/*/// bezier
	const GLfloat texcoord[] = {
			0.0, 0.0,  0.0, 4.0,
			4.0, 0.0,  4.0, 4.0
	};

	glMap2f(GL_MAP2_VERTEX_3, 0.f, 1.f, 3, _w, 0.f, 1.f, 3 * _w, _h, _map);
//	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 0.5, 2, 2, 0, 0.5, 4, 2, texcoord);
	glMapGrid2f(1 * _w, 0, (float)_w, 1 * _h, 0.0, (float)_h);
	glEnable(GL_MAP2_VERTEX_3);
//	glEnable(GL_MAP2_TEXTURE_COORD_2);


//*/
}

void Map::generate(int w, int h, Point const& offset, Color cl) {
	float buffer[w][h];
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			buffer[i][j] =	i < 1
								? j < 1
									? 0
									: buffer[i][j-1]
								: j < 1
									? buffer[i-1][j]
									:((buffer[i-1][j] + buffer[i][j-1]) / 2)
							+ rnd(-0.5, 0.5);
	init(buffer[0], w, h, offset, cl);
}

void Map::draw(GLuint x1, GLuint y1, GLuint x2, GLuint y2) {
	if (x2 == 0) x2 = _w;
	if (y2 == 0) y2 = _h;

//* polygonal
	glVertexPointer(3, GL_FLOAT, 0, _map);
	setNP(_normals, _w * _h);
	setTP(_tex,     _w * _h);
	_cl.use();
	glPushMatrix();
	glTranslatef(-_offset.x, -_offset.y, -_offset.z);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	_draw(_idx+((y1*_w+x1)*6), (x2-x1-1)*(y2-y1-1)*6, GL_TRIANGLES);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();

/*/// bezier
	glPushMatrix();
//	glTranslatef(-_offset.x, -_offset.y, -_offset.z);
	_cl.use();
//	glEvalMesh2(GL_FILL, x1, x2, y1, y2);
	glEvalMesh2(GL_FILL, 0.0, 10.0, 0.0, 10.0);
	glPopMatrix();
//*/
}

float Map::getHeight(float x, float y) {
	x += _offset.x, y  += _offset.y;
	unsigned int _x = floor(x), _y = floor(y);
	if ( _x < 0 || _x >= _w || _y < 0 || _y >= _h) return NAN;
			// z = ((m_1 * x + n_1) + (m_2 * x + n_2)) / 2
			//------------------ m --------------------\   /-- x -\   /------- n ------\.
	return -_offset.z + //*
		   (((_map[3*(_w*(_x+1)+_y)+2] - _map[3*(_w*_x+_y+1)+2]) * (x - _x) + _map[3*(_w*_x+_y+1)+2]) +
			((_map[3*(_w*(_x+1)+_y+1)+2] - _map[3*(_w*_x+_y)+2]) * (x - _x) + _map[3*(_w*_x+_y)+2])) / 2.0;
			//*/
}
#endif

//----- class Particle -----

Particle::Particle(bool autoUpdate, Point const& p, Point const& s, float const& lt) {
	pos = p; speed = s; livetime = lt;
	if (autoUpdate) _particles.insertBehind(this);
}

void drawParticles() {
	Particle *p;
	if (_particles.getFirst())
		while (!_particles.iswrapped()) {
			p = _particles.getCurr();
			if (p->livetime > 0.0 || p-> livetime == INFINITY) p->draw();
			else _particles.remove();
			_particles++;
		}
}

void moveParticles(float x) {
	Particle *p;
	if (_particles.getFirst())
		while (!_particles.iswrapped()) {
			p = _particles.getCurr();
			if (p->livetime > 0.0 || p-> livetime == INFINITY) p->_move(x);
			else _particles.remove();
			_particles++;
		}
}

void initEngine(int argc, char** argv) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400); //Set the window size

	// Create the window
	glutCreateWindow("test");

	// openGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

#ifdef GL_API_EXPERIMENTAL
	glEnable(GL_AUTO_NORMAL);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
//	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif

	// cilent settings
	glEnableClientState(GL_VERTEX_ARRAY);

	// misc settings
	glPolygonMode(GL_BACK, GL_LINE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);
}

//Initializes 3D rendering
//Called when the window is resized
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	_w = w;
	_h = h;

	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective

	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(GL_API_FOV, (double)w / (double)h, GL_API_NEAR_CLIP, GL_API_FAR_CLIP);
}

void startEngine() {
	glutReshapeFunc(handleResize);
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
}

void prepareScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef GL_API_EXPERIMENTAL
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif

	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(GL_API_FOV, (double)_w / (double)_h, GL_API_NEAR_CLIP, GL_API_FAR_CLIP);

	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective
}

//----- math funcs -----
float dsin(float a) {
	return sin(a * 3.1415 / 180);
}

float dcos(float a) {
	return cos(a * 3.1415 / 180);
}

template <class t>
void swap(t& a, t& b) {
	 t tmp = a;
	 a = b;
	 b = tmp;
}

float rnd() {
	return rand() / float(RAND_MAX);
}

float rnd(float x) {
	return rnd() * x;
}

float rnd(float x, float y) {
	return rnd(std::max(x, y) - std::min(x, y)) + std::min(x, y);
}

int rnd(int x) {
	return rnd() * x;
}

template <class t>
t power(t const& b, GLuint e) {
	int ret = 1;
	for (GLuint i = 0; i < e; i++) ret *= b;
	return ret;
}

void setVP(Point const* pts, int count) {
	static GLfloat *va;
	va = (float*)realloc(va, sizeof(float) * 3 * count);
	for (int i = 0; i < count; i++) {
		va[3*i]   = pts[i].x;
		va[3*i+1] = pts[i].y;
		va[3*i+2] = pts[i].z;
	}
	glVertexPointer(3, GL_FLOAT, 0, va);
}
//*/
void setNP(Point const* pts, int count) {
	static GLfloat *na;
	na = (float*)realloc(na, sizeof(float) * 3 * count);
	if (!na) return;
	for (int i = 0; i < count; i++) {
		na[3*i]   = pts[i].x;
		na[3*i+1] = pts[i].y;
		na[3*i+2] = pts[i].z;
	}
	glNormalPointer(GL_FLOAT, 0, na);
}

void setTP(GLfloat *tex, int count) {
	static GLfloat *ta;
	ta = (float*)realloc(ta, sizeof(float) * 2 * count);
	if (!ta) return;
	for (int i = 0; i < 2 * count; i++) {
		ta[i] = tex[i];
	}
	glTexCoordPointer(2, GL_FLOAT, 0, ta);
}

//----- class Cam -----

void Cam::_wrapAngles() {
	if (_rotMax) {
		_angle.x = std::max(std::min(_angle.x, 180.0f), 0.0f);
	} else if (abs(_angle.x) > 180) _angle.x -= 360 * _angle.x / abs(_angle.x);
	if (abs(_angle.y) > 180) _angle.y -= 360 * _angle.y / abs(_angle.y);
	if (abs(_angle.z) > 180) _angle.z -= 360 * _angle.z / abs(_angle.z);
}

Cam::Cam(bool rotMax) {
	this->_rotMax = rotMax;
}

Cam& Cam::update(float timegap) {
	_pos   += movespeed * timegap;
	_angle += rotspeed * timegap;
	moveRel(movespeedRel);
	return *this;
}

Cam& Cam::move(GLdouble const& x, GLdouble const& y, GLdouble const& z) {
	_pos.mod(x, y, z);
	return *this;
}

Cam& Cam::move(Point const& p) {
	_pos.mod(p);
	return *this;
}

Cam& Cam::moveTo(GLdouble const& x, GLdouble const& y, GLdouble const& z) {
	_pos.set(x, y, z);
	return *this;
}

Cam& Cam::moveTo(Point const& p) {
	_pos = p;
	return *this;
}

Cam& Cam::moveRel(GLfloat const& x, GLfloat const& y, GLfloat const& z) {
	glm::mat4 r =  glm::rotate(_angle.z, 0.f, 0.f, 1.f) *
				   glm::rotate(_angle.y, 0.f, 1.f, 0.f) *
				   glm::rotate(_angle.x, 1.f, 0.f, 0.f);
	glm::mat4 t1 = glm::translate(x, y, z);
	glm::mat4 t2 = glm::translate(_pos.x, _pos.y, _pos.z);
	glm::vec4 v(0.f, 0.f, 0.f, 1.f);
	v = t1 * v;
	v = r  * v;
	v = t2 * v;
	_pos.set(v.x, v.y, v.z);
	return *this;
}

Cam& Cam::moveRel(Point const& p) {
	return moveRel(p.x, p.y, p.z);
}

Cam& Cam::rotate(GLfloat const& x, GLfloat const& y, GLfloat const& z) {
	_angle.mod(x, y, z);
	_wrapAngles();
	return *this;
}

Cam& Cam::rotate(Point const& p) {
	_angle.mod(p);
	_wrapAngles();
	return *this;
}

Point& Cam::getPos() {
	return _pos;
}

Point& Cam::getAngle() {
	return _angle;
}

void Cam::set() {
	glRotatef(-_angle.x, 1, 0, 0);
	glRotatef(-_angle.y, 0, 1, 0);
	glRotatef(-_angle.z, 0, 0, 1);
	glTranslatef(-_pos.x, -_pos.y, -_pos.z);
}

void Cam::setAngle(Point p) {
	_angle = p;
}

//----- class Color -----

Color::Color(int cl) {
	this->r = (cl & 0xFF000000) / 0xFF000000;
	this->g = (cl & 0xFF0000) / 0xFF0000;
	this->b = (cl & 0xFF00) / 0xFF00;
	this->a = (cl & 0xFF) / 0xFF;
	_check();
}

Color::Color(int r, int g, int b, int a) {
	this->r = r / 255; this->g = g / 255;
	this->b = b / 255; this->a = a / 255;
	_check();
}

Color::Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	this->r = r; this->g = g;
	this->b = b; this->a = a;
	_check();
}

void Color::use() const {
	glColor4f(r, g, b, a);
}

void Color::set(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
	_check();
}

void Color::mod(float r, float g, float b, float a) {
	this->r += r;
	this->g += g;
	this->b += b;
	this->a += a;
	_check();
}

void Color::_check() {
	if (r > 1.0) r = 1.0;
	if (g > 1.0) g = 1.0;
	if (b > 1.0) b = 1.0;
	if (a > 1.0) a = 1.0;
	if (r < 0.0) r = 0.0;
	if (g < 0.0) g = 0.0;
	if (b < 0.0) b = 0.0;
	if (a < 0.0) a = 0.0;
}

//----- class Particle -----

void Particle::_move(float timegap) {
	float tg = timegap;
	if ((livetime -= tg) <= 0) return;
	pos.mod(speed.x * tg, speed.y * tg, speed.z * tg);
	move(timegap);
}



//----- drawing functions -----
void _draw(GLuint const* indices, int count, int mode) {
	glDrawElements(mode, count, GL_UNSIGNED_INT, indices);
}

void _draw(int count, int mode) {
	glDrawArrays(mode, 0, count);
}

void drawRect(Point len, Color const& fill) {
	static GLuint id = 0;

	if (len.x == 0) len.x = 1.0;
	if (len.y == 0) len.y = 1.0;

	glPushMatrix();
	glScalef(len.x, len.y, 1.f);

	if (id) glCallList(id);
	else {
		id = glGenLists(1);
		glNewList(id, GL_COMPILE_AND_EXECUTE); glBegin(GL_QUADS);
			fill.use();
			glNormal3f(0, 0, 1);
			glTexCoord2f(0, 0); glVertex3f(-0.5,  0.5, 0.0);
			glTexCoord2f(0, 1); glVertex3f(-0.5, -0.5, 0.0);
			glTexCoord2f(1, 1); glVertex3f( 0.5, -0.5, 0.0);
			glTexCoord2f(1, 0); glVertex3f( 0.5,  0.5, 0.0);
		glEnd(); glEndList();
	}
	glPopMatrix();
}

void drawCircle(Point const& p, GLfloat r, Color edge) {
	static int list = -1;

	if (list < 0) {
		list = glGenLists(1);
		glNewList(list, GL_COMPILE); glBegin(GL_LINE_LOOP);
			for (int i = 0; i < 36; i++)
				glVertex3f(dsin(10.0 * i), dcos(10.0 * i), 0);
			glEnd(); glEndList();
	}

	edge.use();

	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	glScalef(r, r, 1.0);
	glCallList(list);
	glPopMatrix();
}

void drawQuad(Point len, Color const& fill) {
	static unsigned int id = 0;
	if (len.x == 0.f) len.x = 1.f;
	if (len.y == 0.f) len.y = 1.f;
	if (len.z == 0.f) len.z = 1.f;
	glPushMatrix();
	glTranslatef(len.x / 2.f, len.y / 2.f, len.z / 2.f);
	glScalef(len.x, len.y, len.z);
	fill.use();

	if (id > 0) glCallList(id);
	else {
		id = glGenLists(1);

		glNewList(id, GL_COMPILE_AND_EXECUTE); glBegin(GL_QUADS);
			// front
			glNormal3f(0, -1,   0);
			glVertex3f(-1.f, -1.f,  1.f);
			glVertex3f(-1.f, -1.f, -1.f);
			glVertex3f( 1.f, -1.f, -1.f);
			glVertex3f( 1.f, -1.f,  1.f);

			// back
			glNormal3f(0, 1, 0);
			glVertex3f( 1.f,  1.f,  1.f);
			glVertex3f( 1.f,  1.f, -1.f);
			glVertex3f(-1.f,  1.f, -1.f);
			glVertex3f(-1.f,  1.f,  1.f);

			// left
			glNormal3f(-1, 0, 0);
			glVertex3f(-1.f,  1.f,  1.f);
			glVertex3f(-1.f,  1.f, -1.f);
			glVertex3f(-1.f, -1.f, -1.f);
			glVertex3f(-1.f, -1.f,  1.f);

			// right
			glNormal3f(1, 0, 0);
			glVertex3f( 1.f, -1.f,  1.f);
			glVertex3f( 1.f, -1.f, -1.f);
			glVertex3f( 1.f,  1.f, -1.f);
			glVertex3f( 1.f,  1.f,  1.f);

			// top
			glNormal3f(0, 0, 1);
			glVertex3f(-1.f,  1.f,  1.f);
			glVertex3f(-1.f, -1.f,  1.f);
			glVertex3f( 1.f, -1.f,  1.f);
			glVertex3f( 1.f,  1.f,  1.f);

			// bottom
			glNormal3f(0, 0, -1);
			glVertex3f( 1.f,  1.f, -1.f);
			glVertex3f( 1.f, -1.f, -1.f);
			glVertex3f(-1.f, -1.f, -1.f);
			glVertex3f(-1.f,  1.f, -1.f);

		glEnd(); glEndList();

	}
	glPopMatrix();
}

void drawNoSphere(Point m, GLdouble r, Color fill) {
	glPushMatrix();
	glTranslatef(m.x, m.y, m.z);
	glBegin(GL_TRIANGLE_STRIP);
		fill.use();
		glNormal3f(0, 0, -1);
		glVertex3f(0, 0, -r);
		for (int j = 0; j < 6; j++)
			for (int i = 0; i < 24; i++) {
				glNormal3f(-cos((j+i%2) * 30) * dsin(30 * (i/2+1)), dcos(30 * (i/2+1)), -dsin((j+i%2) * 30));
				glVertex3f(-cos((j+i%2) * 30) * dsin(30 * (i/2+1)) * r, dcos(30 * i) * r, -dsin((j+i%2) * 30) * r);
			}
	glEnd();
	glPopMatrix();
}

// V2.1
void drawSphere(Point m, GLdouble r, Color fill) {
#ifdef GL_API_EXPERIMENTAL_SPHERE
	static Point va[66];
	static GLuint idx[162];
	static bool first = true;
	float a = 22.5, a2;
	int x = 1;

	fill.use();

	if (first) {
		// calc points
		va[0].set(0, 0, -1);
		for (int i = -3; i <= 3; i++) {
			for (int j = 0; j < (4 - abs(i)) * 4; j++) {
				a2 = 90 / (4 - abs(i)) * j;
				va[x].set(dcos(i * a) * dsin(a2), dcos(i * a) * dcos(a2), dsin(i * a));
				x++;
			}
		}
		va[65].set(0, 0, 1);

		idx[0] = 0;
		for (int i = 1; i <= 4; i++) idx[i] = i;
		idx[5] = 1;

		idx[6] = 12;
		for (int i = 0; i < 8 * 2; i++) { // idx[5] - idx[20] -> 16 tri_strip
			idx[7+i] = 5 - (4 * (i % 2)) + (i / 2 - ((i % 2) * i / 4)) * (i == 15 ? 0 : 1);
		}

		idx[23] = 24;
		for (int i = 0; i < 12 * 2; i++) { // idx[21] - idx[44] -> 24 tri_strip
			idx[24+i] = 13 - (8 * (i % 2)) + (i / 2 - ((i % 2) * i / 6)) * (i == 23 ? 0 : 1);
		}

		idx[48] = 40;
		for (int i = 0; i < 16 * 2; i++) { // idx[45] - idx[76] -> 32 tri_strip
			idx[49+i] = 25 - (12 * (i % 2)) + (i / 2 - ((i % 2) * i / 8)) * (i == 31 ? 0 : 1);
		}

		for (int i = 0; i < 16 * 2; i++) { // idx[77] - idx[108] -> 32 tri_strip
			idx[112-i] = 25 + (16 * (i % 2)) + (i / 2 - ((i % 2) * i / 8)) * (i == 31 ? 0 : 1);
		} idx[113] = 40;

		for (int i = 0; i < 12 * 2; i++) { // idx[109] - idx[132] -> 24 tri_strip
			idx[137-i] = 41 + (12 * (i % 2)) + (i / 2 - ((i % 2) * i / 6)) * (i == 23 ? 0 : 1);
		} idx[138] = 52;

		for (int i = 0; i < 8 * 2; i++) { // idx[133] - idx[148] -> 16 tri_strip
			idx[154-i] = 53 + (8 * (i % 2)) + (i / 2 - ((i % 2) * i / 4)) * (i == 15 ? 0 : 1);
		} idx[155] = 60;

		idx[156] = 65;
		for (int i = 1; i <= 4; i++) idx[156+i] = 65 - i;
		idx[161] = 64;

		first = false;
	}

	setVP(va, 66);
	setNP(va, 66);

	glPushMatrix();
	glTranslatef(m.x, m.y, m.z);
	glScalef(r, r, r);
/*
 * #				65
 * ####				61
 * ########			53
 * ############		41
 * ################	25
 * ############		13
 * ########			 5
 * ####				 1
 * #				 0
 */

	glEnableClientState(GL_NORMAL_ARRAY);

	_draw(idx, 6, GL_TRIANGLE_FAN);

	_draw(idx + 6, 17, GL_TRIANGLE_STRIP);
	_draw(idx +23, 25, GL_TRIANGLE_STRIP);
	_draw(idx +48, 33, GL_TRIANGLE_STRIP);
	_draw(idx+ 81, 33, GL_TRIANGLE_STRIP);
	_draw(idx+114, 25, GL_TRIANGLE_STRIP);
	_draw(idx+139, 17, GL_TRIANGLE_STRIP);

	_draw(idx+156, 6, GL_TRIANGLE_FAN);

	glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();
#else
	fill.use();
	glPushMatrix();
	glTranslatef(m.x, m.y, m.z);
	glutSolidSphere(r, 30, 20);
	glPopMatrix();
#endif
}

void drawFakeSphere(Point const& m, GLdouble r, Point const& camAngle, int pts, Color const& fill) {
	float a = 360.0 / pts;
	glPushMatrix();

	glTranslatef(m.x, m.y, m.z);
	glRotatef(camAngle.x + 90.0, 1, 0, 0);
	glRotatef(camAngle.y, 0, 1, 0);
	glRotatef(camAngle.z, 0, 0, 1);

	glBegin(GL_TRIANGLE_FAN);
		fill.use();
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		for (int i = 0; i < pts; i++) {
			glNormal3f(dcos(i * a), dsin(i * a), 0.0);
			glVertex3f(r * dcos(i * a), r * dsin(i * a), 0.0);
		}
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(r, 0.0, 0.0);
	glEnd();

	glPopMatrix();
}

void drawLine(Point const* pt, GLuint count, Color cl) {
	glBegin(GL_LINE_STRIP);
		cl.use();
		for (GLuint i = 0; i < count; i++) glVertex3f(pt[i].x, pt[i].y, pt[i].z);
	glEnd();
}

void drawWheel(GLdouble d, int s, Color fill) {
	static Point va[26], na[26];
	static bool first = true;
	GLfloat a = 360.0 / s;
	GLuint idx[23];

//*/
	if (first) {
		// front site
		va[ 0].set(0, 0, d);
		va[ 1].set(0, 0, 1 - d);
		va[ 2].set(0, 0, 1);
		va[ 3].set(d * dsin(a/2), 0, d * dcos(a/2));
		va[ 4].set(2 * d * dsin(a/5), 0, 2 * d * dcos(a/5));
		va[ 5].set((1 - d) * dsin(a/5), 0, (1 - d) * dcos(a/5));
		va[ 6].set((1 - d) * dsin(a/2), 0, (1 - d) * dcos(a/2));
		va[ 7].set(dsin(a/2), 0, dcos(a/2));
		va[ 8].set(d * dsin(a), 0, d * dcos(a));
		va[ 9].set(2 * d * dsin(a), 0, 2 * d * dcos(a));
		va[10].set((1 - d) * dsin(a), 0, (1 - d) * dcos(a));
		va[11].set(dsin(a), 0, dcos(a));
		va[12].set(0, 0, 2 * d);

		//back side
		va[13].set(0, d, d);
		va[14].set(0, d, 1 - d);
		va[15].set(0, d, 1);
		va[16].set(d * dsin(a/2), d, d * dcos(a/2));
		va[17].set(2 * d * dsin(a/5), d, 2 * d * dcos(a/5));
		va[18].set((1 - d) * dsin(a/5), d, (1 - d) * dcos(a/5));
		va[19].set((1 - d) * dsin(a/2), d, (1 - d) * dcos(a/2));
		va[20].set(dsin(a/2), d, dcos(a/2));
		va[21].set(d * dsin(a), d, d * dcos(a));
		va[22].set(2 * d * dsin(a), d, 2 * d * dcos(a));
		va[23].set((1 - d) * dsin(a), d, (1 - d) * dcos(a));
		va[24].set(dsin(a), d, dcos(a));
		va[25].set(0, d, 2 * d);

		na[0].set(0, -1, 0);
		na[13] = na[0];
		na[2].set(0, 1, 0);
		na[15] = na[2];
		na[3].set(-dsin(a / 10), -dcos(a / 10), 0);
		na[16] = na[3];
		na[5]  = na[3];
		na[18] = na[3];
		na[6].set(-dsin(a / 2), -dcos(a / 2), 0);
		na[19] = na[6];
		na[7]  = p0 - na[6];
		na[20] = na[7];
		na[8].set(-dsin(a), -dcos(a), 0);
		na[21] = na[8];
		na[9]  = p0 - na[8];
		na[22] = na[9];
		na[10] = na[8];
		na[23] = na[10];
		na[11] = na[9];
		na[24] = na[9];
		first = false;
	}

	setVP(va, 26);
	setNP(na, 26);

	for (int i = 0; i < s; i++) {
		// front
		glNormal3f(0, -1, 0);
		idx[ 0] =  0;
		idx[ 1] =  3;
		idx[ 2] =  4;
		idx[ 3] = 12;

		idx[ 4] =  3;
		idx[ 5] =  8;
		idx[ 6] =  9;
		idx[ 7] =  4;

		idx[ 8] = 12;
		idx[ 9] =  4;
		idx[10] =  5;
		idx[11] =  1;

		idx[12] =  1;
		idx[13] =  5;
		idx[14] =  7;
		idx[15] =  2;

		idx[16] =  5;
		idx[17] =  6;
		idx[18] = 11;
		idx[19] =  7;

		idx[20] =  6;
		idx[21] = 10;
		idx[22] = 11;

		fill.use();
		_draw(idx, 20, GL_QUADS);

		_draw(idx + 20, 3, GL_TRIANGLES);

		// back
		glNormal3f(0, 1, 0);
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 4; j++)
				idx[4*i+j] += 13;
			swap(idx[4*i], idx[4*i+3]);
			swap(idx[4*i+1], idx[4*i+2]);
		}
		for (int i = 20; i < 23; i++)
			idx[i] += 13;
		swap(idx[21], idx[22]);

		_draw(idx, 20, GL_QUADS);
		_draw(idx + 20, 3, GL_TRIANGLES);

		// left
		glNormal3f(-1, 0, 0);

		idx[ 0] = 12;
		idx[ 1] =  1;
		idx[ 2] = 14;
		idx[ 3] = 25;

		_draw(idx, 4, GL_QUADS);

		//right
		glNormal3f(-dsin(a / 10), dcos(a / 10), 0);
		idx[0] =  5;
		idx[1] =  4;
		idx[2] = 17;
		idx[3] = 18;

		_draw(idx, 4, GL_QUADS);

		// top

		glNormal3f(dsin(a / 10),dcos(a / 10),0);
		idx[0] = 4;
		idx[1] = 9;
		idx[2] = 22;
		idx[3] = 17;

		_draw(idx, 4, GL_QUADS);

		glEnableClientState(GL_NORMAL_ARRAY);
		idx[0] = 2;
		idx[1] = 7;
		idx[2] = 20;
		idx[3] = 15;

		idx[4] =  7;
		idx[5] = 11;
		idx[6] = 24;
		idx[7] = 20;

		_draw(idx, 8, GL_QUADS);

		//bottom

		idx[ 0] =  3;
		idx[ 1] =  0;
		idx[ 2] = 13;
		idx[ 3] = 16;

		idx[ 4] =  8;
		idx[ 5] =  3;
		idx[ 6] = 16;
		idx[ 7] = 21;

		idx[ 8] =  6;
		idx[ 9] =  5;
		idx[10] = 18;
		idx[11] = 19;

		idx[12] = 10;
		idx[13] =  6;
		idx[14] = 19;
		idx[15] = 23;

		_draw(idx, 16, GL_QUADS);

		glDisableClientState(GL_NORMAL_ARRAY);
		glRotatef(a, 0, 1, 0);
	} //for

	glPopMatrix();
}
