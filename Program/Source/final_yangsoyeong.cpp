#define _CRT_SECURE_NO_WARNINGS

#include<GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<iostream>

using namespace std;

#define WIRE 0
#define SHADE 1

typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned int ip[3];
} Face;

int pnum;
int fnum;
Point* mpoint = NULL;
Face* mface = NULL;
Point norm;

GLfloat angle = 0;

int moving;
int mousebegin;
int light_moving;
float scalefactor = 1.0;
int scaling = 0;
int status = 0;

float xmove = 500.0;
float ymove = 500.0;
float zmove = 500.0;
float down = 0.0;

float colorR = 0.3;
float colorG = 0.3;
float colorB = 0.3;

string fname = "c:\\data\\myModel.dat";

void InitLight() {
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { 200, 200, -200.0, 0.0 };

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

Point cnormal(Point a, Point b, Point c) {  // flat shading
	Point p, q, r;
	double val;
	p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
	q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;

	r.x = p.y * q.z - p.z * q.y;
	r.y = p.z * q.x - p.x * q.z;
	r.z = p.x * q.y - p.y * q.x;

	val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	r.x = r.x / val; r.y = r.y / val; r.z = r.z / val;
	return r;
}

void ReadModel() {
	FILE* f1;
	char s[81];
	int i;

	if (mpoint != NULL)
		delete mpoint;
	if (mface != NULL)
		delete mface;

	if ((f1 = fopen(fname.c_str(), "rt")) == NULL) { printf("No file\n"); exit(0); }
	fscanf(f1, "%s", s);
	printf("%s", s);
	fscanf(f1, "%s", s);
	printf("%s", s);
	fscanf(f1, "%d", &pnum);
	printf("%d\n", pnum);
	mpoint = new Point[pnum];

	for (i = 0; i < pnum; i++) {
		fscanf(f1, "%f", &mpoint[i].x);
		fscanf(f1, "%f", &mpoint[i].y);
		fscanf(f1, "%f", &mpoint[i].z);
		printf("%f %f %f\n", mpoint[i].x, mpoint[i].y, mpoint[i].z);
	}

	fscanf(f1, "%s", s);
	printf("%s", s);
	fscanf(f1, "%s", s);
	printf("%s", s);
	fscanf(f1, "%d", &fnum);
	printf("%d\n", fnum);

	mface = new Face[fnum];
	for (i = 0; i < fnum; i++) {
		fscanf(f1, "%d", &mface[i].ip[0]);
		fscanf(f1, "%d", &mface[i].ip[1]);
		fscanf(f1, "%d", &mface[i].ip[2]);
		printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void axis() {
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);  // draw x-axis
	glVertex3f(0.0, -100.0, 0.0);
	glVertex3f(160.0, -100.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);  // draw y-axis
	glVertex3f(0.0, -100.0, 0.0);
	glVertex3f(0.0, 60.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);  // draw z-axis
	glVertex3f(0.0, -100.0, 0.0);
	glVertex3f(0.0, -100.0, 160.0);
	glEnd();
	glLineWidth(1.0);
}

void DrawWire(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	axis();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(1);

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, -100, 0);
	glBegin(GL_QUADS);
	glVertex3f(200, 0, 200);
	glVertex3f(200, 0, -200);
	glVertex3f(-200, 0, -200);
	glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();
	glutSwapBuffers();
}

void DrawShade(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	axis();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);

	glColor3f(0.6, 0.6, 0.6);  // draw plane
	glPushMatrix();
	glTranslatef(0, -100, 0);
	glBegin(GL_QUADS);
	glVertex3f(200, 0, 200);
	glVertex3f(200, 0, -200);
	glVertex3f(-200, 0, -200);
	glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();
	glutSwapBuffers();
}

void MakeGL_Model(void) {
	int i;
	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1, 1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();

	glRotatef(angle, 0.0, 1.0, 0.0);
	glScalef(scalefactor, scalefactor, scalefactor);
	glColor3f(1, 0, 0);

	glColor3f(colorR, colorG, colorB);

	glEnable(GL_NORMALIZE);
	for (i = 0; i < fnum; i++) {
		Point norm = mpoint[mface[i].ip[0]];
		glBegin(GL_TRIANGLES);

		glNormal3f(norm.x, norm.y, norm.z);  // 면을 이루는 첫번째 vertex
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);

		norm = mpoint[mface[i].ip[1]];
		glNormal3f(norm.x, norm.y, norm.z);  // 면을 이루는 두번째 vertex
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);

		norm = mpoint[mface[i].ip[2]];
		glNormal3f(norm.x, norm.y, norm.z);  // 면을 이루는 세번째 vertex
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
		glEnd();
	}
	glPopMatrix();
	glEndList();
}

void perspmode() {  // perspective mode
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 1.0, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(600.0, 600.0, 600.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.);
}

void orthomode() {  // ortho mode
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0, 100.0, -100.0, 100.0, -800.0, 800.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(600.0, 600.0, 600.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.);
}

void GLSetupRC(void) {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 1.0, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(xmove, ymove, zmove, 0.0, 0.0, 0.0, 0.0, 1.0, down);
	MakeGL_Model();
	if (status == WIRE)
		DrawWire();
	else
		DrawShade();
}

void keyboard(unsigned char key, int x, int y) {
	printf("key %d\n", key);
	switch (key) {
	case'w':  // wire mode
		status = WIRE;
		glutPostRedisplay();
		break;
	case's':  // shade mode
		status = SHADE;
		glutPostRedisplay();
		break;

	case'e':  // hide back
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glutPostRedisplay();
		break;
	case'd':  // show back
		glDisable(GL_CULL_FACE);
		glutPostRedisplay();
		break;

	case'r':  // perspective mode
		display();
		perspmode();
		glutPostRedisplay();
		break;
	case'f':  // ortho mode
		display();
		orthomode();
		glutPostRedisplay();
		break;

	case'q':  // x viewport
		xmove = 600;
		ymove = 0;
		zmove = 0;
		down = 0;
		display();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		break;
	case'a':  // y viewports
		xmove = 0;
		ymove = 600;
		zmove = 0;
		down = -1;
		display();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		break;
	case'z':  // z viewport
		xmove = 0;
		ymove = 0;
		zmove = 600;
		down = 0;
		display();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		break;

	case'1':  // red color
		colorR = 0.8; colorG = 0.4; colorB = 0.4;
		display();
		glutPostRedisplay();
		break;
	case'2':  // green color
		colorR = 0.4; colorG = 0.8; colorB = 0.4;
		display();
		glutPostRedisplay();
		break;
	case'3':  // blue color
		colorR = 0.4; colorG = 0.4; colorB = 0.8;
		display();
		glutPostRedisplay();
		break;

	case 'b':  // xmoving
		xmove += 10;
		display();
		glutPostRedisplay();
		break;
	case 'n':  // ymoving
		ymove += 10;
		display();
		glutPostRedisplay();
		break;
	case 'm':  // zmoving
		zmove += 10;
		display();
		glutPostRedisplay();
		break;
	case 'h':  // xmoving
		xmove -= 10;
		display();
		glutPostRedisplay();
		break;
	case 'j':  // ymoving
		ymove -= 10;
		display();
		glutPostRedisplay();
		break;
	case 'k':  // zmoving
		zmove -= 10;
		display();
		glutPostRedisplay();
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		moving = 1;
		mousebegin = x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		moving = 0;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		scaling = 1;
		mousebegin = x;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		scaling = 0;
	}
}

void motion(int x, int y) {
	if (scaling) {
		scalefactor = scalefactor * (1.0 + (mousebegin - x) * 0.0001);
		glutPostRedisplay();
	}
	if (moving) {
		angle = angle + (x - mousebegin);
		mousebegin = x;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("YANGSOYEONG");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	InitLight();
	ReadModel();
	GLSetupRC();
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glutMainLoop();
	return 0;
}