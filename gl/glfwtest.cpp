#ifdef USE_GLAD
    #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

void drawGrid(float size = 1, float step = 0.2) {
	glBegin(GL_LINES);
	glColor3f(0.3f, 0.3f, 0.3f);
    glLoadIdentity();
    glTranslated(0, 0, -1);
	for (float i = step; i <= size; i += step)
	{
        glVertex3f(-size, i, 0);
        glVertex3f(size, i, 0);
        glVertex3f(-size, -i, 0);
        glVertex3f(size, -i, 0);
        glVertex3f(i, -size, 0);
        glVertex3f(i, size, 0);
        glVertex3f(-i, -size, 0);
        glVertex3f(-i, size, 0);
	}
    glVertex3f(-size, 0, 0);
	glVertex3f(size, 0, 0);
	glVertex3f(0, -size, 0);
	glVertex3f(0, size, 0);
	glEnd();
}

void drawTriangle(float angle = 0.0f) {
    glTranslated(0, 0, -1);
    glRotatef(angle, 0,0,1);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0, -0.5);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-0.5, 0.5);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.5, 0.5);
    glEnd();
}

int main(int argc, char *argv[]){
    glfwInit();
#if USE_GLAD
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
#if USE_GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    float angle = 0;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        drawGrid();
        drawTriangle(angle);
        angle -= 0.3f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}