#include <GL/glut.h>
#include "Camera.h"
#include "Maze.h"
#include "SOR.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 전역 포인터 객체 (GLUT 콜백 때문에 필요)
Camera* g_camera;
Maze* g_maze;
SOR* g_sor;

// ---- Wrapper 함수들 ----
void mouseButtonWrapper(int btn, int state, int x, int y) {
    g_camera->mouseButton(btn, state, x, y);
}
void mouseMotionWrapper(int x, int y) {
    g_camera->mouseMotion(x, y);
}

// ---- Display 함수 ----
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_camera->apply();

    g_maze->draw();

    glPushMatrix();
    glTranslatef(0, 1.0f, 0);
    glColor3f(1, 0.2f, 0.2f);
    g_sor->draw();
    glPopMatrix();

    glutSwapBuffers();
}

// ---- Reshape ----
void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
}

// ---- Keyboard ----
void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') g_camera->moveForward(0.2f);
    if (key == 's') g_camera->moveBackward(0.2f);
    if (key == 'a') g_camera->moveLeft(0.2f);
    if (key == 'd') g_camera->moveRight(0.2f);

    glutPostRedisplay();
}

// ---- main ----
int main(int argc, char** argv) {
    // 전역 객체 초기화
    g_camera = new Camera();
    g_maze = new Maze();
    g_sor = new SOR();

    g_sor->generateFakeSphere();

    // GLUT 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Color Spirit Maze");

    glEnable(GL_DEPTH_TEST);

    // 콜백 등록
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButtonWrapper);
    glutMotionFunc(mouseMotionWrapper);
    glutIdleFunc([]() { glutPostRedisplay(); });

    glutMainLoop();
    return 0;
}
