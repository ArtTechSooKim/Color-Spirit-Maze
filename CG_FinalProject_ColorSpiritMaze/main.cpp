#include <GL/glut.h>
#include "Camera.h"
#include "Maze.h"
#include "SOR.h"
#include "SpiritManager.h"

Camera* g_camera;
Maze* g_maze;
SOR* g_sor;
SpiritManager* g_spirit;

void mouseButtonWrapper(int btn, int state, int x, int y) {
    g_camera->mouseButton(btn, state, x, y);
}
void mouseMotionWrapper(int x, int y) {
    g_camera->mouseMotion(x, y);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_camera->apply();
    g_maze->draw();

    glPushMatrix();
    glTranslatef(0, 1.0f, 0);
    g_sor->draw();
    glPopMatrix();

    g_spirit->drawSpirits();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') g_camera->moveForward(0.2f);
    if (key == 's') g_camera->moveBackward(0.2f);
    if (key == 'a') g_camera->moveLeft(0.2f);
    if (key == 'd') g_camera->moveRight(0.2f);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    g_camera = new Camera();
    g_maze = new Maze();
    g_sor = new SOR();
    g_spirit = new SpiritManager();

    // ★ 반드시 있어야 한다! (핵심 버그 해결)
    g_spirit->maze = g_maze;

    // 3D 모델 한 번만 생성
    g_sor->generateTorchSpirit();
    g_spirit->sorModel.generateTorchSpirit();

    // 정령 초기화
    g_spirit->initSpirits();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Color Spirit Maze");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButtonWrapper);
    glutMotionFunc(mouseMotionWrapper);

    glutIdleFunc([]() {
        g_spirit->updateSpiritCollision(g_camera->x, g_camera->y, g_camera->z);
        glutPostRedisplay();
        });

    glutMainLoop();
    return 0;
}
