#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include "math.h"
#include "algorithm"

int g_screen_width = 400;
int g_screen_height = 400;
Shader *shader;
float cam_pos[3] = {0,0,1.6f};
float model_pos[3] = {0,0,0};
GLint g_lastX = 0;
GLint g_lastY = 0;
bool g_bLbutton= false;
bool g_bShowCube = true;
bool g_bShowAxis = true;
bool g_bShowFrustum =true;
bool g_bShowGrid = true;
glm::mat4 g_model;
glm::mat4 g_view;
glm::mat4 g_mv;

void reshape(int w,int h){
    g_screen_width = w;
    g_screen_height = h;
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, w * 1.0f / h, 0.1, 10);
    glMatrixMode(GL_MODELVIEW);
}

void drawAxis(float scale = 0.6){
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.6, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.6, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.6);
	glEnd();
	
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0;i<10*(scale/2);i++){
		float axis = i * 1.0/ 10;
		glVertex3f(axis,0,0);
		glVertex3f(0,axis,0);
		glVertex3f(0,0,axis);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.58,0,0);
	glRotatef(90,0,1,0);
	glutSolidCone(0.01,0.03,100,10);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.58,0);
	glRotatef(270,1,0,0);
	glutSolidCone(0.01,0.03,100,10);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.0,0.58);
	glutSolidCone(0.01,0.03,100,10);	
	glPopMatrix();

	glRasterPos3f(0.62f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
	glRasterPos3f(0.0f, 0.62f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
	glRasterPos3f(0.0f, 0.0f, 0.62f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
}

void drawGrid(float size = 1, float step = 0.2){
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
    for(float i=step; i <= size; i+= step)
    {
        glVertex3f(-size, 0,  i);   // lines parallel to X-axis
        glVertex3f( size, 0,  i);
        glVertex3f(-size, 0, -i);   // lines parallel to X-axis
        glVertex3f( size, 0, -i);

        glVertex3f( i, 0, -size);   // lines parallel to Z-axis
        glVertex3f( i, 0,  size);
        glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
        glVertex3f(-i, 0,  size);
    }
    glVertex3f(-size, 0, 0);
    glVertex3f( size, 0, 0);
    glVertex3f(0, 0, -size);
    glVertex3f(0, 0,  size);
    glEnd();
    // enable lighting back
    glEnable(GL_LIGHTING);
}

void drawFrustum(float size = 0.6){
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(-size,-size,0);
    glVertex3f(-size,size,0);
    glVertex3f(size,size,0);
    glVertex3f(size,-size,0);
    glEnd();
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

void renderCube(float size = 0.4){
    glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
    GLfloat alpha = .9f;

    glBegin(GL_TRIANGLES);
    glColor4f ( .8f,    0.4f,   0.4f, alpha);
    glVertex3f( size,   0.f,   0.f);
    glVertex3f( size,   size,  0.f);
    glVertex3f( size,   size,  size);
    glVertex3f( size,   0.f,   0.f);
    glVertex3f( size,   size,  size);
    glVertex3f( size,   0.f,   size);
    
    glColor4f ( .4f,    0.2f,   0.2f, alpha);
    glVertex3f( 0.f,    size,  size);
    glVertex3f( 0.f,    size,  0.f);
    glVertex3f( 0.f,    0.f,   0.f);
    glVertex3f( 0.f,    0.f,   size);
    glVertex3f( 0.f,    size,  size);
    glVertex3f( 0.f,    0.f,   0.f);
    
    glColor4f ( 0.4f,   0.8f,  0.4f, alpha);
    glVertex3f( size,   size,  size);
	glVertex3f( size,   size,  0.f);
	glVertex3f( 0.f,    size,  0.f);
    glVertex3f( 0.f,    size,  size);
    glVertex3f( size,   size,  size);
    glVertex3f( 0.f,    size,  0.f);

    glColor4f ( 0.2f,   0.4f,  0.2f, alpha);
    glVertex3f( 0.f,    0.f,  0.f);
	glVertex3f( size,   0.f,  0.f);
	glVertex3f( size,   0.f,  size);
    glVertex3f( 0.f,    0.f,  0.f);
    glVertex3f( size,   0.f,  size);
	glVertex3f( 0.f,    0.f,  size);

    glColor4f ( .4f,    .4f,   .8f, alpha);
    glVertex3f( size,   size,  size);
    glVertex3f( 0.f,    size,  size);
    glVertex3f( 0.f,    0.f,   size);
    glVertex3f( size,   0.f,   0.4f);
    glVertex3f( size,   size,  size);
    glVertex3f( 0.f,    0.f,   0.4f);

    glColor4f ( 0.2f,   0.2f,  .4f, alpha);
    glVertex3f( 0.f,    0.f,   0.f);
    glVertex3f( 0.f,    size,  0.f);
    glVertex3f( size,   size,  0.f);
    glVertex3f( size,   size,  0.f);
    glVertex3f( size,   0.f,   0.f);
    glVertex3f( 0.f,    0.f,   0.f);
    glEnd();
}

void show_matrix(glm::mat4 m){
    for (int i = 0;i<4;i++){
        for(int j = 0;j<4;j++){
            std::cout<<m[j][i]<<" ";
        }
        std::cout<<std::endl;
    }
}

void display(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_mv = g_view * g_model;
	glLoadMatrixf(glm::value_ptr(g_mv));
    // std::cout<<"m"<<std::endl;
    // show_matrix(g_model);
    // std::cout<<"v"<<std::endl;
    // show_matrix(g_view);
    // std::cout<<"mv"<<std::endl;
    // show_matrix(g_mv);
    if (g_bShowCube)
        renderCube();
    if (g_bShowAxis)
        drawAxis();
    if (g_bShowGrid)
        drawGrid();
    if (g_bShowFrustum)
        drawFrustum();
    glutSwapBuffers();
}

void processSpecialKey(GLint key, GLint x, GLint y) {
    if (key == GLUT_KEY_LEFT){
        model_pos[0] -= 0.05f;
    }
    if (key == GLUT_KEY_RIGHT){
        model_pos[0] += 0.05f;
    }
    if (key == GLUT_KEY_DOWN){
        model_pos[1] -= 0.05f;
    }
    if (key == GLUT_KEY_UP){
        model_pos[1] += 0.05f;
    }
    glutPostRedisplay();
}

void processNormalKeys(unsigned char key,int x,int y){
    switch (key){
        case 'a':
        case 'A':
            g_bShowAxis = !g_bShowAxis;
            break;
        case 'f':
        case 'F':
            g_bShowFrustum = !g_bShowFrustum;
            break;
        case 'g':
        case 'G':
            g_bShowGrid = !g_bShowGrid;
            break;
        case 'r':
        case 'R':
            g_view = glm::lookAt(glm::vec3(cam_pos[0], cam_pos[1], cam_pos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            g_model = glm::mat4(1.0);
            break;
        case 's':
        case 'S':
            g_bShowCube= !g_bShowCube;
            break;
        case '[':
            model_pos[2] -= 0.1f;
            break;
        case ']':
            model_pos[2] += 0.1f;
            break;
    }
    glutPostRedisplay();
}

glm::vec3 computePointOnSphere(double x, double y){
	glm::vec3 coord = glm::vec3(2.0*x / g_screen_width - 1.0,
        2.0*y / g_screen_height - 1.0, 0);
    coord.y = -coord.y;
    float length_squared = coord.x * coord.x + coord.y * coord.y;
    if (length_squared <= 1.0)
        coord.z = sqrt(1.0 - length_squared);
    else
        coord = glm::normalize(coord);
    return coord;
}

void onMouse(GLint button, GLint state, GLint x, GLint y){
	switch (button){
		case GLUT_LEFT_BUTTON:
			switch (state){
				case GLUT_DOWN:
					g_lastX = x;
					g_lastY = y;
					g_bLbutton = true;
					break;
				case GLUT_UP:
					g_lastX = x;
					g_lastY = y;
					g_bLbutton = false;
					break;
			}
            break;
		default:
			break;
	}
}

void onMotion(GLint x, GLint y){
	if (g_bLbutton){        
        glm::vec3 prevPos = computePointOnSphere(g_lastX, g_lastY);
        glm::vec3 currPos = computePointOnSphere(x, y);
        float d = glm::dot(prevPos, currPos);
        if (d < 1 - 0.00001){
            glm::vec3 axis = glm::cross(prevPos, currPos);
            float angle = std::acos(std::min(1.0f, d)) * 3;
            axis = glm::inverse(glm::mat3(g_mv)) * axis;
            g_model = glm::rotate(g_model, angle, axis);
            glutPostRedisplay();          
        }
    }
    g_lastX = x;
	g_lastY = y;
}

void initGL(){
	#if _WIN32
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	#endif
    glutInitWindowSize(g_screen_width, g_screen_height);
    glutCreateWindow("glutcube");
    #if _WIN32
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        }
    #endif
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    g_view = glm::lookAt(glm::vec3(cam_pos[0], cam_pos[1], cam_pos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    g_model = glm::mat4(1.0);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKey);
    glutKeyboardFunc(processNormalKeys);
    glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    initGL();
    glutMainLoop();
    return 0;
}