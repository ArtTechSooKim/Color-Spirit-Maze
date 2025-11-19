#include <GL/glut.h>
#include "Camera.h"
#include "Maze.h"
#include "SpiritManager.h"

Camera* g_camera;
Maze* g_maze;
SpiritManager* g_spirit;

// 마우스 입력 래퍼
void mouseButtonWrapper(int btn, int state, int x, int y) {
    if (g_camera) g_camera->mouseButton(btn, state, x, y);
}
void mouseMotionWrapper(int x, int y) {
    if (g_camera) g_camera->mouseMotion(x, y);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_camera->apply();

    // 미로
    g_maze->draw();

    // 정령
    g_spirit->drawSpirits();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') g_camera->moveForward(0.2f);
    if (key == 's') g_camera->moveBackward(0.2f);
    if (key == 'a') g_camera->moveLeft(0.2f);
    if (key == 'd') g_camera->moveRight(0.2f);
    if (key == 27)  exit(0);  // ESC

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    g_camera = new Camera();
    g_maze = new Maze();
    g_spirit = new SpiritManager();

    g_spirit->maze = g_maze;
    g_spirit->initSpirits();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Color Spirit Maze - New Spirit System");

    glEnable(GL_DEPTH_TEST);

    // 배경색
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButtonWrapper);
    glutMotionFunc(mouseMotionWrapper);

    glutIdleFunc([]() {
        // 카메라 위치를 중심으로 충돌 체크 (y는 안 씀)
        g_spirit->updateSpiritCollision(g_camera->x, g_camera->y, g_camera->z);
        glutPostRedisplay();
        });

    glutMainLoop();
    return 0;
}
