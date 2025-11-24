#include <GL/glut.h>
#include <cmath>
#include <string>
#include <algorithm>
#include <Windows.h>
#include "Texture.h"
#include "Camera.h"
#include "Maze.h"
#include "SpiritManager.h"

// -------------------------------
// 전역 객체
// -------------------------------
Camera* g_camera = nullptr;
Maze* g_maze = nullptr;
SpiritManager* g_spirits = nullptr;
GLuint g_wallTex = 0;
GLuint g_floorTex = 0;
// 카메라 모드 (Camera.cpp에서 extern으로 사용)
CamMode g_camMode = PLAYER_MODE;

// 게임 상태
enum GameState { MENU, PLAYING };
GameState g_state = MENU;

// 시간
int   g_lastMs = 0;
float g_delta = 0.0f;

// 스프린트 관련
bool  wPressed = false;
float baseSpeed = 0.12f;
float sprintSpeed = 0.28f;

// 마우스 중앙 고정용
int winW = 1280;
int winH = 720;

// -----------------------------
// F1: 디버그 카메라 토글
// -----------------------------
void specialDown(int key, int, int)
{
    if (key == GLUT_KEY_F1)
    {
        // 모드 전환
        g_camMode = (g_camMode == PLAYER_MODE ? DEBUG_MODE : PLAYER_MODE);

        // Debug Mode에 들어간 순간 카메라 위치/시야 강제 설정
        if (g_camMode == DEBUG_MODE)
        {
            // 🔥 Maze 전체를 내려다보는 오버뷰 위치
            g_camera->x = 0.0f;
            g_camera->y = 10.0f;
            g_camera->z = 0.0f;

            // 🔥 아래(-y)를 바라보도록 pitch 설정
            g_camera->pitch = -89.0f;

            // 🔥 yaw는 아무 방향이나 OK (0도면 -Z 방향 바라봄)
            g_camera->yaw = 0.0f;
        }
    }
}

// -----------------------------
// 간단 텍스트 출력
// -----------------------------
void drawText(float x, float y, const std::string& s)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// -----------------------------
// 메뉴 화면
// -----------------------------
void drawMenu()
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    std::string msg = "CLICK TO START";
    glColor3f(1, 1, 1);
    int textW = (int)msg.length() * 10;

    glRasterPos2i(w / 2 - textW / 2, h / 2);
    for (char c : msg)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
}

// -----------------------------
// Display
// -----------------------------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---------------------------
    // ⭐ 1) Projection 설정 추가
    // ---------------------------
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        70.0f,                 // FOV (시야각)
        (float)winW / winH,    // 종횡비
        0.1f,                  // Near plane
        200.0f                 // Far plane (Maze 전체 보이도록 크게 설정)
    );

    // 이제 모델뷰로 변경
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ---------------------------
    // MENU 처리
    // ---------------------------
    if (g_state == MENU) {
        drawMenu();
        glutSwapBuffers();
        return;
    }

    // ---------------------------
    // ⭐ 2) FPS 카메라 적용
    // ---------------------------
    g_camera->apply();

    // Maze + Spirits
    g_maze->draw();
    g_spirits->drawSpirits();

    // ---------------------------
    // UI overlay (2D 모드)
    // ---------------------------

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1, 1, 1);

    drawText(10, winH - 20, "Time: " + std::to_string((int)g_camera->playTime) + "s");
    drawText(10, winH - 50,
        "R: " + std::to_string(g_spirits->Rcount) +
        "  G: " + std::to_string(g_spirits->Gcount) +
        "  B: " + std::to_string(g_spirits->Bcount)
    );

    drawText(winW - 200, 20, "WASD: Move");
    drawText(winW - 200, 40, "Mouse: Look");
    drawText(winW - 200, 60, "Space: Jump");
    drawText(winW - 200, 80, "F1: Debug Cam");

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

// -----------------------------
// 마우스 클릭
// -----------------------------
void mouseButton(int btn, int state, int x, int y)
{
    // 메뉴 상태에서 클릭하면 게임 시작
    if (g_state == MENU && btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        g_state = PLAYING;

        // 커서 숨기기
        glutSetCursor(GLUT_CURSOR_NONE);

        // 마우스 중앙으로 이동
        int cx = winW / 2;
        int cy = winH / 2;
        glutWarpPointer(cx, cy);
        g_camera->lastX = (float)cx;
        g_camera->lastY = (float)cy;

        return;
    }

    // 플레이 중일 때만 카메라 마우스 버튼 처리
    if (g_state == PLAYING)
        g_camera->mouseButton(btn, state, x, y);
}

// -----------------------------
// 마우스 이동
// -----------------------------
void mouseMotion(int x, int y)
{
    if (g_state != PLAYING) return;

    int cx = winW / 2;
    int cy = winH / 2;

    // warpPointer로 인해 들어온 이벤트면 무시
    if (x == cx && y == cy)
        return;

    // 상대 이동량 계산 (절대좌표 X)
    int dx = x - cx;
    int dy = cy - y;

    g_camera->handleMouseLook(dx, dy);

    // 다시 중앙으로
    glutWarpPointer(cx, cy);
}

// -----------------------------
// 키보드
// -----------------------------
void keyDown(unsigned char key, int, int)
{
    if (g_state != PLAYING) return;

    if (key == 'w') wPressed = true;
    if (key == 's') g_camera->moveBackward(baseSpeed);
    if (key == 'a') g_camera->moveLeft(baseSpeed);
    if (key == 'd') g_camera->moveRight(baseSpeed);

    if (key == ' ')
        g_camera->jump();

    // ESC → 메뉴로 돌아가기
    if (key == 27) {
        g_state = MENU;
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }
}

void keyUp(unsigned char key, int, int)
{
    if (key == 'w') wPressed = false;
}

// -----------------------------
// Idle / Update
// -----------------------------
void update()
{
    int now = glutGet(GLUT_ELAPSED_TIME);
    g_delta = (now - g_lastMs) / 1000.0f;
    g_lastMs = now;

    if (g_state == PLAYING) {
        g_camera->playTime += g_delta;

        // W를 누르고 있으면 스프린트 처리 + 앞으로 이동
        if (wPressed) {
            float speed = g_camera->updateSprint(g_delta, baseSpeed, sprintSpeed);
            g_camera->sprinting = true;
            g_camera->moveForward(speed);
        }
        else {
            // W를 떼면 sprinting false, 스태미나 회복만 진행
            g_camera->sprinting = false;
            g_camera->updateSprint(g_delta, baseSpeed, sprintSpeed);
        }
    }

    glutPostRedisplay();
}

// -----------------------------
// GL 초기화
// -----------------------------
void initGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Texture 객체 생성
    Texture wallTexObj;
    Texture floorTexObj;

    // 파일 불러오기 (png/jpg 가능)
    wallTexObj.loadFromFile("Debug/assets/wall_texture.png");
    floorTexObj.loadFromFile("Debug/assets/floor_texture.png");

    // 전역 변수에 ID 저장
    g_wallTex = wallTexObj.getID();
    g_floorTex = floorTexObj.getID();

    // 기존 Init들
    g_camera = new Camera();
    g_maze = new Maze();
    g_spirits = new SpiritManager();

    g_maze->init();
    g_camera->maze = g_maze;
    g_spirits->maze = g_maze;
    g_spirits->initSpirits();

    g_camera->setPosition(-13.5f, 1.5f, -13.5f);
}

// -----------------------------
// main
// -----------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(winW, winH);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Spirit Maze FPS");

    initGL();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc(update);
    glutSpecialFunc(specialDown);

    g_lastMs = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();
    return 0;
}
