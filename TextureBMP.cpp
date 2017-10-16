/*
 * TextureBMP.cpp
 *
 * Loosely based on the example checker.c 
 * (Example 9-1, OpenGL Programming Guide, 3rd edition) 
 * 
 * Read an image from a bitmap (.bmp) file, and draw it as a texture 
 * map on a quad.  A cpp class RgbImage is used to read a texture
 * map from a bitmap (.bmp) file.
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book.
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 * Modified by Jeremy Kyle Delima for CPSC 453 Assignment 1
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "RgbImage.h"
#include <iostream>
#include <math.h>
#include <string>

// Menu macros
#define MENU_LOAD         1
#define MENU_QUANTIZATION 2
#define MENU_BRIGHTNESS   3
#define MENU_SATURATION   4
#define MENU_SCALING      5
#define MENU_ROTATION     6
#define MENU_CONTRAST     7
#define MENU_SAVE         8

// Image variables
GLuint originalID;
GLuint modifiedID;
RgbImage * originalImage;
RgbImage * modifiedImage;
RgbImage * scaledImage;
RgbImage * rotatedImage;

// List of pictures
const char* filename1 = "landscape1.bmp";
const char* filename2 = "landscape2.bmp";
const char* filename3 = "landscape3.bmp";
const char* filename4 = "clouds1.bmp";
const char* filename5 = "clouds2.bmp";
const char* filename6 = "clouds3.bmp";
const char* filename7 = "clouds4.bmp";

// Global variables
using namespace std;
int file;
bool modFlag = false;
bool scaleFlag = false;
bool rotateFlag = false;
double param;

// Function constructors
void processMenu(int choice);
void load(void);
void quantize(void);
void bright(void);
void saturate(void);
void scale(void);
void rotate(void);
void contrast(void);
void save(void);

/*
 * This function chooses the picture to operate on.
 * This takes in the choice of the user as input.
 * This outputs the filename of the picture.
 */
const char * pickFile(int choice)
{
	switch (choice){
		case 1: return filename1; break;
		case 2: return filename2; break;
		case 3: return filename3; break;
		case 4: return filename4; break;
		case 5: return filename5; break;
		case 6: return filename6; break;
		case 7: return filename7; break;
	}
	return 0;
}

/*
 * This function displays the menu from a right click on the app.
 */
void createMenu()
{
	glutCreateMenu(processMenu);
	glutAddMenuEntry("Load Image", MENU_LOAD);
	glutAddMenuEntry("Quantization", MENU_QUANTIZATION);
	glutAddMenuEntry("Brightness", MENU_BRIGHTNESS);
	glutAddMenuEntry("Saturation", MENU_SATURATION);
	glutAddMenuEntry("Scaling", MENU_SCALING);
	glutAddMenuEntry("Rotation", MENU_ROTATION);
	glutAddMenuEntry("Contrast", MENU_CONTRAST);
	glutAddMenuEntry("Save Modified", MENU_SAVE);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*
 * This function handles the menu operations being clicked.
 * This takes in the choice of the user as input.
 */
void processMenu(int choice)
{
	switch (choice){
		case MENU_LOAD: cout << "Selecting image to load..." << endl; load(); break;
		case MENU_QUANTIZATION: cout << "Enter quantization level:" << endl; quantize(); break;
		case MENU_BRIGHTNESS: cout << "Enter brightness factor:" << endl; bright(); break;
		case MENU_SATURATION: cout << "Enter saturation factor:" << endl; saturate(); break;
		case MENU_SCALING: cout << "Scaling image by a factor of 2..." << endl; scale(); break;
		case MENU_ROTATION: cout << "Rotating image by 45 degrees..." << endl; rotate(); break;
		case MENU_CONTRAST: cout << "Enter contrast factor..." << endl; contrast(); break;
		case MENU_SAVE: cout << "Saving image..." << endl; save(); break;
	}
}

/*
 * This function updates the texture.
 * This takes in the image map as input.
 */
void updateTexture(RgbImage * theTexMap)
{	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, theTexMap -> GetNumCols(), theTexMap -> GetNumRows(),
		GL_RGB, GL_UNSIGNED_BYTE, theTexMap -> ImageData() );
}

/*
 * This function loads the texture of the picture.
 * This takes in the image map, picture filename, and the texture ID as input.
 */
void loadTextureFromFile(RgbImage * theTexMap, const char * filename, GLuint & texID)
{    
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	theTexMap = new RgbImage(filename);
	originalImage = theTexMap;
	modifiedImage = new RgbImage(filename);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	updateTexture(theTexMap);
}

/*
 * This function creates the background display of the application.
 */
void drawScene1(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// The following creates the outline for the original image
	glBindTexture(GL_TEXTURE_2D, originalID);
	glBegin(GL_QUADS);   
	glTexCoord2f(0.0, 0.0); 
	glVertex3f(-0.9, 0.1, 0.0);
	glTexCoord2f(0.0, 1.0); 
	glVertex3f(-0.9, 0.9, 0.0);
	glTexCoord2f(1.0, 1.0); 
	glVertex3f(-0.1, 0.9, 0.0);
	glTexCoord2f(1.0, 0.0); 
	glVertex3f(-0.1, 0.1, 0.0);
	glEnd();

	// The following creates the outline for the modified image
	glBindTexture(GL_TEXTURE_2D, modifiedID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, -0.9, 0.0);
	glTexCoord2f(0.0, 1.0); 
	glVertex3f(0.1, -0.1, 0.0);
	glTexCoord2f(1.0, 1.0); 
	glVertex3f(0.9, -0.1, 0.0);
	glTexCoord2f(1.0, 0.0); 
	glVertex3f(0.9, -0.9, 0.0);  
	glEnd();
	
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

/*
 * This function creates the background display only for the scaled function.
 */
void drawScene2(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, originalID);
	glBegin(GL_QUADS);   
	glTexCoord2f(0.0, 0.0); 
	glVertex3f(-0.9, 0.1, 0.0);
	glTexCoord2f(0.0, 1.0); 
	glVertex3f(-0.9, 0.9, 0.0);
	glTexCoord2f(1.0, 1.0); 
	glVertex3f(-0.1, 0.9, 0.0);
	glTexCoord2f(1.0, 0.0); 
	glVertex3f(-0.1, 0.1, 0.0);
	glEnd();

	// New vertices to scale the image by a factor of 2
	glBindTexture(GL_TEXTURE_2D, modifiedID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.7, -0.9, 0.0);
	glTexCoord2f(0.0, 1.0); 
	glVertex3f(-0.7, -0.1, 0.0);
	glTexCoord2f(1.0, 1.0); 
	glVertex3f(0.9, -0.1, 0.0);
	glTexCoord2f(1.0, 0.0); 
	glVertex3f(0.9, -0.9, 0.0);  
	glEnd();
	
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

/*
 * This function resizes the image window if the window is manually being resized.
 * This takes in the width and height of the window as input.
 */
void resizeWindow(int w, int h)
{
	float viewWidth = 1.1;
	float viewHeight = 1.1;
	glViewport(0, 0, w, h);
	h = (h==0) ? 1 : h;
	w = (w==0) ? 1 : w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h < w) viewWidth *= (float)w/(float)h;
	else viewHeight *= (float)h/(float)w;
	glOrtho( -viewWidth, viewWidth, -viewHeight, viewHeight, -1.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
 * This function handles keyboard controls.
 * Only the escape function is being supported as in 'case 27'.
 * This takes in the key input, and two integers.
 */
void keyboard (unsigned char key, int x, int y)
{
	switch (key){
		case 27:
		exit(0);
		break;
	default:
		break;
	}
}

/*
 * This function loads the images onto the application window.
 */
void load()
{
	cout << "Enter an integer from 1 to 7 to load a file:" << endl;
	cin >> file;
	loadTextureFromFile(originalImage, pickFile(file), originalID);
	loadTextureFromFile(modifiedImage, pickFile(file), modifiedID);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * quantizes the original image given a parameter.
 */
void quantize()
{
	modFlag = true; scaleFlag = false; rotateFlag = false; // Sets flags used for saving image
	double max = 255;
	cin >> param; // User input
	cout <<	"Quantizing by " << param << endl;
	param = param - 1;
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage -> GetNumCols(); y++){	
			double r, g, b;
			originalImage->GetRgbPixel(x, y, &r, &g, &b);
			// The following are calculations for r, g, and b values for the new modified image
			r = r * max;
			if (r > max) r = max;
			r = max * (floor(r * param / max) / param) / max;
			g = g * max;
			if (g > max) g = max;
			g = max * (floor(g * param / max) / param) / max;
			b = b * max;
			if (b > max) b = max;
			b = max * (floor(b * param / max) / param) / max;
			modifiedImage -> SetRgbPixelf(x, y, r, g, b);
		}
	}
	updateTexture(modifiedImage);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * changes the brightness of the original image given a parameter.
 */
void bright()
{
	modFlag = true; scaleFlag = false; rotateFlag = false; // Sets flags used for saving image
	cin >> param; // User input
	cout <<	"Updating brightness by " << param << endl;
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage -> GetNumCols(); y++){
			double r, g, b;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			// The following are calculations for r, g, and b values for the new modified image
			r = r * param;
			g = g * param;
			b = b * param;
			modifiedImage -> SetRgbPixelf(x, y, r, g, b);
		}
	}
	updateTexture(modifiedImage);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * changes the saturation of the original image given a parameter.
 */
void saturate()
{
	modFlag = true; scaleFlag = false; rotateFlag = false; // Sets flags used for saving image
	cin >> param; // User input
	cout <<	"Saturating by " << param << endl;
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage -> GetNumCols(); y++){
			double r,g,b, luminance;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			// The following are calculations for r, g, and b values for the new modified image
			luminance = (0.3 * r) + (0.59 * g) + (0.11 * b);
			r = ((1 - param) * luminance) + (param * r);
			g = ((1 - param) * luminance) + (param * g);
			b = ((1 - param) * luminance) + (param * b);
			modifiedImage -> SetRgbPixelf(x, y, r, g, b);
		}
	}
	updateTexture(modifiedImage);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * scales the original image along the x-axis by a factor of 2.
 */
void scale()
{
	modFlag = false; scaleFlag = true; rotateFlag = false; // Sets flags used for saving image
	cout <<	"Scaling along x-axis by a factor of 2" << endl;
	// A new image is created with a different width
	scaledImage = new RgbImage(originalImage -> GetNumRows(), originalImage -> GetNumCols() * 2);
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage -> GetNumCols(); y++){
			double r,g,b;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			// The scaled image number of columns is just doubling the original's
			scaledImage -> SetRgbPixelf(x, y * 2, r, g, b);
		}
	}
	updateTexture(scaledImage);
	// We use the second drawScene here.
	drawScene2();
}

/*
 * This function gets called from the menu and
 * rotates the original image counter-clockwise by 45 degrees.
 */
void rotate()
{
	modFlag = false; scaleFlag = false; rotateFlag = true; // Sets flags used for saving image
	cout <<	"Rotating image 45 degrees counter-clockwise" << endl;
	double x, y, x1, y1, x2, y2, xmax, ymax, xnew, ynew;
	// The following are calculations for length and width values for the new modified image
	x1 = (originalImage -> GetNumCols() * cos(M_PI/4)) - (originalImage -> GetNumRows() * sin(M_PI/4));
	y1 = (originalImage -> GetNumCols() * sin(M_PI/4)) + (originalImage -> GetNumRows() * cos(M_PI/4));
	x2 = (originalImage -> GetNumCols() * cos(M_PI/4));
	y2 = (originalImage -> GetNumCols() * sin(M_PI/4));
	if (x1 > x2) {xmax = x1;}
	else xmax = x2;
	if (y1 > y2) {ymax = y1;}
	else ymax = y2;
	// A new image is created with a different height and width
	rotatedImage = new RgbImage(ymax, xmax);
	for (x = 0; x < originalImage -> GetNumRows(); x++){
		for (y = 0; y < originalImage -> GetNumCols(); y++){
			double r,g,b;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			// The following are calculations for x and y values for the new modified image
			ynew = y * cos(M_PI/4) - x * sin(M_PI/4);
			xnew = x * sin(M_PI/4) + y * cos(M_PI/4);
			if (ynew >= 0) rotatedImage -> SetRgbPixelf(xnew, ynew, r, g, b);
		}
	}
	updateTexture(rotatedImage);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * changes the saturation of the original image given a parameter.
 */
void contrast()
{
	modFlag = true; scaleFlag = false; rotateFlag = false; // Sets flags used for saving image
	double aveL = 0, pix = 0;
	cin >> param; // User input
	cout <<	"Changing contrast by " << param << endl;
	// The following calculates the average luminance of the original image
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage ->GetNumCols(); y++){
			double r,g,b, luminance;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			luminance = (0.3 * r) + (0.59 * g) + (0.11 * b);
			aveL += luminance;
			pix++;
		}
	}
	aveL = aveL / pix;
	for (int x = 0; x < originalImage -> GetNumRows(); x++){
		for (int y = 0; y < originalImage -> GetNumCols(); y++){
			double r,g,b;
			originalImage -> GetRgbPixel(x, y, &r, &g, &b);
			// The following are calculations for r, g, and b values for the new modified image
			r = ((1 - param) * aveL) + (param * r);
			g = ((1 - param) * aveL) + (param * g);
			b = ((1 - param) * aveL) + (param * b);
			modifiedImage -> SetRgbPixelf(x, y, r, g, b);
		}
	}
	updateTexture(modifiedImage);
	glutPostRedisplay();
}

/*
 * This function gets called from the menu and
 * saves the modified image with a specified output filename.
 */
void save()
{
	string outfile;
	cout << "Enter output file name:" << endl;
	cin >> outfile; // User input
	if (modFlag) {modifiedImage -> WriteBmpFile((char *)outfile.c_str());}
	else if (scaleFlag) {scaledImage -> WriteBmpFile((char *)outfile.c_str());}
	else {rotatedImage -> WriteBmpFile((char *)outfile.c_str());}
}

/*
 * This is the main function of our program.
 */
int main(int argc, char** argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 750);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 1");
	createMenu();
	glutDisplayFunc(drawScene1);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

