#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

class Maze;

// 카메라 모드: 플레이어 / 디버그 플라이
enum CamMode {
    PLAYER_MODE,
    DEBUG_MODE
};

class Camera {
public:
    float x, y, z;
    float yaw, pitch;

    // 마우스 중심 기준 FPS 카메라용
    float lastX, lastY;
    bool  firstMouse;

    // 점프 / 중력
    float yVelocity;
    float jumpPower;
    bool  onGround;

    // 스프린트 (달리기)용 스태미나
    float stamina;
    float staminaMax;
    bool  sprinting;
    float wHoldTime;

    // 플레이 시간(초)
    float playTime;

    // 미로(충돌체크용)
    Maze* maze;
    bool ignoreNextMotion;

public:
    Camera();

    // 뷰 행렬 적용 (gluLookAt + 디버그 모드 처리)
    void apply();

    // 마우스 이동으로 시야 회전
    void handleMouseLook(int x, int y);

    // 이동 (Maze 충돌 반영)
    void moveForward(float s);
    void moveBackward(float s);
    void moveLeft(float s);
    void moveRight(float s);

    // (필요 시 확장용, 현재는 비어 있어도 됨)
    void mouseButton(int button, int state, int x, int y);

    // 점프 시작
    void jump();

    // 스프린트 스태미나 갱신 + 현재 속도 리턴
    float updateSprint(float dt, float baseSpeed, float sprintSpeed);

    // 위치 강제 세팅
    void setPosition(float px, float py, float pz);
};

#endif
