#pragma once

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#if __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#if _WIN32
	#pragma comment(lib,"OPENGL32.lib")
	#pragma comment(lib,"glu32.lib")
	#pragma comment(lib,"glut32.lib")
	#pragma comment(lib,"GLAUX.LIB")
#endif