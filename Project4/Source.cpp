#include <stdio.h>
#include <math.h>
#include "glut.h"


GLuint texture[4];
GLuint bitTex;
GLubyte *pic; // original picture
GLubyte *detailed; // details picture
GLubyte *blur5; // blured picture (5x5 filter)
GLubyte *blur3; // blured picture (3x3 filter - to be used with the halftone)
GLubyte *halftone; // halftone picture
GLubyte *floydSteinberg; // floyd Steinberg picture
GLint *floydTmp; // Temporary int array for floyd steinberg picture
GLubyte *bitPicture; // A bit picture - every pixel is represented by one bit
GLint width;
GLint height;
int picSize;
int rd;
GLubyte header[54];
GLubyte colorTable[1024];


/* This method creates the detailed picture */
void detaildPicture() {
	
	// Blur picture 5x5 filter
	blur5 = new GLubyte[picSize];

	// Detailed picture
	detailed = new GLubyte[picSize];

	// Creating the blured picture first: 5x5 filter: 
	int result = 0;
	int counter = 0;
	
	// First two loops for running on each pixel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
		
			// Second two loops, to run on all 25 pixels around it
			for (int x = -2; x <= 2; x++) {
				for (int y = -2; y <= 2; y++) {

					// Dealing with edeges
					if (j + y > 0 && j + y < width && i + x > 0 && i + x < height) {
						int index = (i + x)*width + (j + y);
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

/* This method creates the halftone algorithem - 
	IMPORTANT: As agreed by Tamir, we use a 3X3 blur filter and use the avarage of each 9 pixel to set the halftone picture */
void halftonePicture() {

	// Blured picture 3x3 filer (to be used for the halftone)
	blur3 = new GLubyte[picSize];

	// Halftone picture
	halftone = new GLubyte[picSize];

	// First we're creating a blured pictures with 3x3 filter
	int result = 0;
	int counter = 0;

	// First two loops for running on each pixel
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			// Second two loops, to run on all 9 pixels around it
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
		}
	}

	// Next, for every third pixel in the blured picture (which contains the avarge of the
	// 9 neighbours in the original picture, we'll check its value in a scale of 0 to 1 ( we'll do it by devided to 1/10 groups)
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

/* This method creates the floyd-steinberg filter for the picture*/
void floydSteinbergPicture() {

	// Floyd-Steinberg picture
	floydSteinberg = new GLubyte[picSize];
	
	// A temporary int array where all the calculations will be made (in order the prevent overflows) 
	floydTmp = new GLint[picSize];
	
	int rounded;
	float error;

	// First we copy the original values to the new array
	for (int i = 0; i < picSize; i++) {
		floydTmp[i] = pic[i];
	}
	// The a, b, c, d values as defined by the algorithem
	float alpha = 7.0 / 16.0, beta = 3.0 / 16.0, gamma = 5.0 / 16.0, delta = 1.0 / 16.0;
	
	// For every pixel of the picture
	for (int i = height - 1; i >= 0; i--) {
		for (int j = 0; j < width; j++) {
			int k = (i*width) + j;
			
			// Round the color to 16 grey scale
			rounded = ((floydTmp[(i * width) + j] + 8) / 16) * 16;
			
			// Calculate the error
			error = floydTmp[k] - rounded;
			
			// Switch the value to the rounded value
			floydTmp[k] = rounded;

			// Corner case number 1: the pixel is part of the first line, in that case "spread" the error to the right only 
			if (i == 0) {
				floydTmp[k + 1] += int(alpha*error + 0.5);
			}
			// Corner case number 2: the pixel is part of the last column, in that case "spread" the error to the bottom and bottom back only 
			else if (j == width - 1) {
				floydTmp[k - width - 1] += int(beta*error + 0.5);
				floydTmp[k - width] += int(gamma*error + 0.5);
			}
			// The heart of the algorithem
			else {
				floydTmp[k + 1] += int(alpha*error + 0.5);
				floydTmp[k - width - 1] += int(beta*error + 0.5);
				floydTmp[k - width] += int(gamma*error + 0.5);
				floydTmp[k - width + 1] += int(delta*error + 0.5);
			}
		}
	}
	// Copy the values from the int array to a byte array
	for (int i = 0; i < picSize; i++) {
		floydSteinberg[i] = floydTmp[i];
	}
}

/* Bouns question - This method recieves a regular picture and turn it into a picture where every bit represents a pixel */
GLubyte* bitmapPictures(GLubyte *bytePicture) {
	
	// The array in which the new bit picture will be saved
	bitPicture = new GLubyte[picSize / 8];
	int eightBits = 0;
	int bitLocation = 128;
	
	// Using the binary number 10000000 with the | ('or') operator. every pixel we'll shift the '1' digit, one place to the right. 
	for (int i = 0; i < picSize; i++) {
		if (bytePicture[i] != 0) {
			eightBits = eightBits | bitLocation;
		}
		bitLocation = bitLocation >> 1;
		if (bitLocation < 1) {
			bitLocation = 128;
			bitPicture[i / 8] = eightBits;
			eightBits = 0;
		}
	}
	return bitPicture;
}

/* Texture defitions */
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
	// Deleted since we implemented the bonus

	//************ Third image - bottom left **************************
	// Deleted since we implemented the bonus

	//************ Fourth image - bottom right **************************
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

/* This method prints the picture into text files*/
void printToTXT() {

	// Opeing the three text file. ******IMPORTANT******: As agreed by Tamir we worte to img5 the small halftone we've created, threfore, it's named img5_s.txt
	FILE *img4, *img5_s, *img6;
	img4 = fopen("img4.txt", "w");
	img5_s = fopen("img5_s.txt", "w");
	img6 = fopen("img6.txt", "w");
	
	// Go over each pixel in each picture and write it to the file (Accorrding to the instructions)
	for (int i = 0; i < height*width; i++) {
		if (detailed[i] == 255) {
			fprintf(img4, "%d,", 1);
		}
		else {
			fprintf(img4, "%d,", detailed[i]);
		}
		if (halftone[i] == 255) {
			fprintf(img5_s, "%d,", 1);

		}
		else {
			fprintf(img5_s, "%d,", halftone[i]);
		}
		fprintf(img6, "%d,", floydSteinberg[i] / 16);
	}

	// Close all files.
	fclose(img4);
	fclose(img5_s);
	fclose(img6);
}

/* Initiate the window and the pictures*/
void init() {
	FILE *f;
	// Basic settings for the window
	glEnable(GL_TEXTURE_2D);
	glOrtho(-1.0, 1.0, -1.0, 1.0, 2.0, -2.0);
	glClearColor(0, 0, 0, 0);

	GLubyte colorTable[1024];

	// Open the file
	f = fopen("coloseum21.bmp", "rb");

	//original image header reading
	fread(header, 54, 1, f);
	if (header[0] != 'B' || header[1] != 'M')
		exit(1);  //not a BMP file

	picSize = (*(int*)(header + 2) - 54);
	width = *(int*)(header + 18);
	height = *(int*)(header + 22);

	fread(colorTable, 1, 1024, f);

	// Initilize the original picture
	pic = new GLubyte[picSize];
	
	//read image
	rd = fread(pic, 1, picSize, f);

	// Creating all the pictures, each in its own method
	detaildPicture();
	halftonePicture();
	floydSteinbergPicture();

	// calling the texture functions
	textures();

	// Close the picture file
	fclose(f);

	// Write the pictures into txt files
	printToTXT();
}

/* Display functions */
void mydisplay(void){
	glClear(GL_COLOR_BUFFER_BIT);

	// ************ TOP LEFT PICTURE *******************************
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glViewport(0, 256, 256, 256); // top left position
	glBindTexture(GL_TEXTURE_2D, texture[2]); //using third texture
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

	// ************ TOP RIGHT PICTURE - BONUS ***********************
	glViewport(256, 256, 256, 256);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-1, -1);
	glBitmap(width, height, 0.0, 0.0, 0.0, 0.0, bitmapPictures(detailed));

	// ************ BOTTOM LEFT PICTURE - BONUS ***********************
	glViewport(0, 0, 256, 256);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-1, -1);
	glBitmap(width, height, 0.0, 0.0, 0.0, 0.0, bitmapPictures(halftone));

	
	// ************ BOTTOM RIGHT PICTURE *******************************
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glViewport(256, 0, 256, 256); // bottom right position
	glBindTexture(GL_TEXTURE_2D, texture[3]); //using fourth texture
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

	// Flush it all
	glFlush();
}

int main(int  argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Simple");
	init();
	glutDisplayFunc(mydisplay);
	glutMainLoop();
	delete(pic);
}