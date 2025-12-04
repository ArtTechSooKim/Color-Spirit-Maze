#include <GL/glut.h>
#include <cmath>
#include <string>
#include <algorithm>
#include <Windows.h>
#include "Texture.h"
#include "Camera.h"
#include "Maze.h"
#include "SpiritManager.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
enum GameState { MENU, PLAYING, TUTORIAL };
GameState g_state = MENU;

// 튜토표시시간
bool firstStart = true;
float tutorialTimer = 0.0f;

// 시간
int   g_lastMs = 0;
float g_delta = 0.0f;

// 게임 타이머
float gameTimer = 180.0f;//180.0f;   // 3분(초)
bool timeUp = false;
float resultMsgTimer = 0.0f;

// 스프린트 관련
bool  wPressed = false;
bool  aPressed = false;
bool  sPressed = false;
bool  dPressed = false;
float baseSpeed = 0.12f;
float sprintSpeed = 0.12f;

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

void drawBigText(float x, float y, const std::string& s, float scale = 0.2f)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, scale);   // 🔥 글자 크기 배율 조절
    glLineWidth(2.0f);               // 글자 두께 조금 증가

    for (char c : s)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);

    glPopMatrix();
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
    // 튜토리얼 표시
    // ---------------------------
    if (g_state == TUTORIAL) {

        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, winW, 0, winH);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1, 1, 1); 

        std::string msg;

        if (tutorialTimer < 2.0f)        msg = "Make Your Own Color Spirit!";
        else if (tutorialTimer < 4.0f)   msg = "Find RGB Spirits to make it.";
        else if (tutorialTimer < 5.0f)   msg = "      Ready?            ";
        else                             msg = "       GO!            ";

        int textW = msg.length() * 10;
        drawBigText(winW / 2 - 150, winH / 2, msg, 0.2f);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);

        glutSwapBuffers();
        return;
    }

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
	// 🔵 우측 하단: 미니맵
    int mapSize = 120;  // 미니맵 크기
    int startX = winW - mapSize - 10;
    int startY = winH - mapSize - 30;
    
    // 미니맵 배경 (어두운 회색)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.15f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(startX, startY);
    glVertex2f(startX + mapSize, startY);
    glVertex2f(startX + mapSize, startY + mapSize);
    glVertex2f(startX, startY + mapSize);
    glEnd();

    // 미로 벽 그리기 (회색 점들)
    glColor3f(0.6f, 0.6f, 0.6f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int z = 0; z < 60; z++) {
        for (int x = 0; x < 60; x++) {
            if (g_maze->map[z][x] == 1) {
                float mx = startX + x * 2;  // 60 -> 120px
                float my = startY + z * 2;
                glVertex2f(mx, my);
            }
        }
    }
    glEnd();

    // 플레이어 위치 (빨간 점)
    float px = startX + (g_camera->x + 30) * 2;
    float py = startY + (g_camera->z + 30) * 2;

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(px, py);
    glEnd();

    glDisable(GL_BLEND);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(1.0f);
    glLineWidth(1.0f);

    // 🔵 좌측 상단: 제한시간 + RGB 카운트
    drawText(10, winH - 20,
        "Time Left: " + std::to_string((int)gameTimer) + "s");
    drawText(10, winH - 50,
        "  R: " + std::to_string(g_spirits->Rcount) +
        "  G: " + std::to_string(g_spirits->Gcount) +
        "  B: " + std::to_string(g_spirits->Bcount)
    );

    // 🔵 우측 상단: 플레이어 좌표 표시 추가
    {
        char pos[128];
        sprintf_s(pos, sizeof(pos),
            "X: %.2f  Y: %.2f  Z: %.2f",
            g_camera->x, g_camera->y, g_camera->z);

        drawText(winW - 260, winH - 20, pos);
    }

    // 🔵 우측 하단: 조작 방법
    drawText(winW - 200, 20, "WASD: Move");
    drawText(winW - 200, 40, "Mouse: Look");
    drawText(winW - 200, 60, "Space: Jump");
    drawText(winW - 200, 80, "F1: Debug Cam");

    if (resultMsgTimer > 0.0f) {
        drawBigText(winW / 3, winH - 50, "Congratulation! You Made Spirit!", 0.2f);
    }
    resultMsgTimer -= g_delta;
    if (resultMsgTimer < 0) resultMsgTimer = 0;


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
        // 처음 시작일 때만 튜토리얼 실행
        if (firstStart&& gameTimer >= 179.0f) {
            tutorialTimer = 0.0f;
            firstStart = false;
            g_state = GameState(2);   // 튜토리얼 전용 상태
            return;
        }
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
    if (key == 'a') aPressed = true;
    if (key == 's') sPressed = true;
    if (key == 'd') dPressed = true;

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
    if (key == 'a') aPressed = false;
    if (key == 's') sPressed = false;
    if (key == 'd') dPressed = false;
}

// -----------------------------
// Idle / Update
// -----------------------------
void update()
{
    int now = glutGet(GLUT_ELAPSED_TIME);
    g_delta = (now - g_lastMs) / 1000.0f;
    g_lastMs = now;
    // ------------------
   // 1) MENU 상태
   // ------------------
    if (g_state == MENU) {
        // 시간 멈춤 (아무것도 하지 않음)
        glutPostRedisplay();
        return;
    }

    // ----------------------------
    // 튜토리얼 상태 처리
    // ----------------------------
    if (g_state == TUTORIAL) {
        tutorialTimer += g_delta;

        // 시간 순서대로 문구가 바뀜
        if (tutorialTimer < 2.0f) { /* 2초간 메시지1 */ }
        else if (tutorialTimer < 4.0f) { /* 메시지2 */ }
        else if (tutorialTimer < 5.0f) { /* 메시지3 */ }
        else if (tutorialTimer < 6.0f) { /* 메시지4 */ }
        else 
        // 튜토리얼이 끝나면 PLAYING으로 전환
        if (tutorialTimer >= 7.0f) {
            g_state = PLAYING;

            glutSetCursor(GLUT_CURSOR_NONE);
            int cx = winW / 2;
            int cy = winH / 2;
            glutWarpPointer(cx, cy);
            g_camera->lastX = (float)cx;
            g_camera->lastY = (float)cy;
        }


        glutPostRedisplay();
        return; // 튜토리얼 중에는 게임 업데이트 멈춤
    }

    if (g_state == PLAYING) {
        g_camera->playTime += g_delta;
        gameTimer -= g_delta;
        if (gameTimer < 0) gameTimer = 0;
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
        if (aPressed) {
            g_camera->moveLeft(baseSpeed);
        }

        // --- S: 뒤 ---
        if (sPressed) {
            g_camera->moveBackward(baseSpeed);
        }

        // --- D: 우 ---
        if (dPressed) {
            g_camera->moveRight(baseSpeed);
        }
    }
    if (g_camMode == PLAYER_MODE) {
        bool gotSpeed = g_spirits->updateSpiritCollision(
            g_camera->x,
            g_camera->y,
            g_camera->z
        );

    }
    glutPostRedisplay();
    // ▼▼ 제한 시간 감소 ▼▼
    if (!timeUp) {
        if (gameTimer <= 0) {
            gameTimer = 0;
            timeUp = true;
        }
    }
    // ▼▼ 타임업 이벤트 처리 ▼▼
    if (timeUp) {
        // 카메라를 중앙으로 이동
        g_camera->setPosition(0.0f, 1.5f, 0.0f);
        // 카메라 방향
        float dx = cosf(g_camera->pitch * M_PI / 180) * cosf(g_camera->yaw * M_PI / 180);
        float dz = cosf(g_camera->pitch * M_PI / 180) * sinf(g_camera->yaw * M_PI / 180);
        // 결과 정령 생성
        g_spirits->showResultSpirit = true;
        g_spirits->spawnResultSpirit(g_camera->x, g_camera->y, g_camera->z, dx, dz);

        // UI 표시 2초
        resultMsgTimer = 2.0f;

        // RGB 초기화
        g_spirits->Rcount = 0;
        g_spirits->Gcount = 0;
        g_spirits->Bcount = 0;

        // 더 진행되면 안 되니까 return
        return;
    }

}

// -----------------------------
// GL 초기화
// -----------------------------
void initGL()
{
	// 조명 설정
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_pos[] = { 0.0f, 10.0f, 0.0f, 1.0f };  // 위에서 비추는 광원
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

	// 안개 설정
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);

    GLfloat fogColor[] = { 0.2f, 0.2f, 0.3f, 1.0f };  // 어두운 푸른 안개
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 5.0f);   // 안개 시작 거리
    glFogf(GL_FOG_END, 25.0f);    // 완전히 안개로 덮이는 거리

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

    g_camera->setPosition(0.0f, 3.5f, 0.0f);
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
