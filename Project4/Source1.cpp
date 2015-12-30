#include <stdio.h>
#include <math.h>
#include "glut.h"


GLubyte A[14] = {
	0xc0, 0x00,
	0x60, 0xc0,
	0x3f, 0x80,
	0x11, 0x00,
	0x0a, 0x00,
	0x0a, 0x00,
	0x04, 0x00,
};

void init(void)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClearColor(0, 0, 0, 0);
}

void display(void)
{
	float c[4];

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1, 1, 1);

	glGetFloatv(GL_CURRENT_RASTER_POSITION, c);
	printf("%f %f %f %f\n", c[0], c[1], c[2], c[3]);
	printf("%x\n", A[0]);

	glRasterPos2f(20, 20);
	glGetFloatv(GL_CURRENT_RASTER_POSITION_VALID, c);
	printf("%f\n", c[0]);

	glBitmap(12, 7, 0, 0, 11, 0, A);
	//glBitmap(12,7,0,0,11,0,A);
	//glBitmap(12,7,0,0,11,0,A);

	glGetFloatv(GL_CURRENT_RASTER_POSITION, c);
	printf("%f %f %f %f\n", c[0], c[1], c[2], c[3]);
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	//gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("FOnt");
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}