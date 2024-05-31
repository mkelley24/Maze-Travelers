//
//  gl_frontEnd.h
//  GL threads
//
//  Created by Jean-Yves Hervé on 2018-12-08, revised 2023-12-01
//

#ifndef GL_FRONT_END_H
#define GL_FRONT_END_H

#include "dataTypes.h"

//------------------------------------------------------------------------------
//	Find out whether we are on Linux or macOS (sorry, Windows people)
//	and load the OpenGL & glut headers.
//	For the macOS, lets us choose which glut to use
//------------------------------------------------------------------------------
//  Windows platform
#if (defined(_WIN32) || defined(_WIN64))
    //  Visual
    #if defined(_MSC_VER)
		#include <Windows.h>
        #include <GL\gl.h>
		#include <GL\glut.h>
    //  gcc-based compiler
    #elif defined(__CYGWIN__) || defined(__MINGW32__)
        #include <GL/gl.h>
        #include <GL/glut.h>
    #elif (defined( __MWERKS__) && __INTEL__))
		#error not supported anymore
    #endif
//  Linux and Unix
#elif  (defined(__FreeBSD__) || defined(linux) || defined(__linux__) || defined(sgi) || defined(__NetBSD__) || defined(__OpenBSD) || defined(__QNX__))
    #include <GL/gl.h>
    #include <GL/glut.h>

//  Macintosh
#elif defined(__APPLE__)
	#if 1
		#include <GLUT/GLUT.h>
		//	Here ask Xcode/clang++ to suppress "deprecated" warnings
		#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	#else
		#include <GL/freeglut.h>
		#include <GL/gl.h>
	#endif
#else
	#error undknown OS
#endif

//-----------------------------------------------------------------------------
//	Function prototypes
//-----------------------------------------------------------------------------

//	I don't want to impose how you store the information about your robots,
//	boxes and doors, so the two functions below will have to be called once for
//	each pair robot/box and once for each door.

//	This draws a colored multi-segment traveler
void drawTraveler(const Traveler& traveler);

//	This function assigns a color to the door based on its number
void drawDoor(int doorNumber, int doorRow, int doorCol);

//	Defined in main.cpp
void speedupTravelers(void);
void slowdownTravelers(void);
void drawTravelers(void);
void updateMessages(void);
void drawMessages(int numMessages, const char*const* message);
void handleKeyboardEvent(unsigned char c, int x, int y);

void initializeFrontEnd(int argc, char* argv[]);
float** createTravelerColors(unsigned int numTravelers);


#endif // GL_FRONT_END_H

