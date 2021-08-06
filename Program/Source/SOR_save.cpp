#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
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

GLsizei winWidth = 1000, winHeight = 700;
GLboolean revolveFcn = false;  // �� ȸ��
GLboolean clean = false;  // ȭ�� �ʱ�ȭ
GLboolean draw = true;  // �� ���
GLboolean Wireframe = false;  // wireframe ���
GLfloat angle = 0;

int moving;
int mousebegin;
int light_moving;
float scalefactor = 1.0;
int scaling = 0;
int status = 0;

float fRotAngle;  // �Է¹��� ����
float radian;
int sweepResolutionMod = (360 / fRotAngle);  // ���� ȸ���ϴ� Ƚ��


class xPoint3D {
public:
	float x, y, z, w;
	xPoint3D() { x = y = z = 0; w = 1; };
};

xPoint3D pt;
vector<xPoint3D> arInputPoints;  // �ʱ⿡ ���� �� �ޱ�
vector<xPoint3D> arRotPoints;  // ȸ���� �� �ޱ�

int pnum;
int fnum;
Point* mpoint = NULL;
Face* mface = NULL;
int cnum = 0;  // count number

void line(int l) {
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);  // wireframe �� �׸���
	for (int i = 0; i < sweepResolutionMod; i++) {
		glVertex3f(arRotPoints[l + i].x, arRotPoints[l + i].y, arRotPoints[l + i].z);
	}
	glEnd();
	glFlush();
}

void cross(int c) {
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);  // wireframe �ﰢ�� �׸���
	for (int i = 0; i < (sweepResolutionMod - 1); i++) {
		glVertex3f(arRotPoints[c * sweepResolutionMod + i].x, arRotPoints[c * sweepResolutionMod + i].y, arRotPoints[c * sweepResolutionMod + i].z);
		glVertex3f(arRotPoints[c * sweepResolutionMod + (sweepResolutionMod + 1) + i].x, arRotPoints[c * sweepResolutionMod + (sweepResolutionMod + 1) + i].y,
			arRotPoints[c * sweepResolutionMod + (sweepResolutionMod + 1) + i].z);
	}
	glVertex3f(arRotPoints[c * sweepResolutionMod + (sweepResolutionMod - 1)].x, arRotPoints[c * sweepResolutionMod + (sweepResolutionMod - 1)].y,
		arRotPoints[c * sweepResolutionMod + (sweepResolutionMod - 1)].z);
	glVertex3f(arRotPoints[c * sweepResolutionMod + sweepResolutionMod].x, arRotPoints[c * sweepResolutionMod + sweepResolutionMod].y,
		arRotPoints[c * sweepResolutionMod + sweepResolutionMod].z);
	glVertex3f(arRotPoints[c * sweepResolutionMod].x, arRotPoints[c * sweepResolutionMod].y, arRotPoints[c * sweepResolutionMod].z);

	glEnd();
	glFlush();
}

void SaveModel() {

	FILE* fout;

	fout = fopen("c:\\data\\myModel.dat", "w");

	fprintf(fout, "VERTEX = %d\n", pnum);
	for (int i = 0; i < pnum; i++) {
		fprintf(fout, "%.1f %.1f %.1f\n", mpoint[i].x, mpoint[i].y, mpoint[i].z);
	}

	fprintf(fout, "FACE = %d\n", fnum);
	for (int i = 0; i < fnum; i++) {
		fprintf(fout, "%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(fout);
}

void MyDisplay() {
	glViewport(0, 0, 1000, 700);
	glColor3f(1.0, 0.0, 0.0);

	glColor3f(0.5, 0.5, 0.5);  // �� �׸���
	glBegin(GL_LINES);
	glVertex3f(500.0, 0.0, 0.0);  // x��
	glVertex3f(-500.0, 0.0, 0.0);
	glVertex3f(0.0, -500.0, 0.0);  // y��
	glVertex3f(0.0, 500.0, 0.0);
	glVertex3f(0.0, 0.0, -500.0);  // z��
	glVertex3f(0.0, 0.0, 500.0);
	glEnd();

	if (revolveFcn) {  // �� ȸ��

		float fNewAngle = 0;
		radian = fNewAngle * (M_PI / 180.0);

		for (int i = 0; i < arInputPoints.size(); i++) {
			for (int k = 0; k < sweepResolutionMod; k++) {
				xPoint3D newpt;
				newpt.x = arInputPoints[i].x * cos(radian) + arInputPoints[i].z * sin(radian);
				newpt.y = arInputPoints[i].y;
				newpt.z = -arInputPoints[i].x * sin(radian) + arInputPoints[i].z * cos(radian);

				arRotPoints.push_back(newpt);

				cout << "(" << newpt.x << ", " << newpt.y << ", " << newpt.z << ")\n";

				glColor3f(1.0, 1.0, 0.0);
				glPointSize(5.0);
				glBegin(GL_POINTS);
				glVertex3f(newpt.x, newpt.y, newpt.z);
				glEnd();
				glFlush();

				fNewAngle += fRotAngle;  // �� �ݺ����� ���ο� ���� ������Ʈ
				radian = fNewAngle * (M_PI / 180.);  // �� �ݺ����� ���� ������Ʈ
			}
		}

		pnum = arRotPoints.size();  // point ���� ����
		fnum = sweepResolutionMod * (arInputPoints.size() - 1) * 2;  // face ���� ����
		mpoint = new Point[pnum];
		mface = new Face[fnum];

		for (int i = 0; i < pnum; i++) {
			mpoint[i].x = arRotPoints[i].x * 0.7;
			mpoint[i].y = arRotPoints[i].y * 0.7;
			mpoint[i].z = arRotPoints[i].z * 0.7;
		}

		for (int k = 0; k < arInputPoints.size() - 1; k++) {
			for (int i = 0; i < sweepResolutionMod - 1; i++) {  // up-triangle
				mface[cnum].ip[1] = sweepResolutionMod * k + i + 1;
				mface[cnum].ip[2] = sweepResolutionMod * k + i;
				mface[cnum].ip[0] = sweepResolutionMod * (k + 1) + i;
				cnum += 1;
			}
			mface[cnum].ip[1] = sweepResolutionMod * k;
			mface[cnum].ip[2] = sweepResolutionMod * (k + 1) - 1;
			mface[cnum].ip[0] = sweepResolutionMod * (k + 2) - 1;
			cnum += 1;

			for (int i = 0; i < sweepResolutionMod - 1; i++) {  // down-triangle
				mface[cnum].ip[0] = sweepResolutionMod * k + i + 1;
				mface[cnum].ip[1] = sweepResolutionMod * (k + 1) + i;
				mface[cnum].ip[2] = sweepResolutionMod * (k + 1) + i + 1;
				cnum += 1;
			}
			mface[cnum].ip[0] = sweepResolutionMod * (k);
			mface[cnum].ip[1] = sweepResolutionMod * (k + 2) - 1;
			mface[cnum].ip[2] = sweepResolutionMod * (k + 1);
			cnum += 1;
		}
		SaveModel();
	}

	if (clean) {  // ȭ�� �ʱ�ȭ
		arInputPoints.clear();
		arRotPoints.clear();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.5, 0.5, 0.5);  // �� �׸���
		glBegin(GL_LINES);
		glVertex3f(500.0, 0.0, 0.0);  // x��
		glVertex3f(-500.0, 0.0, 0.0);
		glVertex3f(0.0, -500.0, 0.0);  // y��
		glVertex3f(0.0, 500.0, 0.0);
		glVertex3f(0.0, 0.0, -500.0);  // z��
		glVertex3f(0.0, 0.0, 500.0);
		glEnd();
		glFlush();
	}

	if (Wireframe) {  // wireframe ���
		for (int i = 0; i < arInputPoints.size(); i++) {  // arInputPoints ������ŭ �� �׸���
			line(i * sweepResolutionMod);
		}

		for (int i = 0; i < (arInputPoints.size() - 1); i++) {  // arInputPoints ���� - 1��ŭ �� �׸���
			cross(i);
		}
	}
}


void Drawing(GLint button, GLint action, GLint xMouse, GLint yMouse) {

	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && draw == true) {  // ���콺 ��ġ �Է� �� �� ���
		pt.x = xMouse - winWidth / 2;
		pt.y = -yMouse + winHeight / 2;
		pt.z = 0.0;
		arInputPoints.push_back(pt);
		cout << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")\n";

		glColor3f(1.0, 1.0, 0.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		glVertex3f(pt.x, pt.y, pt.z);
		glEnd();
		glFlush();
	}
	glutPostRedisplay();
	glutSwapBuffers();
}


void MyMainMenu(int entryID) {
	if (entryID == 1) {  // ��� �� �����
		revolveFcn = false;
		clean = true;
		Wireframe = false;
		MyDisplay();
		clean = false;
		draw = true;
	}

	else if (entryID == 2) {  // wireframe ���
		Wireframe = true;
		draw = false;
		revolveFcn = false;
	}
	glutPostRedisplay();
}


void MySubMenu(int entryID) {
	if (entryID == 1) {  // 30�� ȸ��
		cout << "(" << pt.x << ", " << pt.y << ", 0.0) by 30 radians: \n ";
		fRotAngle = 30;
		radian = fRotAngle * (M_PI / 180.);
		sweepResolutionMod = 360 / fRotAngle;
	}

	else if (entryID == 2) {  // 60�� ȸ��
		cout << "(" << pt.x << ", " << pt.y << ", 0.0) by 60 radians: \n ";
		fRotAngle = 60;
		radian = fRotAngle * (M_PI / 180.);
		sweepResolutionMod = 360 / fRotAngle;
	}

	else if (entryID == 3) {  // 90�� ȸ��
		cout << "(" << pt.x << ", " << pt.y << ", 0.0) by 90 radians: \n ";
		fRotAngle = 90;
		radian = fRotAngle * (M_PI / 180.);
		sweepResolutionMod = 360 / fRotAngle;
	}

	else if (entryID == 4) {  // 120�� ȸ��
		cout << "(" << pt.x << ", " << pt.y << ", 0.0) by 120 radians: \n ";
		fRotAngle = 120;
		radian = fRotAngle * (M_PI / 180.);
		sweepResolutionMod = 360 / fRotAngle;
	}

	else if (entryID == 5) {  // 180�� ȸ��
		cout << "(" << pt.x << ", " << pt.y << ", 0.0) by 180 radians: \n ";
		fRotAngle = 180;
		radian = fRotAngle * (M_PI / 180.);
		sweepResolutionMod = 360 / fRotAngle;
	}
	draw = false;
	revolveFcn = true;
	glutPostRedisplay();

}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("YANGSOYEONG");

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-500.0, 500.0, -350.0, 350.0, -500.0, 500.0);

	GLint MySubMenuID = glutCreateMenu(MySubMenu);
	glutAddMenuEntry("30", 1);
	glutAddMenuEntry("60", 2);
	glutAddMenuEntry("90", 3);
	glutAddMenuEntry("120", 4);
	glutAddMenuEntry("180", 5);

	GLint MyMainMenuID = glutCreateMenu(MyMainMenu);
	glutAddSubMenu("Revolve angle", MySubMenuID);
	glutAddMenuEntry("Remove All", 1);
	glutAddMenuEntry("Wireframe Mode", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutDisplayFunc(MyDisplay);
	glutMouseFunc(Drawing);
	glutMainLoop();
	return 0;
}