#include "mrgl.h"
#include "mropencv.h"

GLuint textureID;
VideoCapture capture(0);
cv::Mat img;
float angle = 0;
float dist = 1.0f;

void draw_gl1(){
	glPushMatrix();
	glTranslatef(0,0,-dist);
	glRotatef(angle,0,0,1);
	glBegin(GL_TRIANGLES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0,0.5,0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(-0.5,-0.5,0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.5,-0.5,0);
    glEnd();
	glPopMatrix();
	angle += 1;
}

void drawFrame(){
	glPushMatrix();
	capture >> img;
	flip(img, img, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	float aspect = img.cols * 1.0f / img.rows;
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-aspect*dist, -dist, -dist);
	glTexCoord2f(0.0, 1.0); glVertex3f(-aspect*dist, dist, -dist);
	glTexCoord2f(1.0, 1.0); glVertex3f(aspect*dist, dist, -dist);
	glTexCoord2f(1.0, 0.0); glVertex3f(aspect*dist, -dist, -dist);
	glEnd();
	glPopMatrix();
}

void display(void){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	drawFrame();
	draw_gl1();
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, w * 1.0f / h, 0.1, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void on_timer(int val){
	glutPostRedisplay();
	glutTimerFunc(20,on_timer,0);
}

void initOpenGL(){
	capture >> img;
	if (!img.data){
		std::cout<<"Failed to open camera "<<std::endl;
		exit(-1);
	}
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(img.cols, img.rows);
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
	glutTimerFunc(30,on_timer,0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	initOpenGL();
	glutMainLoop();
	return 0;
}