#pragma once
#include <cmath>

class Camera {
public:
    float x, y, z;
    float yaw, pitch;
    float lastX, lastY;
    bool mouseDown;

    Camera();

    void apply();              // gluLookAt Àû¿ë
    void moveForward(float s);
    void moveBackward(float s);
    void moveLeft(float s);
    void moveRight(float s);
    void mouseMotion(int x, int y);
    void mouseButton(int btn, int state, int x, int y);

private:
    void getDirection(float& dx, float& dy, float& dz);
};
