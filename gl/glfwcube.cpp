#define _USE_MATH_DEFINES
#include "cmath"
#include "algorithm"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "opencv2/opencv.hpp"

int g_screen_width = 400;
int g_screen_height = 400;
Shader *shader;
float cam_pos[3] = {0, 0, 1.6f};
GLint g_lastX = 0;
GLint g_lastY = 0;
bool g_bLbutton = false;
bool g_bShowCube = true;
bool g_bShowAxis = true;
bool g_bShowFrustum = true;
bool g_bShowGrid = true;
bool g_bctrl = false;
float g_scale = 0.4;
float g_fov = 60;
float g_near = 0.1;
float g_far = 100;
glm::mat4 g_model;
glm::mat4 g_view;
glm::mat4 g_mv;
glm::mat4 g_proj;

void reshape(GLFWwindow* window, int w, int h) {
	g_screen_width = w;
	g_screen_height = h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);
	g_proj = glm::perspective(glm::radians(g_fov), g_screen_width * 1.0f / g_screen_height, g_near, g_far);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(g_proj));
	glMatrixMode(GL_MODELVIEW);
}

void drawAxis(float scale = 0.6) {
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
	for (int i = 0; i < 10 * (scale / 2); i++) {
		float axis = i * 1.0 / 10;
		glVertex3f(axis, 0, 0);
		glVertex3f(0, axis, 0);
		glVertex3f(0, 0, axis);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.58, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.58, 0);
	glRotatef(270, 1, 0, 0);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.58);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glRasterPos3f(0.62f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
	glRasterPos3f(0.0f, 0.62f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
	glRasterPos3f(0.0f, 0.0f, 0.62f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
}

void drawGrid(float size = 1, float step = 0.2) {
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(0.3f, 0.3f, 0.3f);
	for (float i = step; i <= size; i += step)
	{
		glVertex3f(-size, 0, i);   // lines parallel to X-axis
		glVertex3f(size, 0, i);
		glVertex3f(-size, 0, -i);   // lines parallel to X-axis
		glVertex3f(size, 0, -i);

		glVertex3f(i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(i, 0, size);
		glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(-i, 0, size);
	}
	glVertex3f(-size, 0, 0);
	glVertex3f(size, 0, 0);
	glVertex3f(0, 0, -size);
	glVertex3f(0, 0, size);
	glEnd();
	// enable lighting back
	glEnable(GL_LIGHTING);
}

void drawFrustum(float size = 0.6) {
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
	glBegin(GL_QUADS);
	glVertex3f(-size, -size, 0);
	glVertex3f(-size, size, 0);
	glVertex3f(size, size, 0);
	glVertex3f(size, -size, 0);
	glEnd();
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

void renderCube(float size = 0.4) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	GLfloat alpha = .9f;

	glBegin(GL_TRIANGLES);
	glColor4f(.8f, 0.4f, 0.4f, alpha);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(size, size, 0.f);
	glVertex3f(size, size, size);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(size, size, size);
	glVertex3f(size, 0.f, size);

	glColor4f(.4f, 0.2f, 0.2f, alpha);
	glVertex3f(0.f, size, size);
	glVertex3f(0.f, size, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, size);
	glVertex3f(0.f, size, size);
	glVertex3f(0.f, 0.f, 0.f);

	glColor4f(0.4f, 0.8f, 0.4f, alpha);
	glVertex3f(size, size, size);
	glVertex3f(size, size, 0.f);
	glVertex3f(0.f, size, 0.f);
	glVertex3f(0.f, size, size);
	glVertex3f(size, size, size);
	glVertex3f(0.f, size, 0.f);

	glColor4f(0.2f, 0.4f, 0.2f, alpha);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(size, 0.f, size);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(size, 0.f, size);
	glVertex3f(0.f, 0.f, size);

	glColor4f(.4f, .4f, .8f, alpha);
	glVertex3f(size, size, size);
	glVertex3f(0.f, size, size);
	glVertex3f(0.f, 0.f, size);
	glVertex3f(size, 0.f, size);
	glVertex3f(size, size, size);
	glVertex3f(0.f, 0.f, size);

	glColor4f(0.2f, 0.2f, .4f, alpha);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, size, 0.f);
	glVertex3f(size, size, 0.f);
	glVertex3f(size, size, 0.f);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glEnd();
}

glm::vec3 computePointOnSphere(double x, double y) {
	glm::vec3 coord = glm::vec3(1.0*x / g_screen_width * 2 - 1.0,
		1.0*y / g_screen_height * 2 - 1.0, 0);
	coord.y = -coord.y;
	float length_squared = coord.x * coord.x + coord.y * coord.y;
	if (length_squared <= 1.0)
		coord.z = sqrt(1.0 - length_squared);
	else
		coord = glm::normalize(coord);
	return coord;
}

void scroll_callback(GLFWwindow *win, double xoffset, double yoffset) {
	g_view = glm::translate(g_view, glm::vec3(0, 0, yoffset * 0.01));
}

void mouse_button_callback(GLFWwindow *win, int button, int action, int mods) {
	if (button == 0) {
		if (action == 1) {
			g_bLbutton = true;
		}
		else {
			g_bLbutton = false;
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	if (g_bLbutton) {
		if (g_bctrl) {
			g_model = glm::translate(g_model, glm::vec3(0.01*(x - g_lastX), -0.01*(y - g_lastY), 0));
		}
		else {
			glm::vec3 prevPos = computePointOnSphere(g_lastX, g_lastY);
			glm::vec3 currPos = computePointOnSphere(x, y);
			glm::vec3 axis = glm::cross(prevPos, currPos);
			float angle = std::acos(std::min(1.0f, glm::dot(prevPos, currPos))) * 2;
			axis = glm::inverse(glm::mat3(g_mv)) * axis;
			g_model = glm::rotate(g_model, angle, axis);
		}
	}
	g_lastX = x;
	g_lastY = y;
}

void saveScreen(){
	int vPort[4];
    glGetIntegerv(GL_VIEWPORT, vPort);
	cv::Mat bgra(vPort[3], vPort[2], CV_8UC3);
    glReadPixels(0, 0, vPort[2], vPort[3], GL_BGR, GL_UNSIGNED_BYTE, bgra.data);
	cv::flip(bgra,bgra,0);
	cv::imwrite("trackball.png",bgra);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
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
		case 'p':
		case 'P':
			saveScreen();
			break;
		case 'r':
		case 'R':
			//glLoadIdentity();
			//gluLookAt(cam_pos[0],cam_pos[1],cam_pos[2],0.f,0.f,0.f,0.f,1.f,0.f);
			g_scale = 0.4f;
			g_view = glm::lookAt(glm::vec3(cam_pos[0], cam_pos[1], cam_pos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			g_model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0));
			break;
		case 's':
		case 'S':
			g_bShowCube = !g_bShowCube;
			break;
		case '[':
			g_model = glm::translate(g_model, glm::vec3(0, 0, -0.05));
			break;
		case ']':
			g_model = glm::translate(g_model, glm::vec3(0, 0, 0.05));
			break;
		case '+':
		case '=':
			g_scale *= 1.05;
			break;
		case '-':
		case '_':
			g_scale /= 1.05;
			break;
		case GLFW_KEY_LEFT:
			g_model = glm::translate(g_model, glm::vec3(-0.05, 0, 0));
			break;
		case GLFW_KEY_RIGHT:
			g_model = glm::translate(g_model, glm::vec3(0.05, 0, 0));
			break;
		case GLFW_KEY_UP:
			g_model = glm::translate(g_model, glm::vec3(0, 0.05, 0));
			break;
		case GLFW_KEY_DOWN:
			g_model = glm::translate(g_model, glm::vec3(0, -0.05, 0));
			break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			g_bctrl = true;
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
	else {
		g_bctrl = false;
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	#if _WIN32
		glewInit();
	#endif
	if (!glfwInit()) {
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(g_screen_width, g_screen_height, "glfwcube", NULL, NULL);
	if (!window){
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	g_proj = glm::perspective(glm::radians(g_fov), g_screen_width * 1.0f / g_screen_height, g_near, g_far);
	glLoadMatrixf(glm::value_ptr(g_proj));
	glMatrixMode(GL_MODELVIEW);
	g_view = glm::lookAt(glm::vec3(cam_pos[0], cam_pos[1], cam_pos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	g_model = glm::mat4(1.0);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		g_mv = g_view * g_model;
		glLoadMatrixf(glm::value_ptr(g_mv));
		if (g_bShowCube)
			renderCube(g_scale);
		if (g_bShowAxis)
			drawAxis();
		if (g_bShowGrid)
			drawGrid();
		if (g_bShowFrustum)
			drawFrustum();
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}
	glfwTerminate();
	return 0;
}