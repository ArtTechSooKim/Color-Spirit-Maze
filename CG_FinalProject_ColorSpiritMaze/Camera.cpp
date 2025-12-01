#include "Camera.h"
#include "Maze.h"
#include <cmath>
#include <Windows.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 메인에서 정의되는 전역 카메라 모드
extern CamMode g_camMode;

// 생성자: 기본 위치/각도/상태 초기화
Camera::Camera() {
    
    x = 0.0f;
    y = 1.5f;
    z = 0.0f;

    yaw = 90.0f;
    pitch = 0.0f;

    lastX = 640.0f;
    lastY = 640.0f;
    firstMouse = true;

    yVelocity = 0.0f;
    jumpPower = 0.18f;
    onGround = true;

    stamina = 100.0f;
    staminaMax = 100.0f;
    sprinting = false;
    wHoldTime = 0.0f;

    playTime = 0.0f;

    maze = nullptr;

    ignoreNextMotion = false;
}

// yaw/pitch로부터 방향 벡터 계산
static void getDirection(float yaw, float pitch,
    float& dx, float& dy, float& dz)
{
    dx = cosf(pitch * M_PI / 180.0f) * cosf(yaw * M_PI / 180.0f);
    dy = sinf(pitch * M_PI / 180.0f);
    dz = cosf(pitch * M_PI / 180.0f) * sinf(yaw * M_PI / 180.0f);
}

// 카메라 적용
void Camera::apply()
{
    if (g_camMode == DEBUG_MODE)
    {
        // ======= Debug Free Fly Mode =======
        float dirX = cosf(pitch * M_PI / 180.0f) * cosf(yaw * M_PI / 180.0f);
        float dirY = sinf(pitch * M_PI / 180.0f);
        float dirZ = cosf(pitch * M_PI / 180.0f) * sinf(yaw * M_PI / 180.0f);

        float speed = 0.25f;

        if (GetAsyncKeyState('W') & 0x8000) { x += dirX * speed; y += dirY * speed; z += dirZ * speed; }
        if (GetAsyncKeyState('S') & 0x8000) { x -= dirX * speed; y -= dirY * speed; z -= dirZ * speed; }

        float rightX = cosf((yaw + 90) * M_PI / 180.0f);
        float rightZ = sinf((yaw + 90) * M_PI / 180.0f);

        if (GetAsyncKeyState('A') & 0x8000) { x -= rightX * speed; z -= rightZ * speed; }
        if (GetAsyncKeyState('D') & 0x8000) { x += rightX * speed; z += rightZ * speed; }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) y += speed;
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) y -= speed;

        // ===== apply free camera =====
        glLoadIdentity();
        glRotatef(-pitch, 1, 0, 0);
        glRotatef(-yaw, 0, 1, 0);
        glTranslatef(-x, -y, -z);

        return;
    }


    // ======== Player FPS Mode ========
    y += yVelocity;
    yVelocity -= 0.01f;
    if (y <= 1.5f) { y = 1.5f; yVelocity = 0; onGround = true; }

    float dx = cosf(pitch * M_PI / 180.0f) * cosf(yaw * M_PI / 180.0f);
    float dy = sinf(pitch * M_PI / 180.0f);
    float dz = cosf(pitch * M_PI / 180.0f) * sinf(yaw * M_PI / 180.0f);

    glLoadIdentity();
    gluLookAt(x, y, z, x + dx, y + dy, z + dz, 0, 1, 0);
}

// 마우스 이동 → yaw/pitch 변경 (중심 기준)
void Camera::handleMouseLook(int dx, int dy)
{
    float sensitivity = 0.15f;

    yaw += dx * sensitivity;
    pitch += dy * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

// 앞으로 이동 (Maze 충돌 고려)
void Camera::moveForward(float s) {
    float dx, dy, dz;
    getDirection(yaw, pitch, dx, dy, dz);

    // y 방향 움직임은 무시하고, xz 평면에서만 이동
    float nx = x + dx * s;
    float nz = z + dz * s;

    if (!maze || !maze->checkCollision(nx, nz, 0.3f)) {
        x = nx;
        z = nz;
    }
}

// 뒤로 이동
void Camera::moveBackward(float s) {
    float dx, dy, dz;
    getDirection(yaw, pitch, dx, dy, dz);

    float nx = x - dx * s;
    float nz = z - dz * s;

    if (!maze || !maze->checkCollision(nx, nz, 0.3f)) {
        x = nx;
        z = nz;
    }
}

// 좌측 이동 (strafe)
void Camera::moveLeft(float s) {
    float dx = cosf((yaw - 90.0f) * M_PI / 180.0f);
    float dz = sinf((yaw - 90.0f) * M_PI / 180.0f);

    float nx = x + dx * s;
    float nz = z + dz * s;

    if (!maze || !maze->checkCollision(nx, nz, 0.3f)) {
        x = nx;
        z = nz;
    }
}

// 우측 이동 (strafe)
void Camera::moveRight(float s) {
    float dx = cosf((yaw + 90.0f) * M_PI / 180.0f);
    float dz = sinf((yaw + 90.0f) * M_PI / 180.0f);

    float nx = x + dx * s;
    float nz = z + dz * s;

    if (!maze || !maze->checkCollision(nx, nz, 0.3f)) {
        x = nx;
        z = nz;
    }
}

// 지금은 별 행동 없음 (필요하면 클릭 사용 가능)
void Camera::mouseButton(int, int, int, int) {
    // 비워둠
}

// 점프 시작
void Camera::jump() {
    if (onGround) {
        yVelocity = jumpPower;
        onGround = false;
    }
}

// 스프린트 스태미나 업데이트 + 현재 속도 리턴
float Camera::updateSprint(float dt, float baseSpeed, float sprintSpeed) {
    if (sprinting) {
        stamina -= dt * 45.0f;
        if (stamina < 0.0f) stamina = 0.0f;
        if (stamina == 0.0f) sprinting = false;
        return sprintSpeed;
    }
    else {
        stamina += dt * 25.0f;
        if (stamina > staminaMax) stamina = staminaMax;
        return baseSpeed;
    }
}

// 강제로 위치 설정
void Camera::setPosition(float px, float py, float pz) {
    x = px;
    y = py;
    z = pz;
}
