#include "mrgl.h"
#include "mropencv.h"

GLuint textureID;
VideoCapture capture(0);
cv::Mat img;
float angle = 0;

void drawFrame(){
	glPushMatrix();
	capture >> img;
	flip(img, img, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	glTranslatef(0.0f, 0.0f, -3.6f);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-3.2, -2.4, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(3.2, 2.4, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(3.2, -2.4, 0.0);
	glEnd();
	glRotatef(angle,0,0,1);
	glBegin(GL_TRIANGLES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0,0.5,1);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(-0.5,-0.5,1);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.5,-0.5,1);
    glEnd();
	angle+=1;
	glPopMatrix();
}

void display(void){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawFrame();
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, w * 1.0f / h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initOpenGL(){
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("glcv");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	initOpenGL();
	glutMainLoop();
	return 0;
}