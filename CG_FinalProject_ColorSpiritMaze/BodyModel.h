#pragma once
#include <GL/glut.h>

// 🔥 골드 바디 플래그 (전역)
extern bool g_isGoldBody;

class BodyModel {
public:
    void init();
    void draw();
};