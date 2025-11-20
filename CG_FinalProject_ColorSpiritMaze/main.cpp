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
enum GameState { Menu, Playing, Ended };
GameState g_state = Menu;

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
    // 메뉴 상태에서는 Start 버튼 클릭만 처리하고 카메라 입력은 무시
    if (g_state == Menu && btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        // 버튼 크기/위치 (drawOverlay와 동일하게 계산)
        int btnW = 240;
        int btnH = 64;
        int btnX = w / 2 - btnW / 2;
        int btnY = h / 2 - btnH / 2;

        // GLUT 마우스 y는 윈도우 상단이 0이므로 변환
        int oy = h - y;
        int ox = x;

        if (ox >= btnX && ox <= btnX + btnW && oy >= btnY && oy <= btnY + btnH) {
            // Start 버튼 클릭: 게임 시작 초기화
            g_state = Playing;
            g_timeRemaining = 120.0f;
            g_moveSpeed = g_baseSpeed;
            g_speedBuffTime = 0.0f;
            g_countR = g_countG = g_countB = 0;
            // 스피릿 카운트/상태 초기화가 필요하면 SpiritManager에 초기화 함수 추가 후 호출
            g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
        }
        return;
    }

    // 메뉴가 아닐 때는 평상시 카메라 입력 전달
    if (g_state != Menu) {
        g_camera->mouseButton(btn, state, x, y);
    }
}
void mouseMotionWrapper(int x, int y) {
    if (g_state != Menu) {
        g_camera->mouseMotion(x, y);
    }
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

    // 버프 텍스트는 이제 활성 중일 때만 생성/표시
    std::string buffStr;
    if (g_speedBuffTime > 0.0f) {
        std::ostringstream sb;
        sb << "SpeedBuff: " << std::fixed << std::setprecision(1) << g_speedBuffTime << "s";
        buffStr = sb.str();
    }

    // 좌상단 표시 (y는 아래에서 위로)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, h - 20);
    drawText(rgb);

    glRasterPos2i(10, h - 40);
    drawText(timeStr);

    // SpeedBuff는 버프 활성 중일 때만 표시
    if (!buffStr.empty()) {
        glRasterPos2i(10, h - 60);
        drawText(buffStr);
    }

    // 메뉴 화면: 중앙에 Start 버튼 표시
    if (g_state == Menu) {
        int btnW = 240;
        int btnH = 64;
        int btnX = w / 2 - btnW / 2;
        int btnY = h / 2 - btnH / 2;

        // 버튼 배경
        glColor3f(0.1f, 0.1f, 0.35f);
        glBegin(GL_QUADS);
            glVertex2i(btnX, btnY);
            glVertex2i(btnX + btnW, btnY);
            glVertex2i(btnX + btnW, btnY + btnH);
            glVertex2i(btnX, btnY + btnH);
        glEnd();

        // 버튼 테두리
        glLineWidth(2.0f);
        glColor3f(0.8f, 0.8f, 1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2i(btnX, btnY);
            glVertex2i(btnX + btnW, btnY);
            glVertex2i(btnX + btnW, btnY + btnH);
            glVertex2i(btnX, btnY + btnH);
        glEnd();

        // 시작 버튼
        std::string label = "CLICK TO START";
        glColor3f(1.0f, 1.0f, 1.0f);
        int textW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)label.c_str());
        int textX = btnX + (btnW - textW) / 2;
        // 수평 중심은 btn 내부, 수직 중심은 버튼 중앙에 폰트의 기준점을 맞춰 약간 위로 조정
        int textY = btnY + (btnH / 2) + 6; // +6으로 baseline 보정
        glRasterPos2i(textX, textY);
        drawText(label);

        // 조작 안내
        std::string hint = "WASD move, drag mouse to look";
        int hintW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)hint.c_str());
        int hintX = btnX + (btnW - hintW) / 2;
        int hintY = btnY - 18; // 버튼 아래에 배치
        if (hintY < 10) hintY = 10;
        // 파란색
        glColor3f(0.2f, 0.6f, 1.0f);
        glRasterPos2i(hintX, hintY);
        drawText(hint);
        // 이후 다른 텍스트는 필요시 색 복원
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // 게임 종료 화면과 성적 표시
    if (g_state == Ended) {
        int totalCollected = g_countR + g_countG + g_countB;
        std::string grade;
        if (totalCollected >= 3) grade = "A";
        else if (totalCollected == 2) grade = "B";
        else if (totalCollected == 1) grade = "C";
        else grade = "F";

        std::string title = "GAME OVER";
        std::string totalStr;
        {
            std::ostringstream sst;
            sst << "Total collected: " << totalCollected << "  (R:" << g_countR << " G:" << g_countG << " B:" << g_countB << ")";
            totalStr = sst.str();
        }
        std::string gradeStr = std::string("Grade: ") + grade;

        // 중앙 정렬 계산
        int titleW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)title.c_str());
        int totalW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)totalStr.c_str());
        int gradeW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)gradeStr.c_str());

        int centerX = w / 2;
        int centerY = h / 2;

        // 타이틀 (노란)
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2i(centerX - titleW / 2, centerY + 20);
        drawText(title);

        // 합계 (흰색)
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(centerX - totalW / 2, centerY);
        drawText(totalStr);

        // 등급 (녹색 강조)
        glColor3f(0.2f, 1.0f, 0.2f);
        glRasterPos2i(centerX - gradeW / 2, centerY - 20);
        drawText(gradeStr);

        // 안내: 종료 후 ESC로 종료
        std::string hint = "Press ESC to exit";
        int hintW = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)hint.c_str());
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(centerX - hintW / 2, centerY - 44);
        drawText(hint);
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

    // SOR는 Y축으로 올려서 한 번만 그리도록 정리
    glPushMatrix();
    glTranslatef(0, 1.0f, 0);
    g_sor->draw();
    glPopMatrix();

    // 정령들 그리기 (게임 중에만 충돌/수집이 활성화되지만 렌더는 항상 가능)
    g_spirit->drawSpirits();

    // UI 오버레이는 마지막에 그림
    drawOverlay();

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
    if (key == 27) { // ESC
        exit(0);
    }
    // 게임이 시작된 경우에만 이동 허용
    if (g_state != Playing) return;

    if (key == 'w') g_camera->moveForward(g_moveSpeed);
    if (key == 's') g_camera->moveBackward(g_moveSpeed);
    if (key == 'a') g_camera->moveLeft(g_moveSpeed);
    if (key == 'd') g_camera->moveRight(g_moveSpeed);
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

    // 초기 시간 설정
    g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);

    glutIdleFunc([]() {
        // 메뉴나 엔딩 상태에서는 게임 로직 업데이트를 건너뜀(화면은 계속 갱신)
        if (g_state != Playing) {
            glutPostRedisplay();
            return;
        }

        // 시간 계산 (초)
        int curr = glutGet(GLUT_ELAPSED_TIME);
        float delta = (curr - g_lastTimeMs) / 1000.0f;
        g_lastTimeMs = curr;

        // 정령 충돌 검사(녹색 수집 시 true 반환)
        bool gotSpeed = g_spirit->updateSpiritCollision(g_camera->x, g_camera->y, g_camera->z);

        // 전역 카운트 동기화
        g_countR = g_spirit->Rcount;
        g_countG = g_spirit->Gcount;
        g_countB = g_spirit->Bcount;

        // 타이머 감소
        g_timeRemaining -= delta;
        if (g_timeRemaining <= 0.0f) {
            g_timeRemaining = 0.0f;
            g_state = Ended;
        }

        // 속도 버프 처리: 녹색 정령을 먹으면 5초 버프
        if (gotSpeed) {
            g_speedBuffTime = 5.0f; // 5초
            g_moveSpeed = g_baseSpeed * 1.8f; // 버프 배수(원하면 조정)
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
