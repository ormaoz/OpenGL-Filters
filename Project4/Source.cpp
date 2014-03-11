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
GLubyte *bitHalftone;
GLubyte *bitDetailed;

void detaildPicture() {
	// blur picture 5x5 filter
	blur5 = new GLubyte[picSize];

	// detailed picture
	detailed = new GLubyte[picSize];

	// Creating the blured picture first: 5x5 filter: 
	int result = 0;
	int counter = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int x = -2; x <= 2; x++) {
				for (int y = -2; y <= 2; y++) {

					// Dealing with edeges
					if (j + y > 0 && j + y < width && i + x > 0 && i + x < height) {
						int index = (i+x)*width + (j+y);
						counter++;
						result += pic[index];
					}
				}
			}
			int k = i*width + j;
			blur5[k] = result / counter;
			counter = 0;
			result = 0;
			// After creating the blured 5x5, sub it from the original 8 bit picture and conduct treshhold 
			int temp = abs(pic[k] - blur5[k]);
			if (temp >= 30) {
				detailed[k] = 255;
			
			}
			else {
				detailed[i*width + j] = 0;
			}
		}
	}
}	

void halftonePicture() {

	// Blured picture 3x3 filer (to be used for the halftone)
	blur3 = new GLubyte[picSize];

	// Halftone picture
	halftone = new GLubyte[picSize];

	// First I'm creating a blured pictures with 3x3 filter
	int result = 0;
	int counter = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {

					// Dealing with edeges
					if (j + y > 0 && j + y < width && i + x > 0 && i + x < height) {
						int index = (i + x)*width + (j + y);
						counter++;
						result += pic[index];
					}
				}
			}
			int k = i*width + j;
			blur3[k] = result / counter;
			counter = 0;
			result = 0;
			// After creating the blured 3x3, sub it from the original 8 bit picture and conduct treshhold 
			int temp = abs(pic[k] - blur3[k]);
		}
	}

	// Next, for every third pixel in the blured picture (which contains the avarge of the original its
	// 9 neighbours in the original picture, I'll check it's value between 0 to 1 (devided to 1/10 groups)
	for (int i = 1; i < height; i += 3) {
		for (int j = 1; j < width; j += 3) {
			int k = i*width + j;

			// 0 to 0.1 case
			if (blur3[k] >= 255 * 0 && blur3[k] < 255 * 0.1) {
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
	int rounded;
	float error;
	for (int i = 0; i < picSize; i++) {
		floydTmp[i] = pic[i];
	}
	float alpha = 7.0 / 16.0, beta = 3.0 / 16.0, gamma = 5.0 / 16.0, delta = 1.0 / 16.0;
	for (int i = height - 1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			int k = (i*width) + j;
			// Round the color to 16 grey scale
			rounded = ((floydTmp[(i * width) + j] + 8) / 16) * 16;
			// Calculate error
			error = floydTmp[k] - rounded;
			floydTmp[k] = rounded;

			if (i == 0) {
				floydTmp[k + 1] += int(alpha*error + 0.5);
			}
			else if (j == width - 1) {
				floydTmp[k - width - 1] += int(beta*error + 0.5);
				floydTmp[k - width] += int(gamma*error + 0.5);
			}
			else {
				floydTmp[k + 1] += int(alpha*error + 0.5);
				floydTmp[k - width - 1] += int(beta*error + 0.5);
				floydTmp[k - width] += int(gamma*error + 0.5);
				floydTmp[k - width + 1] += int(delta*error + 0.5);
			}
		}
	}
	for (int i = 0; i < picSize; i++) {
		floydSteinberg[i] = floydTmp[i];
	}
}

void bitmapPictures() {
	bitHalftone = new GLubyte[picSize / 8];
	GLubyte check[] = { 255, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 255, 0, 0 };
	int eightBits = 0;
	int bitLocation = 128;
	for (int i = 0; i < picSize; i++) {
		if (halftone[i] != 0) {
			eightBits = eightBits | bitLocation;
		}
		bitLocation = bitLocation >> 1;
		if (bitLocation < 1) {
			bitLocation = 128;
			bitHalftone[i / 8] = eightBits;
			eightBits = 0;
		}

	}
	//for (int i = 0; i < picSize/8; i++) {
		//printf("%d ", bitHalftone[i]);
	//}
	glRasterPos2f(20, 20);
	glGetFloatv(GL_CURRENT_RASTER_POSITION_VALID, c);
	glOrtho(0, 512, 0, 512, -1.0, 1.0);
	glBitmap(12, 7, 0, 0, 11, 0, bitHalftone);
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
	
	
	// BONUS
	bitmapPictures();
	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, halftone);

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

void printToTXT() {
	FILE *img4, *img5, *img6;
	img4 = fopen("img4.txt", "w");
	img5 = fopen("img5_s.txt", "w");
	img6 = fopen("img6.txt", "w");
	for (int i = 0; i < height*width; i++) {
		if (detailed[i] == 255) {
			fprintf(img4, "%d,", 1);
		}
		else {
			fprintf(img4, "%d,", detailed[i]);
		}
		if (halftone[i] == 255) {
			fprintf(img5, "%d,", 1);

		}
		else {
			fprintf(img5, "%d,", halftone[i]);
		}
		fprintf(img6, "%d,", floydSteinberg[i]/16);
	}
	fclose(img4);
	fclose(img5);
	fclose(img6);
}

void init()
{
	FILE *f;
	// Basic settings for the window
	glEnable(GL_TEXTURE_2D);
	glOrtho(-1.0, 1.0, -1.0, 1.0, 2.0, -2.0);
	glClearColor(0, 0, 0, 0);

	GLubyte colorTable[1024];   

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

	fread(colorTable, 1, 1024, f);

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

	printToTXT();
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