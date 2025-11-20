#include <GL/glut.h>
#include "Camera.h"
#include "Maze.h"
#include "SpiritManager.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -------- 전역 객체 --------
Camera* g_camera;
Maze* g_maze;
SpiritManager* g_spirit;

// 게임 상태
enum GameState { Menu, Playing, Ended };
GameState g_state = Menu;

// RGB 카운트
int g_countR = 0;
int g_countG = 0;
int g_countB = 0;

// 이동속도
float g_baseSpeed = 0.2f;
float g_moveSpeed = g_baseSpeed;

// 버프
float g_speedBuffTime = 0.0f;

// 타이머
float g_timeRemaining = 120.0f;
int g_lastTimeMs = 0;


// -------- 마우스 Wrapper --------
void mouseButtonWrapper(int btn, int state, int x, int y) {
    if (g_state == Menu && btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        int btnW = 240;
        int btnH = 64;
        int btnX = w / 2 - btnW / 2;
        int btnY = h / 2 - btnH / 2;

        int oy = h - y;
        int ox = x;

        if (ox >= btnX && ox <= btnX + btnW && oy >= btnY && oy <= btnY + btnH) {
            g_state = Playing;
            g_timeRemaining = 120.0f;
            g_moveSpeed = g_baseSpeed;
            g_speedBuffTime = 0.0f;
            g_countR = g_countG = g_countB = 0;

            g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
        }
        return;
    }

    if (g_state != Menu) g_camera->mouseButton(btn, state, x, y);
}

void mouseMotionWrapper(int x, int y) {
    if (g_state != Menu) g_camera->mouseMotion(x, y);
}


// -------- Overlay Text --------
void drawText(const std::string& s) {
    for (char c : s) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}


// -------- Overlay Rendering --------
void drawOverlay() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    // RGB 카운트
    std::ostringstream ss;
    ss << "R: " << g_countR << "  G: " << g_countG << "  B: " << g_countB;
    glColor3f(1, 1, 1);
    glRasterPos2i(10, h - 20);
    drawText(ss.str());

    // 타이머
    ss.str(""); ss.clear();
    ss << "Time: " << (int)std::ceil(g_timeRemaining) << "s";
    glRasterPos2i(10, h - 40);
    drawText(ss.str());

    // 버프가 있을 때만 표시
    if (g_speedBuffTime > 0.0f) {
        ss.str(""); ss.clear();
        ss << "Speed Buff: " << std::fixed << std::setprecision(1) << g_speedBuffTime << "s";
        glRasterPos2i(10, h - 60);
        drawText(ss.str());
    }

    // ----- MENU START 버튼 -----
    if (g_state == Menu) {
        int btnW = 240;
        int btnH = 64;
        int btnX = w / 2 - btnW / 2;
        int btnY = h / 2 - btnH / 2;

        glColor3f(0.1f, 0.1f, 0.35f);
        glBegin(GL_QUADS);
        glVertex2i(btnX, btnY);
        glVertex2i(btnX + btnW, btnY);
        glVertex2i(btnX + btnW, btnY + btnH);
        glVertex2i(btnX, btnY + btnH);
        glEnd();

        glColor3f(1, 1, 1);
        std::string label = "CLICK TO START";
        int textW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label.c_str());
        glRasterPos2i(btnX + (btnW - textW) / 2, btnY + btnH / 2 + 6);
        drawText(label);
    }

    // ----- GAME OVER 화면 -----
    if (g_state == Ended) {
        int total = g_countR + g_countG + g_countB;
        std::string grade = (total >= 3) ? "A" : (total == 2 ? "B" : (total == 1 ? "C" : "F"));

        std::string title = "GAME OVER";
        std::string totalStr = "Total: " + std::to_string(total);
        std::string gradeStr = "Grade: " + grade;

        int cx = w / 2;
        int cy = h / 2;

        glColor3f(1, 1, 0);
        glRasterPos2i(cx - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)title.c_str()) / 2, cy + 20);
        drawText(title);

        glColor3f(1, 1, 1);
        glRasterPos2i(cx - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)totalStr.c_str()) / 2, cy);
        drawText(totalStr);

        glColor3f(0.2f, 1, 0.2f);
        glRasterPos2i(cx - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)gradeStr.c_str()) / 2, cy - 20);
        drawText(gradeStr);
    }

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}


// -------- Display --------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_camera->apply();

    g_maze->draw();
    g_spirit->drawSpirits();

    drawOverlay();

    glutSwapBuffers();
}


// -------- Reshape --------
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)w / h, 1.0, 500.0);

    glMatrixMode(GL_MODELVIEW);
}


// -------- Keyboard --------
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);

    if (g_state != Playing) return;

    if (key == 'w') g_camera->moveForward(g_moveSpeed);
    if (key == 's') g_camera->moveBackward(g_moveSpeed);
    if (key == 'a') g_camera->moveLeft(g_moveSpeed);
    if (key == 'd') g_camera->moveRight(g_moveSpeed);

    glutPostRedisplay();
}


// -------- main --------
int main(int argc, char** argv) {
    g_camera = new Camera();
    g_maze = new Maze();
    g_spirit = new SpiritManager();

    g_spirit->maze = g_maze;
    g_spirit->initSpirits();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Color Spirit Maze");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButtonWrapper);
    glutMotionFunc(mouseMotionWrapper);

    g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);

    glutIdleFunc([]() {
        if (g_state != Playing) {
            glutPostRedisplay();
            return;
        }

        int curr = glutGet(GLUT_ELAPSED_TIME);
        float delta = (curr - g_lastTimeMs) / 1000.0f;
        g_lastTimeMs = curr;

        // 충돌체크 (녹색이면 true)
        bool gotSpeed = g_spirit->updateSpiritCollision(g_camera->x, g_camera->y, g_camera->z);

        g_countR = g_spirit->Rcount;
        g_countG = g_spirit->Gcount;
        g_countB = g_spirit->Bcount;

        g_timeRemaining -= delta;
        if (g_timeRemaining <= 0.0f) {
            g_timeRemaining = 0.0f;
            g_state = Ended;
        }

        if (gotSpeed) {
            g_speedBuffTime = 5.0f;
            g_moveSpeed = g_baseSpeed * 1.8f;
        }

        if (g_speedBuffTime > 0.0f) {
            g_speedBuffTime -= delta;
            if (g_speedBuffTime <= 0.0f) {
                g_speedBuffTime = 0.0f;
                g_moveSpeed = g_baseSpeed;
            }
        }

        glutPostRedisplay();
        });

    glutMainLoop();
    return 0;
}
