//
//  gl_frontEnd.cpp
//
//  Created by Jean-Yves Hervé on 2018-12-05.  Updated 2023-12-04

 /*-------------------------------------------------------------------------+
 |	A graphic front end for a grid+state simulation.						|
 |																			|
 |	This application simply creates a glut window with a pane to display	|
 |	a colored grid and the other to display some state information.			|
 |	Sets up callback functions to handle menu, mouse and keyboard events.	|
 |	Normally, you shouldn't have to touch anything in this code, unless		|
 |	you want to change some of the things displayed, add menus, etc.		|
 |	Only mess with this after everything else works and making a backup		|
 |	copy of your project.  OpenGL & glut are tricky and it's really easy	|
 |	to break everything with a single line of code.							|
 |																			|
 |	Note:  It should be obvious from my list of included headers that this	|
 |	code was originaly C and that the C++ veener on top of that C code is	|
 |	very thin (basically, I just scopped the FontSize enum type).			|
 +-------------------------------------------------------------------------*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <mutex>
//
#include "gl_frontEnd.h"

using namespace std;

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Extern Variables
//-----------------------------------------------------------------------------
#endif

const extern int MAX_NUM_MESSAGES;
const extern int MAX_LENGTH_MESSAGE;

extern SquareType** grid;
extern unsigned int numRows;			//	height of the grid
extern unsigned int numCols;			//	width
extern unsigned int numLiveThreads;		//	the number of live traveler threads



#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Custom Data Type
//-----------------------------------------------------------------------------
#endif

enum class FontSize
{
	SMALL_FONT = 0,
	MEDIUM_FONT,
	LARGE_FONT
};

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Private Functions' Prototypes
//-----------------------------------------------------------------------------
#endif

void myResizeFunc(int w, int h);
void displayTextualInfo(const char* infoStr, int x, int y, FontSize fontSize);
void myMouseFunc(int b, int s, int x, int y);
void myGridPaneMouseFunc(int b, int s, int x, int y);
void myStatePaneMouse(int b, int s, int x, int y);
void myKeyboardFunc(unsigned char c, int x, int y);
void myTimerFunc(int val);
void createtravelerColors(void);
void freetravelerColors(void);
void drawGrid(void);

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Interface Constants
//-----------------------------------------------------------------------------
#endif

#define SMALL_DISPLAY_FONT    GLUT_BITMAP_HELVETICA_10
#define MEDIUM_DISPLAY_FONT   GLUT_BITMAP_HELVETICA_12
#define LARGE_DISPLAY_FONT    GLUT_BITMAP_HELVETICA_18
const int SMALL_FONT_HEIGHT = 12;
const int LARGE_FONT_HEIGHT = 18;
const int TEXT_PADDING = 0;
const float TEXT_COLOR[4] = {1.f, 1.f, 1.f, 1.f};

const float WALL_COLOR[4] = {0.6f, 0.3f, 0.1f, 1.f};
const float PART_COLOR[4] = {0.6f, 0.6f, 0.6f, 1.f};
const float EXIT_COLOR[4] = {1.f, 1.f, 1.f, 1.f};

const int   INIT_WIN_X = 50,
            INIT_WIN_Y = 40;

const int	GRID_PANE_WIDTH = 900,
			GRID_PANE_HEIGHT = GRID_PANE_WIDTH,
			STATE_PANE_WIDTH = 300,
			STATE_PANE_HEIGHT = GRID_PANE_HEIGHT,
			H_PADDING = 5,
			WINDOW_WIDTH = GRID_PANE_WIDTH + STATE_PANE_WIDTH + H_PADDING,
			WINDOW_HEIGHT = GRID_PANE_HEIGHT;

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark File-level Global Variables
//-----------------------------------------------------------------------------
#endif

//	We use a window split into two panes/subwindows.  The subwindows
//	will be accessed by an index.
const int	GRID_PANE = 0,
			STATE_PANE = 1;
int	gMainWindow,
	gSubwindow[2];

GLfloat** travelerColor;

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Drawing Functions
//-----------------------------------------------------------------------------
#endif

void drawTraveler(const Traveler& traveler)
{
	//	Yes, I know that it's inefficient/dumb to recompute this each and every
	//	the a traveler gets drawn, but gcc on Ubuntu doesn't let me define these
	//	as static [!??].
	const GLfloat	DH = (GRID_PANE_WIDTH - 2.f)/ numCols,
					DV = (GRID_PANE_HEIGHT - 2.f) / numRows;
	const GLfloat segMove[4][2] = {
									{0, DV},	//	NORTH
									{DH, 0},	//	WEST
									{0, -DV},	//	SOUTH
									{-DH, 0}};	//	EAST

	glColor4fv(traveler.rgba);
	glPushMatrix();
	//	The first segment is different
	glTranslatef((traveler.segmentList[0].col + 0.5f)*DH,
				 (traveler.segmentList[0].row + 0.5f)*DV, 0.f);
	//	draw the "head"
	glPushMatrix();
	glScalef(0.2f, 0.2f, 1.f);
	glBegin(GL_POLYGON);
		glVertex2f(0, DV);
		glVertex2f(-DH, 0);
		glVertex2f(0, -DV);
		glVertex2f(DH, 0);
	glEnd();
	glPopMatrix();
	if (traveler.segmentList.size() > 1)
	{
		for (unsigned int currSegIndex=0; currSegIndex<traveler.segmentList.size()-1; currSegIndex++)
		{
			int dirInt = static_cast<int>(traveler.segmentList[currSegIndex].dir);

			//	draw a segment to the center of the next square
			glBegin(GL_LINES);
				glVertex2f(0, 0);
				glVertex2f(segMove[dirInt][0],
						   segMove[dirInt][1]);
			glEnd();
			
			//	and move to that point
			glTranslatef(segMove[dirInt][0],
						 segMove[dirInt][1],
						 0.f);

		}
		//	The last segment is a bit shorter
        glBegin(GL_LINES);
            glVertex2f(0, 0);
            glVertex2f(segMove[static_cast<int>(traveler.segmentList[traveler.segmentList.size()-1].dir)][0]*0.2f,
                       segMove[static_cast<int>(traveler.segmentList[traveler.segmentList.size()-1].dir)][1]*0.2f);
        glEnd();
	}
	else
	{
		//	draw the only segment
		glBegin(GL_LINES);
			glVertex2f(0, 0);
			glVertex2f(segMove[static_cast<int>(traveler.segmentList[0].dir)][0]*0.4f,
					   segMove[static_cast<int>(traveler.segmentList[0].dir)][1]*0.4f);
		glEnd();	}
	
	glPopMatrix();

}


//	This is the function that does the actual grid drawing
void drawGrid(void)
{
	static const GLfloat	DH = (GRID_PANE_WIDTH - 2.f)/ numCols,
							DV = (GRID_PANE_HEIGHT - 2.f) / numRows;
	static const GLfloat	PS = 0.3f, PE = 1.f - PS;

	//	draw the walls and partitions
	for (unsigned int i=0; i< numRows; i++)
	{
		for (unsigned int j=0; j< numCols; j++)
		{
			switch (grid[i][j])
			{
				case SquareType::WALL:
					glColor4fv(WALL_COLOR);
					glBegin(GL_POLYGON);
						glVertex2f(j*DH, i*DV);
						glVertex2f((j+1)*DH, i*DV);
						glVertex2f((j+1)*DH, (i+1)*DV);
						glVertex2f(j*DH, (i+1)*DV);
					glEnd();
					break;
					
					
				case SquareType::VERTICAL_PARTITION:
					glColor4fv(PART_COLOR);
					glBegin(GL_POLYGON);
						glVertex2f((j+PS)*DH, i*DV);
						glVertex2f((j+PE)*DH, i*DV);
						glVertex2f((j+PE)*DH, (i+1)*DV);
						glVertex2f((j+PS)*DH, (i+1)*DV);
					glEnd();
					break;
					
				case SquareType::HORIZONTAL_PARTITION:
					glColor4fv(PART_COLOR);
					glBegin(GL_POLYGON);
						glVertex2f(j*DH, (i+PS)*DV);
						glVertex2f((j+1)*DH, (i+PS)*DV);
						glVertex2f((j+1)*DH, (i+PE)*DV);
						glVertex2f(j*DH, (i+PE)*DV);
					glEnd();
					break;
					
				case SquareType::EXIT:
					glColor4fv(EXIT_COLOR);
					glBegin(GL_POLYGON);
						glVertex2f(j*DH, i*DV);
						glVertex2f((j+1)*DH, i*DV);
						glVertex2f((j+1)*DH, (i+1)*DV);
						glVertex2f(j*DH, (i+1)*DV);
					glEnd();
					glColor4f(0.f, 0.f, 0.f, 1.f);
					glBegin(GL_LINES);
						glVertex2f(j*DH, i*DV);
						glVertex2f((j+1)*DH, (i+1)*DV);
						glVertex2f((j+1)*DH, i*DV);
						glVertex2f(j*DH, (i+1)*DV);
					glEnd();
					break;
				
				default:
					//	nothing
					break;
			}
	
			//	This piece of code displays a small blue square in the upper-left
			//	corner of grid squares in state "TRAVELER".  This lets you verify
			//	that you properly update the grid.
			if (grid[i][j] == SquareType::TRAVELER)
			{
				//	     red  green blue
				glColor4f(0.f, 1.f, 0.f, 1.f);
				
				const float TRAV_DOT_SIZE = 0.2f;	//	fraction of square size

				glBegin(GL_POLYGON);
					glVertex2f(j*DH, i*DV);
					glVertex2f((j+TRAV_DOT_SIZE)*DH, i*DV);
					glVertex2f((j+TRAV_DOT_SIZE)*DH, (i+TRAV_DOT_SIZE)*DV);
					glVertex2f(j*DH, (i+TRAV_DOT_SIZE)*DV);
				glEnd();
			
			}

//	Possibly enable for later versions of the assignment.
//
//	Assuming that you have defined (and properly allocated and initialized)
//	somewhere a 2D array or 2D vector of mutex locks, so
//		mutex** gridLock;
//	or
//		vector<vector<mutex> > gridLock;
//
//	the following piece of code whould display small magenta "locks" at
//	squares where the mutex has been acquired.  That will let you see
//	if you fail to acquire locks or to release locks after your traveler has
//	moved away

//			if (grid[i][j] != SquareType::WALL && grid[i][j] != SquareType::EXIT)
//			{
//				if (lockGrid[i][j].try_lock())
//				{
//					lockGrid[i][j].unlock();
//				}
//				else
//				{
//					//	     red  green blue
//					glColor4f(1.f, 0.f, 1.f, 1.f);
//				
//					const float LOCK_SIZE = 0.2f;	//	fraction of square size
//					glPushMatrix();
//					glTranslatef(j*DH, (i+1.f-LOCK_SIZE)*DV, 0.f);
//					glScalef(LOCK_SIZE*DH, LOCK_SIZE*DV, 1.f);
//					glBegin(GL_POLYGON);
//						glVertex2f(0.f, 0.f);
//						glVertex2f(0.f, 1.f);
//						glVertex2f(1.f, 1.f);
//						glVertex2f(1.f, 0.0f);
//					glEnd();
//					glPopMatrix();
//				}
//			}

		}
	}
	
	//	Then draw a grid of lines on top of the squares
	glColor4f(0.5f, 0.5f, 0.5f, 1.f);
	glBegin(GL_LINES);
		//	Horizontal
		for (unsigned int i=0; i<= numRows+1; i++)
		{
			glVertex2f(0.5f, 0.5f + i*DV);
			glVertex2f(0.5f + numCols*DH, 0.5f + i*DV);
		}
		//	Vertical
		for (unsigned int j=0; j<= numCols+1; j++)
		{
			glVertex2f(0.5f + j*DH, 0.5f);
			glVertex2f(0.5f + j*DH, 0.5f + numRows*DV);
		}
	glEnd();
	
}



void displayTextualInfo(const char* infoStr, int xPos, int yPos, FontSize fontSize)
{
    //-----------------------------------------------
    //  0.  get current material properties
    //-----------------------------------------------
    float oldAmb[4], oldDif[4], oldSpec[4], oldShiny;
    glGetMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
    glGetMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
    glGetMaterialfv(GL_FRONT, GL_SHININESS, &oldShiny);

    glPushMatrix();

    //-----------------------------------------------
    //  1.  Build the string to display <-- parameter
    //-----------------------------------------------
    int infoLn = (int) strlen(infoStr);

    //-----------------------------------------------
    //  2.  Determine the string's length (in pixels)
    //-----------------------------------------------
    int textWidth = 0;
	switch(fontSize)
	{
		case FontSize::SMALL_FONT:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(SMALL_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case FontSize::MEDIUM_FONT:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(MEDIUM_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case FontSize::LARGE_FONT:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(LARGE_DISPLAY_FONT, infoStr[k]);
			}
			break;

		default:
			break;
	}

	//  add a few pixels of padding
    textWidth += 2*TEXT_PADDING;

    //-----------------------------------------------
    //  4.  Draw the string
    //-----------------------------------------------
    glColor4fv(TEXT_COLOR);
    int x = xPos;
	switch(fontSize)
	{
		case FontSize::SMALL_FONT:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(SMALL_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(SMALL_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case FontSize::MEDIUM_FONT:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(MEDIUM_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(MEDIUM_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case FontSize::LARGE_FONT:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(LARGE_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(LARGE_DISPLAY_FONT, infoStr[k]);
			}
			break;

		default:
			break;
	}

    //-----------------------------------------------
    //  5.  Restore old material properties
    //-----------------------------------------------
	glMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, oldShiny);

    //-----------------------------------------------
    //  6.  Restore reference frame
    //-----------------------------------------------
    glPopMatrix();
}


void drawMessages(int numMessages, const char*const* message)
{
	//	I compute once the dimensions for all the rendering of my state info
	//	One other place to rant about that desperately lame gcc compiler.  It's
	//	positively disgusting that the code below is rejected.
	const int LEFT_MARGIN = STATE_PANE_WIDTH / 12;
	const int V_PAD = STATE_PANE_HEIGHT / 12;

	for (int k=0; k<numMessages; k++)
	{
		displayTextualInfo(message[k], LEFT_MARGIN, 3*STATE_PANE_HEIGHT/4 - k*V_PAD,
							FontSize::LARGE_FONT);
	}

	//	display info about number of live threads
	char infoStr[256];
	sprintf(infoStr, "Live Threads: %d", numLiveThreads);
	displayTextualInfo(infoStr, LEFT_MARGIN, 7*STATE_PANE_HEIGHT/8,
						FontSize::LARGE_FONT);
}

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Callback Functions
//-----------------------------------------------------------------------------
#endif


//	This callback function is called when the window is resized
//	(generally by the user of the application).
//	It is also called when the window is created, why I placed there the
//	code to set up the virtual camera for this application.
//
void myResizeFunc(int w, int h)
{
	if ((w != WINDOW_WIDTH) || (h != WINDOW_HEIGHT))
	{
		glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	}
	else
	{
		glutPostRedisplay();
	}
}

void displayGridPaneFunc(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, GRID_PANE_HEIGHT, 0);
	glScalef(1.f, -1.f, 1.f);
	
	drawTravelers();
	
	drawGrid();

	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void displayStatePaneFunc(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	updateMessages();
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void myDisplayFunc(void)
{
    glutSetWindow(gMainWindow);

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

	displayGridPaneFunc();
	displayStatePaneFunc();

    glutSetWindow(gMainWindow);
}

//	This function is called when a mouse event occurs just in the tiny
//	space between the two subwindows.
//
void myMouseFunc(int button, int state, int x, int y)
{
	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}

//	This function is called when a mouse event occurs in the grid pane
//
void myGridPaneMouseFunc(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
			{
				//	do something
			}
			else if (state == GLUT_UP)
			{
				//	exit(0);
			}
			break;

		default:
			break;
	}

	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}

//	This function is called when a mouse event occurs in the state pane
void myStatePaneMouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
			{
				//	do something
			}
			else if (state == GLUT_UP)
			{
				//	exit(0);
			}
			break;

		default:
			break;
	}

	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}


//	This callback function is called when a keyboard event occurs
//
void myKeyboardFunc(unsigned char c, int x, int y)
{
	handleKeyboardEvent(c, x, y);
	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}



void myTimerFunc(int value)
{
	//	value not used.  Warning suppression
	(void) value;

    myDisplayFunc();

	glutTimerFunc(15, myTimerFunc, 0);
}

#if 0
//-----------------------------------------------------------------------------
#pragma mark -
#pragma mark Initialization Functions
//-----------------------------------------------------------------------------
#endif

void initializeFrontEnd(int argc, char* argv[])
{
	//	shutting off warning
	(void) SMALL_FONT_HEIGHT;
	(void) LARGE_FONT_HEIGHT;
	
	//	Initialize glut and create a new window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);


	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(INIT_WIN_X, INIT_WIN_Y);
	gMainWindow = glutCreateWindow("Partitioned Travelers -- CSC 412 - Fall 2020");
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	//	set up the callbacks for the main window
	glutDisplayFunc(myDisplayFunc);
	glutReshapeFunc(myResizeFunc);
	glutMouseFunc(myMouseFunc);
    glutTimerFunc(10, myTimerFunc, 0);

	//	create the two panes as glut subwindows
	gSubwindow[GRID_PANE] = glutCreateSubWindow(gMainWindow,
												0, 0,							//	origin
												GRID_PANE_WIDTH, GRID_PANE_HEIGHT);
    glViewport(0, 0, GRID_PANE_WIDTH, GRID_PANE_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, GRID_PANE_WIDTH, 0.0f, GRID_PANE_HEIGHT, -1, 1);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myGridPaneMouseFunc);
	glutDisplayFunc(displayGridPaneFunc);


	glutSetWindow(gMainWindow);
	gSubwindow[STATE_PANE] = glutCreateSubWindow(gMainWindow,
												GRID_PANE_WIDTH + H_PADDING, 0,	//	origin
												STATE_PANE_WIDTH, STATE_PANE_HEIGHT);
    glViewport(0, 0, STATE_PANE_WIDTH, STATE_PANE_WIDTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, STATE_PANE_WIDTH, 0.0f, STATE_PANE_HEIGHT, -1, 1);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myGridPaneMouseFunc);
	glutDisplayFunc(displayStatePaneFunc);
}

float** createTravelerColors(unsigned int numTravelers)
{
	float** travelerColor = new float*[numTravelers];

	float hueStep = 360.f / numTravelers;

	for (unsigned int k=0; k<numTravelers; k++)
	{
		travelerColor[k] = new float[4];

		//	compute a hue for the door
		float hue = k*hueStep;
		//	convert the hue to an RGB color
		int hueRegion = (int) (hue / 60);
		switch (hueRegion)
		{
				//  hue in [0, 60] -- red-green, dominant red
			case 0:
				travelerColor[k][0] = 1.f;					//  red is max
				travelerColor[k][1] = hue / 60.f;			//  green calculated
				travelerColor[k][2] = 0.f;					//  blue is zero
				break;

				//  hue in [60, 120] -- red-green, dominant green
			case 1:
				travelerColor[k][0] = (120.f - hue) / 60.f;	//  red is calculated
				travelerColor[k][1] = 1.f;					//  green max
				travelerColor[k][2] = 0.f;					//  blue is zero
				break;

				//  hue in [120, 180] -- green-blue, dominant green
			case 2:
				travelerColor[k][0] = 0.f;					//  red is zero
				travelerColor[k][1] = 1.f;					//  green max
				travelerColor[k][2] = (hue - 120.f) / 60.f;	//  blue is calculated
				break;

				//  hue in [180, 240] -- green-blue, dominant blue
			case 3:
				travelerColor[k][0] = 0.f;					//  red is zero
				travelerColor[k][1] = (240.f - hue) / 60;	//  green calculated
				travelerColor[k][2] = 1.f;					//  blue is max
				break;

				//  hue in [240, 300] -- blue-red, dominant blue
			case 4:
				travelerColor[k][0] = (hue - 240.f) / 60;	//  red is calculated
				travelerColor[k][1] = 0;						//  green is zero
				travelerColor[k][2] = 1.f;					//  blue is max
				break;

				//  hue in [300, 360] -- blue-red, dominant red
			case 5:
				travelerColor[k][0] = 1.f;					//  red is max
				travelerColor[k][1] = 0;						//  green is zero
				travelerColor[k][2] = (360.f - hue) / 60;	//  blue is calculated
				break;

			default:
				break;

		}
		travelerColor[k][3] = 1.f;					//  alpha --> full opacity
	}

	return travelerColor;
}
