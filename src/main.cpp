// 区间扫描线zbuffer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<GL\freeglut.h>
#include<string>
#include"model.h"
using namespace std;

//const int winx=1080;
//const int winy=720;

static int winx = 1080;
static int winy = 720;

OBJ tea;
zbuffer prj;
int step = 1;

void display() {

	glClear(GL_COLOR_BUFFER_BIT);
	//for (;r<=360; r++) {
		//prj.creatDS(tea.model, 'y', r++);
		for (int scan_line = 711; scan_line >= 0; scan_line--) {
			prj.exc_zbuffer(scan_line);
			glRasterPos2i(0, scan_line);
			glDrawPixels(1080, 1, GL_RGB, GL_UNSIGNED_BYTE, &prj.x_resolution[0][0]);
		}
		glFlush();
		glutSwapBuffers();
	//}
	
}

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)winx, 0.0, (GLdouble)winy);
	glMatrixMode(GL_MODELVIEW);
	prj.creatDS(tea.model);
}

void keyboard(unsigned char key,int x,int y) {
	switch (key) {
	case'\033':
	{
		exit(0);
	}
	default: {
		tea.trans(key, step);
		prj.creatDS(tea.model);
		glutPostRedisplay();
		break;
	}
	}
}

void specialKeyboard(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP: {
		unsigned char a = '8';
		tea.trans(a, step);
		prj.creatDS(tea.model);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_DOWN: {
		unsigned char a = '2';
		tea.trans(a, step);
		prj.creatDS(tea.model);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_LEFT: {
		unsigned char a = '4';
		tea.trans(a, step);
		prj.creatDS(tea.model);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_RIGHT: {
		unsigned char a = '6';
		tea.trans(a, step);
		prj.creatDS(tea.model);
		glutPostRedisplay();
		break;
	}
	}
}

void mouseActiveMotion(int x, int y) {

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winx,winy);

	glutCreateWindow("Model");

	//char* name = "teapot.obj";

	tea.Load_OBJ("teapot.obj");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	//glutMouseFunc(mouseActiveMotion);

	glutMainLoop();
    return 0;
}

