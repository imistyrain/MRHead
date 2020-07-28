#ifndef MRGL_H
#define MRGL_H

#if _WIN32
	#include <GL/glew.h>
#else
	#define GL_SILENCE_DEPRECATION
	#define GL_GLEXT_PROTOTYPES
#endif
#if __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

//#if _WIN32
//	#pragma comment(lib,"OPENGL32.lib")
//	#pragma comment(lib,"glu32.lib")
//	#pragma comment(lib,"glut32.lib")
//	#pragma comment(lib,"GLAUX.LIB")
//#endif

#endif