#include "Camera.h"
#include <GL/glut.h>
#include <cstdio>   // ★ 로그 출력
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera() {
    x = 0; y = 5; z = 10;
    yaw = 180; pitch = -20;
    mouseDown = false;

}

void Camera::getDirection(float& dx, float& dy, float& dz) {
    dx = cosf(pitch * M_PI / 180.0f) * sinf(yaw * M_PI / 180.0f);
    dy = sinf(pitch * M_PI / 180.0f);
    dz = cosf(pitch * M_PI / 180.0f) * cosf(yaw * M_PI / 180.0f);

    
}

void Camera::apply() {
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    

    gluLookAt(
        x, y, z,
        x + dx, y + dy, z + dz,
        0, 1, 0
    );
}

void Camera::moveForward(float s) {
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    x += dx * s; y += dy * s; z += dz * s;
}

void Camera::moveBackward(float s) {
    float dx, dy, dz;
    getDirection(dx, dy, dz);

    x -= dx * s; y -= dy * s; z -= dz * s;

}

void Camera::moveLeft(float s) {
    x += cosf((yaw - 90) * M_PI / 180.0f) * s;
    z += sinf((yaw - 90) * M_PI / 180.0f) * s;

}

void Camera::moveRight(float s) {
    x += cosf((yaw + 90) * M_PI / 180.0f) * s;
    z += sinf((yaw + 90) * M_PI / 180.0f) * s;

}

void Camera::mouseButton(int btn, int state, int x, int y) {
    if (btn == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = true;
            lastX = x; lastY = y;

        }
        else {
            mouseDown = false;
        }
    }
}

void Camera::mouseMotion(int x, int y) {
    if (!mouseDown) {
        return;
    }

    int dx = x - lastX;
    int dy = y - lastY;

    yaw += dx * 0.2f;
    pitch -= dy * 0.2f;

    if (pitch > 89) pitch = 89;
    if (pitch < -89) pitch = -89;

    lastX = x; lastY = y;
}
