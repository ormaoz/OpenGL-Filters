#include <stdio.h>
#include <math.h>
#include "glut.h"

GLuint texture[4];
GLuint bitTex;
GLubyte *pic;
GLubyte *detailed;
GLubyte *blur5;
GLubyte *blur3;
GLubyte *halftone;
GLubyte *floydSteinberg;
GLint *floydTmp;
GLint width;
GLint height;
int picSize;
int rd;
GLubyte header[54];
GLubyte colorTable[1024];

void detaildPicture() {
	// blur picture 5x5 filter
	blur5 = new GLubyte[picSize];

	// detailed picture
	detailed = new GLubyte[picSize];

	// Creating the blured picture first: 5x5 filter: 
	for (int i = 0; i < height*width; i++) {
		int result = (pic[i]) + (pic[i - 1]) + (pic[i - 2]) + (pic[i + 1]) + (pic[i + 2]) +
			(pic[i + width]) + (pic[i - 1 + width]) + (pic[i - 2 + width]) + (pic[i + 1 + width]) + (pic[i + 2 + width]) +
			(pic[i - width]) + (pic[i - 1 - width]) + (pic[i - 2 - width]) + (pic[i + 1 - width]) + (pic[i + 2 - width]) +
			(pic[i + 2 * width]) + (pic[i - 1 + 2 * width]) + (pic[i - 2 + 2 * width]) + (pic[i + 1 + 2 * width]) + (pic[i + 2 + 2 * width]) +
			(pic[i - 2 * width]) + (pic[i - 1 - 2 * width]) + (pic[i - 2 - 2 * width]) + (pic[i + 1 - 2 * width]) + (pic[i + 2 - 2 * width]);
		blur5[i] = result / 25;
	}

	// After creating the blured 5x5, sub it from the original 8 bit picture and conduct treshhold 
	for (int i = 0; i < height*width; i++) {
		detailed[i] = pic[i] - blur5[i];
		if (detailed[i] >= 30) {
			detailed[i] = 255;
		}
		else {
			detailed[i] = 0;
		}
	}
}

void halftonePicture() {

	// Blured picture 3x3 filer (to be used for the halftone)
	blur3 = new GLubyte[picSize];

	// Halftone picture
	halftone = new GLubyte[picSize * 9];

	// First I'm creating a blured pictures with 3x3 filter
	int result2;
	for (int i = 0; i < height*width; i++) {
		result2 = (pic[i]) + (pic[i - 1]) + (pic[i + 1]) +
			(pic[i + width]) + (pic[i - 1 + width]) + (pic[i + 1 + width]) +
			(pic[i - width]) + (pic[i - 1 - width]) + (pic[i + 1 - width]);
		blur3[i] = result2 / 9;
	}
	// Next, for every third pixel in the blured picture (which contains the avarge of the original its
	// 9 neighbours in the original picture, I'll check it's value between 0 to 1 (devided to 1/10 groups)
	for (int i = 1; i < height; i += 3) {
		for (int j = 1; j < width; j += 3) {
			int k = i*width + j;

			// 0 to 0.1 case
			if (blur3[k] < 255 * 0 && blur3[k] < 255 * 0.1) {
				halftone[k] = 0; // middle
				halftone[k - 1] = 0; // left
				halftone[k + 1] = 0; // right
				halftone[k + width] = 0; // top
				halftone[k - 1 + width] = 0; // top left
				halftone[k + 1 + width] = 0; // top right
				halftone[k - width] = 0; // bottom
				halftone[k - 1 - width] = 0; // bottom left
				halftone[k + 1 - width] = 0; // bottom right

			}
			// 0.1 to 0.2 case
			else if (blur3[k] >= 255 * 0.1 && blur3[k] < 255 * 0.2) {
				halftone[k] = 255;
				halftone[k - 1] = 0;
				halftone[k + 1] = 0;
				halftone[k + width] = 0;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 0;
				halftone[k + 1 - width] = 0;
			}
			// 0.2 to 0.3 case
			else if (blur3[k] >= 255 * 0.2 && blur3[k] < 255 * 0.3) {
				halftone[k] = 255;
				halftone[k - 1] = 0;
				halftone[k + 1] = 255;
				halftone[k + width] = 0;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 0;
				halftone[k + 1 - width] = 0;
			}
			// 0.3 to 0.4 case
			else if (blur3[k] >= 255 * 0.3 && blur3[k] < 255 * 0.4) {
				halftone[k] = 255;
				halftone[k - 1] = 0;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 0;
				halftone[k + 1 - width] = 0;
			}
			// 0.4 to 0.5 case
			else if (blur3[k] >= 255 * 0.4 && blur3[k] < 255 * 0.5) {
				halftone[k] = 255;
				halftone[k - 1] = 0;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 0;
			}
			// 0.5 to 0.6 case
			else if (blur3[k] >= 255 * 0.5 && blur3[k] < 255 * 0.6) {
				halftone[k] = 255;
				halftone[k - 1] = 255;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 0;
			}
			// 0.6 to 0.7 case
			else if (blur3[k] >= 255 * 0.6 && blur3[k] < 255 * 0.7) {
				halftone[k] = 255;
				halftone[k - 1] = 255;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 0;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 255;
			}
			// 0.7 to 0.8 case
			else if (blur3[k] >= 255 * 0.7 && blur3[k] < 255 * 0.8) {
				halftone[k] = 255;
				halftone[k - 1] = 255;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 0;
				halftone[k + 1 + width] = 255;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 255;
			}
			// 0.8 to 0.9 case
			else if (blur3[k] >= 255 * 0.8 && blur3[k] < 255 * 0.9) {
				halftone[k] = 255;
				halftone[k - 1] = 255;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 255;
				halftone[k + 1 + width] = 255;
				halftone[k - width] = 0;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 255;
			}
			// 0.9 to 1 case
			else if (blur3[k] >= 255 * 0.9 && blur3[k] < 255) {
				halftone[k] = 255;
				halftone[k - 1] = 255;
				halftone[k + 1] = 255;
				halftone[k + width] = 255;
				halftone[k - 1 + width] = 255;
				halftone[k + 1 + width] = 255;
				halftone[k - width] = 255;
				halftone[k - 1 - width] = 255;
				halftone[k + 1 - width] = 255;
			}
		}
	}
}

void floydSteinbergPicture() {

	// Floyd-Steinberg picture
	floydSteinberg = new GLubyte[picSize];
	floydTmp = new GLint[picSize];





	/*
	for(int i = 0; i < height*width; i++) {
	for (int j = (i - 2); j <= (i + 2); j++) {
	for (int k = (height - 2); k <= (height + 2); k++) {
	detailed[i] += pic[j+k]/25;
	}
	}
	} */


	for (int i = height - 1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			int k = i*width + j;
			// Round the color to 16 grey scale
			int rounded = int(pic[k] / 16) * 16;

			//	printf("        %d ---> %d     ", pic[k], rounded);

			// Calculate error
			floydTmp[k] = rounded;
			int error = pic[k] - rounded;
			if (i == 0 && j != width - 1) {
				int tmp = int((7.0 / 16.0)*error);
				floydTmp[k + 1] += tmp;
			}
			else if (j == width - 1) {
				int tmp = int((3.0 / 16.0)*error);
				floydTmp[k - width - 1] += tmp;
				tmp = int((5.0 / 16.0)*error);
				floydTmp[k - width] += tmp;
			}
			else {
				int tmp = int((7.0 / 16.0)*error);
				floydTmp[k + 1] += tmp;
				tmp = int((3.0 / 16.0)*error);
				floydTmp[k - width - 1] += tmp;
				tmp = int((5.0 / 16.0)*error);
				floydTmp[k - width] += tmp;
				tmp = int((1.0 / 16.0)*error);
				floydTmp[k - width + 1] += tmp;
			}
		}
	}
	for (int i = 0; i < picSize; i++) {
		floydSteinberg[i] = floydTmp[i];
	}
}

void textures() {


	//************ First image - top left **************************
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texture[2]);  //generate place for new texture
	glBindTexture(GL_TEXTURE_2D, texture[2]); // initialize first texure 

	//texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//build texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pic);


	//************ second image - top right **************************
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texture[1]);  //generate place for new texture
	glBindTexture(GL_TEXTURE_2D, texture[1]); // initialize first texure 

	//texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//build texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, detailed);



	//************ Third image - bottom left **************************
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texture[0]);  //generate place for new texture
	glBindTexture(GL_TEXTURE_2D, texture[0]); // initialize first texure 

	//texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, halftone);

	//************ Fourth image - bottom right **************************
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texture[3]);  //generate place for new texture
	glBindTexture(GL_TEXTURE_2D, texture[3]); // initialize first texure 

	//texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//build texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, floydSteinberg);
}
void init()
{
	FILE *f;
	// Basic settings for the window
	glEnable(GL_TEXTURE_2D);
	glOrtho(-1.0, 1.0, -1.0, 1.0, 2.0, -2.0);
	glClearColor(0, 0, 0, 0);

	// Open the file
	f = fopen("lena.bmp", "rb");

	//original image header reading
	fread(header, 54, 1, f);
	if (header[0] != 'B' || header[1] != 'M')
		exit(1);  //not a BMP file
	//	for (int i = 0; i<54; i++)
	//		printf("%x  ", header[i]);

	picSize = (*(int*)(header + 2) - 54);
	width = *(int*)(header + 18);
	height = *(int*)(header + 22);
	//	printf("\n%d %d %d %d \n", picSize, width, height, width*height * 3);

	// Initilize the original picture
	pic = new GLubyte[picSize];
	//read image
	rd = fread(pic, 1, picSize, f);

	detaildPicture();
	halftonePicture();
	floydSteinbergPicture();
	textures();

	//	printf("texture\n");
	fclose(f);
}



void mydisplay(void){

	glClear(GL_COLOR_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[0]); //using first texture
	glViewport(0, 0, 256, 256); // bottom left


	glColor3f(1.0f, 1.0, 1.0f);

	//glShadeModel(GL_FLAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); //adapt texture to shape
	glVertex3f(-1.0, -1.0f, 1.0);

	glTexCoord2f(1, 0);  //adapt texture to shape
	glVertex3f(1.0, -1.0f, 1.0);

	glTexCoord2f(1, 1);//adapt texture to shape
	glVertex3f(1.0, 1.0f, 1.0);

	glTexCoord2f(0, 1);//adapt texture to shape
	glVertex3f(-1.0, 1.0f, 1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(256, 256, 256, 256); // top right
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[1]); //using second texture
	//glScalef(0.5,0.5,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1.0, -1.0f, -1.0);

	glTexCoord2f(1, 0);
	glVertex3f(1.0, -1.0f, -1.0);

	glTexCoord2f(1, 1);
	glVertex3f(1.0, 1.0f, -1.0);

	glTexCoord2f(0, 1);
	glVertex3f(-1.0, 1.0f, -1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glViewport(0, 256, 256, 256); // top left
	glBindTexture(GL_TEXTURE_2D, texture[2]); //using third texture
	//glScalef(0.5,0.5,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1.0, -1.0f, -1.0);

	glTexCoord2f(1, 0);
	glVertex3f(1.0, -1.0f, -1.0);

	glTexCoord2f(1, 1);
	glVertex3f(1.0, 1.0f, -1.0);

	glTexCoord2f(0, 1);
	glVertex3f(-1.0, 1.0f, -1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glViewport(256, 0, 256, 256); // bottom right
	glBindTexture(GL_TEXTURE_2D, texture[3]); //using fourth texture
	//glScalef(0.5,0.5,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1.0, -1.0f, -1.0);

	glTexCoord2f(1, 0);
	glVertex3f(1.0, -1.0f, -1.0);

	glTexCoord2f(1, 1);
	glVertex3f(1.0, 1.0f, -1.0);

	glTexCoord2f(0, 1);
	glVertex3f(-1.0, 1.0f, -1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glFlush();
}

int main(int  argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Simple");
	init();
	

	// glutReshapeFunc(myReshape);
	glutDisplayFunc(mydisplay);
	//glutIdleFunc(mydisplay);
	// printf("pi = %f",3.14);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glutMainLoop();

	delete(pic);
}