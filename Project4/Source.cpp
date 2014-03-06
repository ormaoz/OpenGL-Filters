#include <stdio.h>
#include <math.h>
#include "glut.h"



GLuint texture[4];
GLuint bitTex;
//first image
GLubyte *pic;
GLubyte *sharp;
GLubyte *blur5;
GLubyte *blur3;
GLubyte *halftone;
GLubyte *halftoneSmall;
GLubyte *floydSteinberg;
GLint *floydTmp;
GLint width;
GLint height;


void init()
{
	FILE *f;
	int picSize;
	int rd;
	GLubyte header[54];
	GLubyte colorTable[1024];

	glEnable(GL_TEXTURE_2D);

	glOrtho(-1.0, 1.0, -1.0, 1.0, 2.0, -2.0);
	//gluPerspective(20,1,0.5,8);
	//glTranslatef(0,0,-4);

	glClearColor(0, 0, 0, 0);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//	glDisable(GL_BLEND);
	f = fopen("lena.bmp", "rb");

	/*************************/
	//first image header reading
	fread(header, 54, 1, f);
	if (header[0] != 'B' || header[1] != 'M')
		exit(1);  //not a BMP file
	for (int i = 0; i<54; i++)
		printf("%x  ", header[i]);


	picSize = (*(int*)(header + 2) - 54);
	width = *(int*)(header + 18);
	height = *(int*)(header + 22);
	printf("\n%d %d %d %d \n", picSize, width, height, width*height * 3);

	/**********************************/

	pic = new GLubyte[picSize];
	rd = fread(pic, 1, picSize, f); //read image

	// blur picture 5x5 filter
	blur5 = new GLubyte[picSize];

	// sharpen picture
	sharp = new GLubyte[picSize];

	// blur picture 3x3 filer (to be used for the halftone)
	blur3 = new GLubyte[picSize];

	// halftone picture
	halftone = new GLubyte[picSize*9];
	halftoneSmall = new GLubyte[picSize];

	// Floyd-Steinberg picture
	floydSteinberg = new GLubyte[picSize];
	floydTmp = new GLint[picSize];
	fclose(f);
	
	int result;
	for (int i = 0; i < height*width; i++) {
		result =	(pic[i]) + (pic[i - 1]) + (pic[i - 2]) + (pic[i + 1]) + (pic[i + 2]) +
					(pic[i + width]) + (pic[i - 1 + width]) + (pic[i - 2 + width]) + (pic[i + 1 + width]) + (pic[i + 2 + width]) +
					(pic[i - width]) + (pic[i - 1 - width]) + (pic[i - 2 - width]) + (pic[i + 1 - width]) + (pic[i + 2 - width]) +
					(pic[i + 2 * width]) + (pic[i - 1 + 2 * width]) + (pic[i - 2 + 2 * width]) + (pic[i + 1 + 2 * width]) + (pic[i + 2 + 2 * width]) +
					(pic[i - 2 * width]) + (pic[i - 1 - 2 * width]) + (pic[i - 2 - 2 * width]) + (pic[i + 1 - 2 * width]) + (pic[i + 2 - 2 * width]);
		blur5[i] = result / 25;
	}

	for (int i = 0; i < height*width; i++) {
		sharp[i] = pic[i] - blur5[i];
		if (sharp[i] >= 30) {
			sharp[i] = 255;
		} else {
			sharp[i] = 0;
		}
	}
	
	
	//First I'm creating a blured pictures with 3x3 filter
	int result2;
	for (int i = 0; i < height*width; i++) {
		result2 = (pic[i]) + (pic[i - 1]) + (pic[i + 1]) +
			(pic[i + width]) + (pic[i - 1 + width]) + (pic[i + 1 + width]) +
			(pic[i - width]) + (pic[i - 1 - width]) + (pic[i + 1 - width]);
		blur3[i] = result2 / 9;
	}
	// Next, for every third pixel in the blured picture (which contains the avarge of the original its
	// 9 neighbours in the original picture, I'll check it's value between 0 to 1 (devided to 1/10 groups)
	for (int i = 1; i < height; i+=3) {
		for (int j = 1; j < width; j+=3) {
		//	int t=i*width + j;
			
			// indexes for the bigger picture
		//	int newI = i * 3 + 1;
		//	int newJ = j * 3 + 1;
			int k = i*width + j;

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
	/*
	for(int i = 0; i < height*width; i++) {
		for (int j = (i - 2); j <= (i + 2); j++) {
			for (int k = (height - 2); k <= (height + 2); k++) {
				sharp[i] += pic[j+k]/25;
			}
		}
	} */


	for (int i = height-1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			int k = i*width + j;
			// Round the color to 16 grey scale
			int rounded= int(pic[k]/16) * 16;
			
		//	printf("        %d ---> %d     ", pic[k], rounded);

			// Calculate error
			floydTmp[k] = rounded;
			int error = pic[k] - rounded;
			if (i == 0 && j != width-1) {
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
			

			



	/*		void buildFloidSteinberg(GLubyte *picture) {

				GLint *floid = new GLint[picSize];

				float error;
				int rounded;
				float alpha = 7.0 / 16.0, beta = 3.0 / 16.0, gamma = 5.0 / 16.0, delta = 1.0 / 16.0;

				for (int i = 0; i < picSize; i++) {
					floid[i] = picture[i];
				}

				for (int i = height - 1; i >= 0; i--) {
					for (int j = 0; j < width - 1; j++) {

						rounded = ((floid[(i * width) + j] + 8) / 16) * 16;
						error = floid[(i * width) + j] - rounded;
						floid[(i * width) + j] = rounded;

						if (i == 0) {
							floid[(i * width) + j + 1] += int(alpha * error + 0.5);
						}
						else if (j == width - 1) {
							floid[((i - 1) * width) + j - 1] += int(beta * error + 0.5);
							floid[((i - 1) * width) + j] += int(gamma * error + 0.5);
						}
						else {
							floid[(i * width) + j + 1] += int(alpha  * error + 0.5);
							floid[((i - 1) * width) + j - 1] += int(beta * error + 0.5);
							floid[((i - 1) * width) + j] += int(gamma * error + 0.5);
							floid[((i - 1) * width) + j + 1] += int(delta * error + 0.5);
						}
					}
				}

				GLubyte *res = new GLubyte[picSize];
				for (int i = 0; i < picSize; i++) {
					res[i] = floid[i];
				}

				buildTexture(res, FLOYD_STEINBERG);
			}

			*/


		//	printf("error is: %d     tmp is: %d      rounded is: %d\n", error, tmp, rounded);

		}
	}
	for (int i = 0; i < picSize; i++) {
		floydSteinberg[i] = floydTmp[i];
	}
	
	printf("*****  %d *******\n", rd);

	/**********************************/


	printf("texture\n");

	//************ first image - bottom left **************************
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texture[0]);  //generate place for new texture
	glBindTexture(GL_TEXTURE_2D, texture[0]); // initialize first texure 

	//texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//build texture
	/*gluScaleImage(GL_LUMINANCE, width * 3, height * 3, GL_UNSIGNED_BYTE, halftone, width, height, GL_UNSIGNED_BYTE, halftoneSmall);
	for (int i = 0; i<width; i++) {
		for (int j = 0; j<height; j++) {
			if (halftoneSmall[i + j*width]>48)
				halftoneSmall[i + j*width] = 255;
			else  {
				halftoneSmall[i + j*width] = 0;
			}
		}
	}*/

	//gluScaleImage(GL_LUMINANCE, 768, 768, GL_UNSIGNED_BYTE, halftone, 256, 256, GL_UNSIGNED_BYTE, halftoneSmall);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, halftone);
	


	
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, sharp);

	//************ third image - top left **************************
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

	//************ fourth image - bottom right **************************
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

int main(int  argc, char** argv)
{

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
	
	FILE *txt;
	txt = fopen("C:\1.txt", "w+");
	(txt, "blabla");
	fclose(txt);


}