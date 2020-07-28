#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int SCR_WIDTH = 400;
unsigned int SCR_HEIGHT = 400;
float angle = 0;
Shader *shader;
float model_x = 0;
float model_y = 0;
float model_z = 1.0f;
bool g_busegl2 = false;

void reshape(int w,int h){
    SCR_WIDTH = w;
    SCR_HEIGHT = h;
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, w * 1.0f / h, 0.1, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void show_matrix(){
    float matMV[16];
    float matP[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matMV);
    glGetFloatv(GL_PROJECTION_MATRIX, matP);
    std::cout.precision(2);
    std::cout<<"\t\tMV\t\t\tP"<<std::endl;
    for (int i = 0;i < 4; i++){
        for (int j = 0;j < 4; j++){
            std::cout<<matMV[i+4*j]<<"\t";
        }
        std::cout<<"    ";
        for (int j = 0;j < 4; j++){
            std::cout<<matP[i+4*j]<<"\t";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void draw_gl1(){
    glPushMatrix();
    //gluLookAt(model_x,model_y,model_z,0,0,0,0,1,0);
    glTranslatef(-model_x,-model_y,-model_z);
    glRotatef(angle,0,0,1);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-0.5,-0.5,0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.5,-0.5,0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0,0.5,0);
    glEnd();

    show_matrix();

    glPopMatrix();
}

void draw_gl2(){
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glm::vec3 cameraPos   = glm::vec3(model_x,model_y,model_z);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "view"), 1, GL_FALSE, &view[0][0]);

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), SCR_WIDTH * 1.0f/SCR_WIDTH, 0.1f, 10.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    std::cout.precision(3);
    for (int i = 0;i < 4; i++){
        for (int j = 0; j < 4; j++){
            std::cout<<model[i][j]<<"\t";
        }
        std::cout<<"     ";
        for (int j = 0; j < 4; j++){
            std::cout<<view[i][j]<<"  ";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void display(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if (g_busegl2){
        draw_gl2();
    } else{
        draw_gl1();
    }
    angle += 1;
    glutSwapBuffers();
}

void on_timer(int val){
	glutPostRedisplay();
	glutTimerFunc(30,on_timer,0);
}

void processSpecialKey(GLint key, GLint x, GLint y) {
    if (key == GLUT_KEY_LEFT){
        model_x += 0.05f;
    }
    if (key == GLUT_KEY_RIGHT){
        model_x -= 0.05f;
    }
    if (key == GLUT_KEY_DOWN){
        model_y += 0.05f;
    }
    if (key == GLUT_KEY_UP){
        model_y -= 0.05f;
    }
}

void processNormalKeys(unsigned char key,int x,int y){
    switch (key){
        case 's':
        case 'S':
            g_busegl2 = !g_busegl2;
            break;
        case '[':
            model_z -= 0.05f;
            break;
        case ']':
            model_z += 0.05f;
            break;
    }
}

void initGL(){
	#if _WIN32
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	#endif
    glutInitWindowSize(SCR_WIDTH,SCR_HEIGHT);
    glutCreateWindow("gltest");
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
    }
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKey);
    glutKeyboardFunc(processNormalKeys);
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    shader = new Shader("shaders/basic.vs","shaders/basic.fs");
    unsigned int VBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glutTimerFunc(30,on_timer,0);
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    initGL();
    glutMainLoop();
    return 0;
}