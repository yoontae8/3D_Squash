#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <time.h>
/* Library for sound effect*/
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")


int cond1[9] = { 728, 926, 1567, 1284, 742, 1347, 894, 1193, 1149 };
int cond2[9] = { 910, 1052, 1393, 7342, 1273, 1148, 1228, 1039, 994 };
int cond3[9] = { 576, 1039, 1094, 733, 839, 984, 653, 628, 723 };
int cond4[9] = { 824, 824, 8323, 554, 593, 773, 483, 374, 593 };
int cond5[9] = { 627, 854, 623, 754, 493, 573, 383, 374, 393 };
int *spSetting = cond5;
static int _windowNumber;
/* speed control*/
int xMax = spSetting[0];
int yMax = spSetting[1];
int zMax = spSetting[2];
/* coordinate for the ball*/
int _xSpeed = xMax / 4;
int _ySpeed = yMax / 4;
int _zSpeed = 0;
double x, y, z;
GLboolean _start = false;
int _xCursor = 0;
int _yCursor = 0;
float xPoint = 0;
float yPoint = 0;
float zFar = 8.0f;
int stage = 1;
/* controls the fog efect(zFar)*/
int _farSpeed = 0;
int _farMax = 1500;
int startTime, curTime, record, initTime, curRecord;
GLboolean save = false;
GLboolean curOn = false;
GLboolean cheat = false;
float _width = 800;
float _height = 600;


float plasterAmbient[3] = { 0.2f, 0.f, 0.f };
float plasterDiffuse[3] = { 1.f, 0.f, 0.f };
float plasterSpecular[3] = { 0.f, 0.f, 0.f };
float plasterShiness = 0.f;

float mirrorAmbient[3] = { 0.2f, 0.f, 0.f };
float mirrorDiffuse[3] = { 0.5f, 0.f, 0.f };
float mirrorSpecular[3] = { 1.f, 0.f, 0.2f };
float mirrorShiness = 40.f;

float pvcAmbient[3] = { 0.2f, 0.f, 0.f };
float pvcDiffuse[3] = { 0.6f, 0.7f, 0.5f };
float pvcSpecular[3] = { 1.f, 0.3f, 1.f };
float pvcShiness = 70.f;

float metalAmbient[3] = { 0.2f, 0.8f, 0.f };
float metalDiffuse[3] = { 0.4f, 0.f, 0.f };
float metalSpecular[3] = { 1.0f, 0.f, 0.f };
float metalShiness = 25.f;

float metal2Ambient[3] = { 0.2f, 0.01f, 0.05f };
float metal2Diffuse[3] = { 0.4f, 0.7f, 0.7f };
float metal2Specular[3] = { 1.0f, 0.f, 1.0f };
float metal2Shiness = 70.f;

void display(void);
void key(unsigned char c, int mx, int my);
void no_events();
void reshape(int x, int y);
void Menu(int value);
void mouse(int button, int state, int x, int y);
void init_light();
void set_torch();
void set_light_positions();
void setObjectLight(int x);
void passive(int x1, int y1);
GLubyte *LoadBmp(const char *Path, int *Width, int *Height);


int main(int argc, char** argv)
{
	/* GLUT environment initialisation */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/* Window creation */
	glutInitWindowSize(_width, _height);
	glutInitWindowPosition(240, 100);
	_windowNumber = glutCreateWindow(argv[0]);

	/* Keyboard interuptions */
	glutKeyboardFunc(key);

	/* Callback function assignment */
	glutDisplayFunc(display);
	glutIdleFunc(no_events);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passive);

	glutCreateMenu(Menu);
	glutAddMenuEntry("Stage 1", 1);
	glutAddMenuEntry("Stage 2", 2);
	glutAddMenuEntry("Stage 3", 3);
	glutAddMenuEntry("Stage 4", 4);
	glutAddMenuEntry("Final Stage", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	/* Initialize Opengl states */
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	init_light();

	initTime = GetCurrentTime();

	sndPlaySoundA("gong.wav", SND_ASYNC | SND_NODEFAULT);
	/* Main event loop */
	glutMainLoop();

	return 0;
}


/* Rendering function */
void display(void)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, _height / 4, _width, _height * 3 / 4);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch (stage) {
	case 1:
		zFar = 9.f;
		break;
	case 2:
		zFar = 9.f;
		break;
	case 3:
		zFar = 9.f;
		break;
	case 4:
		zFar = 5.f;
		break;
	case 5:
		if (_farSpeed < _farMax / 2) {
			zFar = (float)_farSpeed / (_farMax / 10) + 3;
		}
		else{
			zFar = (float)(_farMax - _farSpeed) / (_farMax / 10) + 3;
		}
		break;
	}
	gluPerspective(60.f, _width / _height, 0.1f, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	set_torch();

	/* Camera Position*/
	gluLookAt(0.f, 0.f, 1.1, 0.f, 0.f, -4.f, 0.f, 1.f, 0.f);

	set_light_positions();

	setObjectLight(3);

	xPoint = ((float)_xCursor - 400.f) / 470.f;
	yPoint = -((float)_yCursor - 223.f) / 360.f;


	GLubyte *data;
	int imgWidth, imgHeight;

	/* stage clear after 30 sec*/
	if (_start && GetCurrentTime() - startTime > 30000) {
		if (stage != 5) {
			stage += 1;
			_start = !_start;
			sndPlaySoundA("bonus.wav", SND_ASYNC | SND_NODEFAULT);
			if (save == false) {
				record = GetCurrentTime() - startTime;
				save = true;
			}
			curRecord = GetCurrentTime() - initTime;
			curOn = true;
		}

	}

	glPushMatrix();

	/*when ready, set the coordinate of the ball at the cursor*/
	if (!_start)
	{

		_xSpeed = (float)_xCursor * xMax / 2 / _width;
		_ySpeed = (float)((float)_yCursor * 1.3 - 600) * -1 * yMax / 2 / _height;
		_zSpeed = 0.0f;
		glTranslatef(xPoint, yPoint, 0.0f);

	}
	else {
		/*when racket hits the ball*/
		if (_zSpeed == 0)
			sndPlaySoundA("handbell.wav", SND_ASYNC | SND_NODEFAULT);
		/*when ball hits the wall*/
		if (_zSpeed == zMax / 2||_ySpeed == 0 || _ySpeed == yMax / 2 || _xSpeed == 0 || _xSpeed == xMax / 2) {
			sndPlaySoundA("UTex default sound.wav", SND_ASYNC | SND_NODEFAULT);
			/*in sage three, irregular bounce*/
			if (stage == 3) {
				xMax = (int)rand() % ((spSetting[6] / 20) * 2) + spSetting[6] - (spSetting[6] / 20);
				yMax = (int)rand() % ((spSetting[7] / 20) * 2) + spSetting[7] - (spSetting[7] / 20);
			}
		}

		/* coordinate setting*/
		if (_zSpeed < zMax / 2)
		{
			z = (double)_zSpeed / ((double)zMax / 2 / 7) * -1;
			if (_ySpeed < yMax / 2)
			{
				y = ((double)_ySpeed - (double)yMax / 4) / ((double)yMax / 4 / 0.52);
				if (_xSpeed < xMax / 2){
					x = ((double)_xSpeed - (double)xMax / 4) / ((double)xMax / 4 / 0.74);
					glTranslatef(x, y, z);
				}
				else {
					x = ((double)_xSpeed - (double)xMax * 3 / 4) / ((double)xMax / 4 / 0.74) * -1;
					glTranslatef(x, y, z);
				}
			}
			else
			{
				y = ((double)_ySpeed - (double)yMax * 3 / 4) / ((double)yMax / 4 / 0.52) * -1;
				if (_xSpeed < xMax / 2) {
					x = ((double)_xSpeed - (double)xMax / 4) / ((double)xMax / 4 / 0.74);
					glTranslatef(x, y, z);
				}
				else {
					x = ((double)_xSpeed - (double)xMax * 3 / 4) / ((double)xMax / 4 / 0.74) * -1;
					glTranslatef(x, y, z);

				}
			}
		}
		else
		{
			z = (double)(zMax - _zSpeed) / ((double)zMax / 2 / 7) * -1;
			if (_ySpeed < yMax / 2)
			{
				y = ((double)_ySpeed - (double)yMax / 4) / ((double)yMax / 4 / 0.52);
				if (_xSpeed < xMax / 2) {
					x = ((double)_xSpeed - (double)xMax / 4) / ((double)xMax / 4 / 0.74);
					glTranslatef(x, y, z);
				}
				else{
					x = ((double)_xSpeed - (double)xMax * 3 / 4) / ((double)xMax / 4 / 0.74) * -1;
					glTranslatef(x, y, z);
				}
			}
			else
			{
				y = ((double)_ySpeed - (double)yMax * 3 / 4) / ((double)yMax / 4 / 0.52) * -1;
				if (_xSpeed < xMax / 2) {
					x = ((double)_xSpeed - (double)xMax / 4) / ((double)xMax / 4 / 0.74);
					glTranslatef(x, y, z);
				}
				else {
					x = ((double)_xSpeed - (double)xMax * 3 / 4) / ((double)xMax / 4 / 0.74) * -1;
					glTranslatef(x, y, z);
				}
			}
		}
		if ((float)_zSpeed >= zMax - 1 && (x <= xPoint - 0.12 || x > xPoint + 0.12 || y <= yPoint - 0.12 || y > yPoint + 0.12)) {
			_start = !_start;
			sndPlaySoundA("QUEST.wav", SND_ASYNC | SND_NODEFAULT);
			if (save == false) {
				record = GetCurrentTime() - startTime;
				save = true;
			}
		}
	}

	/*draw racket*/
	glutSolidSphere(0.08, 30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(xPoint, yPoint, 0.0f);

	setObjectLight(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.12, -0.12);
	glVertex2f(0.12, -0.12);
	glVertex2f(0.12, 0.12);
	glVertex2f(-0.12, 0.12);
	glVertex2f(-0.12, -0.12);
	glEnd();

	glPopMatrix();

	glPushMatrix();

	setObjectLight(0);


	/*draw stadium*/
	glBegin(GL_POLYGON);
	glVertex3f(-.8, .6, 0);
	glVertex3f(.8, .6, 0);
	glVertex3f(.8, .6, -7);
	glVertex3f(-.8, .6, -7);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(-.8, -.6, 0);
	glVertex3f(.8, -.6, 0);
	glVertex3f(.8, -.6, -7);
	glVertex3f(-.8, -.6, -7);
	glEnd();

	setObjectLight(1);
	glBegin(GL_POLYGON);
	glVertex3f(-.8, -.6, 0);
	glVertex3f(-.8, -.6, -7);
	glVertex3f(-.8, .6, -7);
	glVertex3f(-.8, .6, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(0.8, -.6, 0);
	glVertex3f(0.8, -.6, -7);
	glVertex3f(0.8, .6, -7);
	glVertex3f(0.8, .6, 0);
	glEnd();
	setObjectLight(4);
	glBegin(GL_POLYGON);
	glVertex3f(-.8, -.8, -7);
	glVertex3f(.8, -.8, -7);
	glVertex3f(.8, .8, -7);
	glVertex3f(-.8, .8, -7);
	glEnd();
	glPopMatrix();

	glViewport(0, 0, _width, _height / 4);

	char pTime[128], rTime[128], sTime[128], stageStr[128];
	char *ach;

	curTime = GetCurrentTime() - startTime;
	ach = "Achievement";

	if (_start)
		sprintf_s(pTime, "elapsed time  %02d : %02d", curTime / 1000 % 100, curTime / 10 % 100);
	else
		sprintf_s(pTime, "elapsed time  00 : 00");

	if (save)
		sprintf_s(rTime, "record time  %02d : %02d", record / 1000 % 100, record / 10 % 100);
	else
		sprintf_s(rTime, "record time...", record / 1000 % 100, record / 10 % 100);
	
	if (cheat) {
		sprintf_s(sTime, "");
		sprintf_s(stageStr, "Stage Jump");
	}
	else {
		if (curOn) {
			sprintf_s(sTime, "time record  %d : %02d : %02d", curRecord / 60000 % 60, curRecord / 1000 % 60, curRecord / 10 % 100);
			sprintf_s(stageStr, "stage %d", stage - 1);
		}
		else {
			sprintf_s(sTime, "time record 0 : 00 : 00");
			sprintf_s(stageStr, "stage");
		}
	}

	


	switch (stage){
	case 1:
		data = LoadBmp("stage1.bmp", &imgWidth, &imgHeight);
		break;
	case 2:
		data = LoadBmp("stage2.bmp", &imgWidth, &imgHeight);
		break;
	case 3:
		data = LoadBmp("stage3.bmp", &imgWidth, &imgHeight);
		break;
	case 4:
		data = LoadBmp("stage4.bmp", &imgWidth, &imgHeight);
		break;
	case 5:
		data = LoadBmp("stage5.bmp", &imgWidth, &imgHeight);
		break;

	}

	if (data != NULL) {
		glRasterPos2f(-0.4, 0.2);
		glDrawPixels(imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
		free(data);
	}

	glRasterPos2f(0.3, 0.0);
	for (int i = 0; i < 30; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, pTime[i]);

	glRasterPos2f(0.3, -0.2);
	for (int i = 0; i < 30; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, rTime[i]);

	glRasterPos2f(-0.7, 0.2);
	for (int i = 0; i < 12; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ach[i]);

	glRasterPos2f(-0.64, 0.0);
	for (int i = 0; i < 30; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, stageStr[i]);

	glRasterPos2f(-0.64, -0.2);
	for (int i = 0; i < 30; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, sTime[i]);

	


	/* Window refresh */
	glutSwapBuffers();
}




/* Key events */

void key(unsigned char c, int mx, int my)
{
	// Avoid annoying warnings

	switch (c)
	{
	case 27:
		glFinish();
		glutDestroyWindow(_windowNumber);
		exit(0);
		break;
	}
	glutPostRedisplay();
}
/*move the coordinate*/
void no_events()
{
	if (_start) {
		_xSpeed = (_xSpeed + 1) % xMax;
		_ySpeed = (_ySpeed + 1) % yMax;
		_zSpeed = (_zSpeed + 1) % zMax;
		_farSpeed = (_farSpeed + 1) % _farMax;
	}


	glutPostRedisplay();
}

void reshape(int x, int y)
{
	glViewport(0, 0, x, y);
	_width = x;
	_height = y;
}

void Menu(int value)
{
	//stage setting
	switch (value){

	case 1:
		xMax = spSetting[0], yMax = spSetting[1], zMax = spSetting[2];
		stage = 1;
		break;
	case 2:
		xMax = spSetting[3], yMax = spSetting[4], zMax = spSetting[5];
		stage = 2;
		break;
	case 3:
		xMax = spSetting[6], yMax = spSetting[7], zMax = spSetting[8];
		stage = 3;
		break;
	case 4:
		xMax = spSetting[6], yMax = spSetting[7], zMax = spSetting[8];
		stage = 4;
		break;
	case 5:
		xMax = spSetting[6], yMax = spSetting[7], zMax = spSetting[8];
		stage = 5;
		break;
	}
	cheat = true;
	_start = false;
	_xSpeed = (float)_xCursor * xMax / 2 / _width;
	_ySpeed = (float)(_yCursor - _height) * -1 * yMax / 2 / _height;
	_zSpeed = 0.0;
	glutPostRedisplay();

}

void mouse(int button, int state, int x, int y) 
{
	//when mouse is clicked, start moving the ball
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && _start == false) {
		sndPlaySoundA("handbell.wav", SND_ASYNC | SND_NODEFAULT);
		_start = !_start;
		save = false;
		startTime = GetCurrentTime();
		glutPostRedisplay();
	}
}

void init_light()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	float light0Ambient[4] = { 0.6f, 0.6f, 0.6f, 0.f };
	float light0Diffuse[4] = { 1.0f, 1.0f, 1.0f, 0.f };
	float light0Specular[4] = { 1.0f, 1.0f, 1.0f, 0.f };

	int i = 0;
	for (i = 0; i < 3; i++)
	{
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light0Ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light0Diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light0Specular);
	}

	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 15);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 100);
}

void set_torch()
{
	float light2_position1[4] = { 0.f, 0.f, -.3f, 1.f };
	float light2_position2[4] = { 0.f, 0.f, -.4f, 1.f };
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position1);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_position2);
}

void set_light_positions()
{
	float light0_position1[4] = { 0.f, 0.f, -.4f, 1.f };
	float light0_position2[4] = { 0.f, 2.f, -.0f, 1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position1);
	glLightfv(GL_LIGHT1, GL_POSITION, light0_position2);
}

void setObjectLight(int x) 
{
	switch (x) {
	case 0:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, plasterAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, plasterDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, plasterSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, plasterShiness);
		break;
	case 1:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mirrorAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mirrorDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mirrorSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mirrorShiness);
		break;
	case 2:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pvcAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pvcDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pvcSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pvcShiness);
		break;
	case 3:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, metalAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, metalDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, metalSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, metalShiness);
		break;
	case 4:
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, metal2Ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, metal2Diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, metal2Specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, metal2Shiness);
		break;
	}
}

//receive position of the 
void passive(int x1, int y1)
{
	_xCursor = x1; _yCursor = y1;
}

GLubyte *LoadBmp(const char *Path, int *Width, int *Height)
{
	HANDLE hFile;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER *fh = NULL;
	BITMAPINFOHEADER *ih;
	BYTE *pRaster;

	hFile = CreateFileA(Path, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	FileSize = GetFileSize(hFile, NULL);
	fh = (BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile, fh, FileSize, &dwRead, NULL);
	CloseHandle(hFile);

	int len = FileSize - fh->bfOffBits;
	pRaster = (GLubyte *)malloc(len);
	memcpy(pRaster, (BYTE *)fh + fh->bfOffBits, len);

	// RGB로 순서를 바꾼다.
	for (BYTE *p = pRaster; p < pRaster + len - 3; p += 3) {
		BYTE b = *p;
		*p = *(p + 2);
		*(p + 2) = b;
	}

	ih = (BITMAPINFOHEADER *)((PBYTE)fh + sizeof(BITMAPFILEHEADER));
	*Width = ih->biWidth;
	*Height = ih->biHeight;

	free(fh);
	return pRaster;
}