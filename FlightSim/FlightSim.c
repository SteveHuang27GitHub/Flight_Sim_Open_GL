
/************************************************************************************

	File: 			FlightSim.c

	Description:	Flight sim

	Author:			Michael Northorp

*************************************************************************************/

/* include the library header files */
// Freeglut header
#include <GL\freeglut.h>
#include <GL\Gl.h>
#include <windows.h>
// Math header
#include <math.h>
// File read in
#include <stdio.h>
// Time header for random seeds
#include <time.h>

/* Defines */
// Ratio of the circumference to the diameter of a circle
#define PI 3.14159265f
// Conversion multiplier for converting from degrees to Radians for some calculations
#define DEG_TO_RAD PI/180.0f

// Grid size X by X
#define GRID_SIZE 100.0f

/* Global variables */

/* Typedefs and structs */

// Defines an x,y,z point
typedef GLfloat point3[3];
// Defines a RGB color
typedef GLfloat color4[4];

/* Position variables for camera and ship */

// Keep track of current camera position and set the default
GLfloat cameraPosition[] = {6, 2, 6, 0, 0, 0};

// Set light position
GLfloat lightPosition[] = {0.0, 60.0, 0.0, 1.0};

// Set default plane position
GLfloat planePosition[] = {3.0, 0.5, 3.0};

// Window size parameters
GLfloat windowWidth  = 640.0;
GLfloat windowHeight = 640.0;

// Set up quad objects
GLUquadricObj* quadricCylinder;
GLUquadricObj* quadricDisk;

// Set up display list for the plane
GLuint thePlane = 0;

// Set up display list for propeller
GLuint theProp = 0;

// This is an array of all the vertices for the plane
point3 planeVertices[6763];
// This is an array of all normals for the plane
point3 planeNormals[6763];

// This is an array of all the vertices for the plane
point3 propVertices[6763];
// This is an array of all normals for the plane
point3 propNormals[6763];

// Interp
GLfloat propInterp = 0.0f;

// Key booleans
// Not full screen by default
GLint isFullScreen = 0;
// Wire rendering on by default
GLint isWireRendering = 1;
// Sea ad sky enabled
GLint isSeaAndSky = 0;

// Define colors for plane and propeller
color4 yellow = {1.0, 1.0, 0.0, 1.0};
color4 black = {0.0, 0.0, 0.0, 1.0};
color4 lightPurple = {0.87, 0.58, 0.98, 1.0};
color4 blue = {0.0, 0.0, 1.0, 1.0};
color4 red = {1.0, 0.0, 0.0, 1.0};
color4 green = {0.0, 1.0, 0.0, 1.0};
color4 white = {1.0, 1.0, 1.0, 1.0};
color4 grey = {0.05, 0.05, 0.05, 1.0};
color4 seaBlue = {0.0, 0.3, 0.8, 1.0};
color4 orange = {1.0, 0.5, 0.0, 1.0};


// This handles full screen or getting out of full screen
void fullScreen() {
	if(isFullScreen) {
		glutFullScreen();
	} else {
		glutReshapeWindow(640, 640);
		glutPositionWindow(0, 0);
	}
}

// Enable the fog to be a slight orange
void setUpFog() {
	// define the color of the fog, pink
	GLfloat fogColor[4] = {0.737255, 0.560784, 0.560784, 1.0};
	// Enable the fog
	glEnable(GL_FOG);
	// set the color of the fog
	glFogfv(GL_FOG_COLOR, fogColor);
	// Set the fog mode to exponential
	glFogf(GL_FOG_MODE, GL_EXP);
	// Set the fog density
	glFogf(GL_FOG_DENSITY, 0.005);
}

// Sets up the propeller by reading it in
void setUpProp() {
	int i = 0;
	int j = 0;
	int objectCount = -1;
	int isFace = 0;
	char firstChar;
	char *token;

	// Set up a file
	FILE * fileStream;
	// Char array to store
	char string[100];
	fileStream = fopen("prop.txt", "rt");

	// Make sure the file stream is not null
	if (fileStream != NULL)
	{
		// Puts the ship in a display list
		theProp = glGenLists(1);
	  	glNewList(theProp, GL_COMPILE);

		// Read each file line while it is not null, store in char array
		while(fgets(string, 100, fileStream) != NULL)
		{
			firstChar = ' ';
			// Store the plane vertices as it reads the file
			if(sscanf(string, "v %f %f %f ", &propVertices[i][0], &propVertices[i][1], &propVertices[i][2]) == 3) {
				// Above stores vertices in the plane vertices array
				// Increase i for reading in vertices
				i++;
			} else if(sscanf(string, "n %f %f %f ", &propNormals[j][0], &propNormals[j][1], &propNormals[j][2]) == 3) {
				// Above stores  the normals in the planenormals array
				j++;
			} else if(sscanf(string, "%c", &firstChar) == 1) { // Check if it is a face and tokenize it
				if(firstChar == 'f') {
					// Check for faces
					token = strtok(string, " ");
					// Get next token
					token = strtok(NULL, " ");

					// Draw polygon for this face
					glBegin(GL_POLYGON);
						glLineWidth(1);
						while(token != NULL ) {
							// Draw the normal and point
							glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);
							if(objectCount <= 0) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
								glMaterialfv(GL_FRONT, GL_AMBIENT, orange);
							} else if(objectCount <= 1) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
								glMaterialfv(GL_FRONT, GL_AMBIENT, red);
							} else {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
								glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
							}
							// Get normal and draw color
							glNormal3f(propNormals[atoi(token)-1][0], propNormals[atoi(token)-1][1], propNormals[atoi(token)-1][2]);
							glVertex3f(propVertices[atoi(token)-1][0], propVertices[atoi(token)-1][1], propVertices[atoi(token)-1][2]);
							// Get next token
							token = strtok(NULL, " ");
						}
					glEnd(); // End drawing of polygon
				} else if (firstChar == 'g') {
					// Increase object count
					objectCount++;
				}
			}
		}
		// End the display list
		glEndList();
	}
	fclose (fileStream);
}

// Draws the propellers
void drawProps() {
	// Draw first propeller
	glPushMatrix();
		// Position it in front of plane
		glTranslatef(planePosition[0]-0.3, planePosition[1]-0.1, planePosition[2]+0.2);
		// Rotate so it is facing away
		glRotatef(-45, 0.0f, 1.0f, 0.0f);

		// Rotate propeller
		glRotatef(propInterp*360, 1.0f, 0.0f, 0.0f);
		glTranslatef(0, 0.15f, -0.35f);

		// Draw propeller
		glCallList(theProp);
	glPopMatrix();

	// Draw second propeller
	glPushMatrix();
		// Position it in front of plane
		glTranslatef(planePosition[0]+0.2, planePosition[1]-0.1, planePosition[2]-0.3);
		// Rotate so it is facing away
		glRotatef(-45, 0.0f, 1.0f, 0.0f);

		// Rotate propeller
		glRotatef(propInterp*360, 1.0f, 0.0f, 0.0f);
		glTranslatef(0, 0.15f, -0.35f);

		// Draw propeller
		glCallList(theProp);
	glPopMatrix();
}

// Sets up the plane by reading it in
void setUpPlane() {
	int i = 0;
	int j = 0;
	int objectCount = -1;
	int isFace = 0;
	char firstChar;
	char *token;

	// Set up a file
	FILE * fileStream;
	// Char array to store
	char string[100];
	fileStream = fopen("plane.txt", "rt");

	// Make sure the file stream is not null
	if (fileStream != NULL)
	{
		// Puts the ship in a display list
		thePlane = glGenLists(1);
	  	glNewList(thePlane, GL_COMPILE);

		// Read each file line while it is not null, store in char array
		while(fgets(string, 100, fileStream) != NULL)
		{
			firstChar = ' ';
			// Store the plane vertices as it reads the file
			if(sscanf(string, "v %f %f %f ", &planeVertices[i][0], &planeVertices[i][1], &planeVertices[i][2]) == 3) {
				// Above stores vertices in the plane vertices array
				// Increase i for reading in vertices
				i++;
			} else if(sscanf(string, "n %f %f %f ", &planeNormals[j][0], &planeNormals[j][1], &planeNormals[j][2]) == 3) {
				// Above stores  the normals in the planenormals array
				j++;
			} else if(sscanf(string, "%c", &firstChar) == 1) { // Check if it is a face and tokenize it
				if(firstChar == 'f') {
					// Check for faces
					token = strtok(string, " ");
					// Get next token
					token = strtok(NULL, " ");

					// Draw polygon for this face
					glBegin(GL_POLYGON);
						glLineWidth(1);
						while(token != NULL ) {
							// Draw the normal and point
							glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);
							if(objectCount <= 3) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 5) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 6) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, lightPurple);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 7) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 10) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 11) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 13) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 25) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else if(objectCount <= 32) {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							} else {
								glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
								glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
								glMaterialfv(GL_FRONT, GL_SPECULAR, white);
							}
							// Get normal and draw color
							glNormal3f(planeNormals[atoi(token)-1][0], planeNormals[atoi(token)-1][1], planeNormals[atoi(token)-1][2]);
							glVertex3f(planeVertices[atoi(token)-1][0], planeVertices[atoi(token)-1][1], planeVertices[atoi(token)-1][2]);
							// Get next token
							token = strtok(NULL, " ");
						}
					glEnd(); // End drawing of polygon
				} else if (firstChar == 'g') {
					// Increase object count
					objectCount++;
				}
			}
		}
		// End the display list
		glEndList();
	}
	fclose (fileStream);
}

// Draws the plane
void drawPlane() {
	glPushMatrix();
		glTranslatef(planePosition[0], planePosition[1], planePosition[2]);
		// Rotate the ship so it is facing away
		glRotatef(-45, 0.0f, 1.0f, 0.0f);
		glCallList(thePlane);
	glPopMatrix();
}

// Draw the sky and sea with gluQuad objects
void drawSkyAndSea() {
	// Set up disk and cylinder
	quadricCylinder = gluNewQuadric();
	quadricDisk = gluNewQuadric();

	// Set up draw style to line
	if(isWireRendering) {
		gluQuadricDrawStyle(quadricCylinder, GLU_LINE);
		gluQuadricDrawStyle(quadricDisk, GLU_LINE);
	} else {
		gluQuadricDrawStyle(quadricCylinder, GLU_FILL);
		gluQuadricDrawStyle(quadricDisk, GLU_FILL);
	}

	// Enable or disable wirerendering based on button press
	if(isWireRendering) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	// Set up normals
	glShadeModel(GL_SMOOTH);
	gluQuadricNormals(quadricCylinder, GLU_SMOOTH);
	gluQuadricDrawStyle(quadricDisk, GLU_SMOOTH);

	// Draw cylinder
	glPushMatrix();
		glLineWidth(1);
		glTranslatef(-30.0, -20.0, -30.0);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, orange);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
		gluCylinder(quadricCylinder, 500, 500, 500, 250, 100);
	glPopMatrix();

	// Draw disk base
	glPushMatrix();
		// Setup fog for the sea only
		setUpFog();

		// Line width is 1
		glLineWidth(1);

		// Move it to correct position
		glTranslatef(-30.0, -20.0, -30.0);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seaBlue);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey);
		gluDisk(quadricCylinder, 0, 501, 100, 100);
	glPopMatrix();

	// Disable the fog
	glDisable(GL_FOG);
}

// Draw the grid and basic frame of reference
void drawFrameReferenceGrid() {
	// Setup for loop
	int i = 0;
	int j = 0;

	// Call draw functions here
	glPushMatrix();

	// Set line width to 1
	glLineWidth(1);

	// Draw the grid of size grid size and translate it to the origin
	glTranslatef(GRID_SIZE/2, 0.0, -GRID_SIZE/2);
		// Enable or disable wirerendering based on button press
		if(isWireRendering) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Draw a row in the grid
		for(i = 0;i<GRID_SIZE;i++) {
			glTranslatef(-GRID_SIZE, 0.0, 0.0f);
			glTranslatef(0.0, 0.0, 1.0);
			for(j = 0;j<GRID_SIZE;j++) {
				glTranslatef(1.0, 0.0, 0.0);
				// Draw the grid
				glBegin(GL_QUADS);
					// Loop through to draw each square
					glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
					glMaterialfv(GL_FRONT, GL_AMBIENT, white);
					glNormal3f(0.0f, 0.0f, 0.0f);
					glVertex3f(0.0f, 0.0f, 0.0f);

					glNormal3f(0.0f, 0.0f, 1.0f);
					glVertex3f(0.0f, 0.0f, 1.0f);

					glNormal3f(1.0f, 0.0f, 1.0f);
					glVertex3f(1.0f, 0.0f, 1.0f);

					glNormal3f(1.0f, 0.0f, 0.0f);
					glVertex3f(1.0f, 0.0f, 0.0f);
				glEnd();
			}
		}
	glPopMatrix();

	// Draw frame of reference for origin
	// X direction
	glPushMatrix();
		// Move slightly above so it draws above grid lines
		glTranslatef(0.0, 0.05, 0.0);
		// Set line width to 5
		glLineWidth(5);

		// Draw the X direction
		glBegin(GL_LINES);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
			glMaterialfv(GL_FRONT, GL_AMBIENT, red);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(2.0f, 0.0f, 0.0f);
			glVertex3f(2.0f, 0.0f, 0.0f);
		glEnd();

		// Y direction
		glBegin(GL_LINES);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
			glMaterialfv(GL_FRONT, GL_AMBIENT, green);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(0.0f, 2.0f, 0.0f);
			glVertex3f(0.0f, 2.0f, 0.0f);
		glEnd();

		// Z direction
		glBegin(GL_LINES);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
			glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
			glNormal3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glNormal3f(0.0f, 0.0f, 2.0f);
			glVertex3f(0.0f, 0.0f, 2.0f);
		glEnd();

		// Draw circle in middle
		glMaterialfv(GL_FRONT, GL_DIFFUSE, grey);
		glMaterialfv(GL_FRONT, GL_AMBIENT, grey);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glutSolidSphere(0.2, 20, 20);
		glLineWidth(1);
	glPopMatrix();
}

/************************************************************************

	Function:		normalKeys

	Description:	This checks when a key is pressed that controls one of the
					features in the program.

*************************************************************************/
void normalKeys(unsigned char key, int x, int y) {
	switch(key) {
		// Toggle full screen
		case 'f':
			// Set full screen to opposite
			isFullScreen = !isFullScreen;
			// Call fullscreen function
			fullScreen();
			break;
		// Toggle full screen
		case 'w':
			// Set full screen to opposite
			isWireRendering = !isWireRendering;
			break;
		case 's':
			// Set full screen to opposite
			isSeaAndSky = !isSeaAndSky;
			break;
		// Quit the program gracefully
		case 'q':
			exit(0);
			break;
		default:
			break;
	}
}

/************************************************************************

	Function:		specialKeys

	Description:	This checks when a special key is pressed to move the
					ship.

*************************************************************************/
void specialKeys(int key, int x, int y) {

}

/************************************************************************

	Function:		specialKeysReleased

	Description:	This checks when a special key is released

*************************************************************************/
void specialKeysReleased(int key, int x, int y) {

}

/************************************************************************

	Function:		printOutcontrols

	Description:	This prints out all of the controls for the program.

*************************************************************************/
void printOutControls() {
	printf("Scene Controls\n--------------\n");
	printf("w: Toggle between wireframe and solid draw mode\n");
	printf("f: Toggle between fullscreen and non fullscreen\n");
	printf("s: Toggle between sea and sky and frame reference grid\n");
	printf("q: Quit the program\n");
}


/************************************************************************

	Function:		init

	Description:	This performs the main setup for openGl, it sets the
					projection mod to gl_projection and modelview. It also sets
					the perspective and performs a number of setup functions.
					These display the controls, set up the planets, stars and ships.
*************************************************************************/
void init(void)
{
	// Set up light colors
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};

	// Set global ambient
	GLfloat globalAmbient[] = {0.05, 0.05, 0.05, 1.0};

	// Set initial window position and size
	glutReshapeWindow(windowWidth, windowHeight);
	glutPositionWindow(0, 0);

	// Enable the depth test
	glEnable(GL_DEPTH_TEST);

	// Set the background to be black
	glClearColor(0, 0, 0, 1.0);

	// Set to projection mode so we can use camera functions
    glMatrixMode(GL_PROJECTION);

	// Load identity matrix onto projection matrix
    glLoadIdentity();

    // gluPerspective(fovy, aspect, near, far)
    gluPerspective(90, windowWidth/windowHeight, 0.1, 40000);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Enable light source
	glEnable(GL_LIGHT0);

	// Enable smooth shading
	glShadeModel(GL_SMOOTH);

	// Enable normalization
	glEnable(GL_NORMALIZE);

	// Set up light colors
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Global ambient
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	// Shade both sides
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);

	// Setup plane
	setUpPlane();

	// Setup propeller
	setUpProp();

	// Print out the controls
	printOutControls();
}

/************************************************************************

	Function:		myIdle

	Description:	This runs whenever the program is idle. It handles most
					of the dynamic functionality of the program.
					This includes rotating planets, changing interp values for
					shield and klingons moving.

*************************************************************************/
void myIdle(void)
{
	// Rotation speed of the plane
	if(propInterp >= 1.0) {
		propInterp = 0;
	} else {
		propInterp += 0.05;
	}

	// Force a redraw in OpenGL
	glutPostRedisplay();
}

/************************************************************************

	Function:		myResize

	Description:	Handles a user resize of the window.

*************************************************************************/
void myResize(int newWidth, int newHeight)
{

	// Update width of window
	windowWidth = newWidth;
	// Update height of window
	windowHeight = newHeight;

	// Update the viewport to still be all of the window
	glViewport (0, 0, windowWidth, windowHeight);

	// Change camera properties
    glMatrixMode(GL_PROJECTION);

	// Load identity matrix
    glLoadIdentity();

    // Modify the gluPerspective (fovy, aspect, near, far)
    gluPerspective(45, windowWidth/windowHeight, 0.1, 40000);

    // Back into modelview
	glMatrixMode(GL_MODELVIEW);
}

/************************************************************************

	Function:		display

	Description:	Clears color and depth buffer. Sets up the camera position
					and look at position. Draws planets and other objects based
					on button toggles. Always draws planets and ship.

*************************************************************************/
void display(void)
{
	// Clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load the indentity matrix
	glLoadIdentity();

	// Set up the camera position
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], cameraPosition[3], cameraPosition[4], cameraPosition[5], 0, 1, 0);

	// Set light position
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// Draw the frame of reference and basic grid
	if(isSeaAndSky) {
		// Draw sky and sea and enable the fog for sea
		drawSkyAndSea();
	} else {
		drawFrameReferenceGrid();
		// Disable the fog
		glDisable(GL_FOG);
	}

	drawPlane();

	// Draw propellers
	drawProps();

	// Swap the drawing buffers here
	glutSwapBuffers();
}


/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop. Display mode is
					set to double buffering, and idle function and keyboard
					event listeners are set up. glut depth display mode is set.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(windowHeight, windowWidth);
	// open the screen window
	glutCreateWindow("Flight Sim");
	//initialize the rendering context
	init();
	// register the idle function
	glutIdleFunc(myIdle);
	// This handles keyboard input for normal keys
	glutKeyboardFunc(normalKeys);
	// This handles keyboard input for special keys
	glutSpecialFunc(specialKeys);
	// This checks when a special key is released
	glutSpecialUpFunc(specialKeysReleased);
	// register redraw function
	glutDisplayFunc(display);
	// Register the resize function
	glutReshapeFunc(myResize);
	// go into a perpetual loop
	glutMainLoop();
}