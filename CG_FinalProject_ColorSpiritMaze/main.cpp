#include <GL/glut.h>
#include "Camera.h"
#include "Maze.h"
#include "SOR.h"
#include "SpiritManager.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 전역 포인터 객체 (GLUT 콜백 때문에 필요)
Camera* g_camera;
Maze* g_maze;
SOR* g_sor;
SpiritManager* g_spirit;

// 게임 상태
enum GameState { Playing, Ended };
GameState g_state = Playing;

int g_countR = 0, g_countG = 0, g_countB = 0;
float g_timeRemaining = 120.0f; // seconds

// 이동 속도 및 버프
float g_baseSpeed = 0.2f;
float g_moveSpeed = g_baseSpeed;
float g_speedBuffTime = 0.0f; // 남은 버프 시간(초)

// 시간 트래킹
int g_lastTimeMs = 0;

// ---- Wrapper 함수들 ----
void mouseButtonWrapper(int btn, int state, int x, int y) {
    g_camera->mouseButton(btn, state, x, y);
}
void mouseMotionWrapper(int x, int y) {
    g_camera->mouseMotion(x, y);
}

// ---- Overlay UI 렌더 ----
void drawText(const std::string& s) {
    for (char c : s) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawOverlay() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    // 프로젝션을 오르소로 변경
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);

    std::ostringstream ss;
    ss << "R: " << g_countR << "  G: " << g_countG << "  B: " << g_countB;
    std::string rgb = ss.str();

    ss.str(""); ss.clear();
    ss << "Time: " << static_cast<int>(std::ceil(g_timeRemaining)) << "s";
    std::string timeStr = ss.str();

    ss.str(""); ss.clear();
    ss << "SpeedBuff: " << std::fixed << std::setprecision(1) << g_speedBuffTime << "s";
    std::string buffStr = ss.str();

    // 좌상단 표시 (y는 아래에서 위로)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, h - 20);
    drawText(rgb);

    glRasterPos2i(10, h - 40);
    drawText(timeStr);

    glRasterPos2i(10, h - 60);
    drawText(buffStr);

    if (g_state == Ended) {
        std::string end = "GAME OVER - Press ESC to exit";
        int len = (int)end.size();
        // 화면 중앙
        glColor3f(1, 1, 0);
        glRasterPos2i(w/2 - 8 * len, h/2);
        drawText(end);
    }

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
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
    // g_sor->draw() 내부에서 인스턴스별 색상을 설정하므로 여기서는 고정 색 설정하지 않음
    g_sor->draw();
    glPopMatrix();

    // UI 오버레이는 마지막에 그림
    drawOverlay();
    g_sor->draw();
    glPopMatrix();

    g_spirit->drawSpirits();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
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
