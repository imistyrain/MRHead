#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

void init()
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClearStencil(0);
    glClearDepth(0);
    glEnable(GL_STENCIL_TEST);
}

void reshape(int w, int h)
{
    if (h == 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35.f, (w * 1.f) / h, 1.f, 100.f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawRect()
{
    glColor3f(1.f, 0.f, 0.f);
    glRectf(-5.f, -5.f, 5.f, 5.f);
}

void drawSpin()
{
    glColor3f(0.f, 1.f, 0.f);
    float dRadius = 5.f * (sqrt(2.0)/2.0);
    glBegin(GL_LINE_STRIP);
    for (float dAngel = 0; dAngel < 10.f; dAngel += 0.1) {
        glVertex2d(dRadius * cos(dAngel), dRadius * sin(dAngel));
        dRadius *= 1.003;
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -20.f);

/*
    永远无法通过模板测试，drawSpin()绘制的内容的像素点颜色不会替换到颜色缓冲区中。
    
    因为无法通过模板测试，所以glStencilOp()中的第一个参数起作用，把模板缓冲区相对应的值+1。所以drawSpin()后，形成一个背景为0，中间螺旋线是1的模板缓冲区。
*/
    glStencilFunc(GL_NEVER, 0x0, 0x0);
    glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
    drawSpin();
    
/*
    当 stencil buffer 像素值的最后 1 位( mask 参数是 0x1) ，与 0x1 这个 ref 值作比较， 如果 not equal 不相等时，才会绘制下面的内容.

所以 drawRect() 在绘制的时候，有些部分是会绘制的，因为这部分的 stencil buffer 像素的值是 0 ; 而 stencil buffer 像素值是 1 的部分,也就是上一步，绘制螺旋线圈的部分，因为 stencil buffer 里对应的像素的 值是 1 ，所以这部分不会绘制。也就是会出现 drawRect()绘制的图像有镂空的情形（改变背景颜色就更清楚了）。

因为这一步的绘制不想修改模板缓存的值，所以无论通过与否，都希望模板缓存的值保持不变，因此 glStencilOp() 的 3个参数都是 GL_KEEP
*/
    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    drawRect();
    
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(600, 600);
    glutCreateWindow("stencil buffer test");
    
    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMainLoop();
    
    return 0;
}