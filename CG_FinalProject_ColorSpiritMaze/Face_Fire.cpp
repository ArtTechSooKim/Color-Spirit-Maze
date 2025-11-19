#include "Face_Fire.h"
#include <GL/glut.h>

void FaceFire::draw() {
    // 메인 불꽃 (위로 세워짐)
    glPushMatrix();
    glColor3f(1.0f, 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.6, 1.0, 20, 20);
    glPopMatrix();

    // 🌋 작은 불꽃 4개
    struct SmallFlame {
        float x, y, z;
        float r, g, b;
        float scale;
    };

    SmallFlame flames[4] = {
        // 작고 다양한 위치로 배치
        { 0.25f, 0.15f, 0.0f,   1.0f, 0.8f, 0.3f,  0.7f }, // 오른쪽 위
        { -0.25f, 0.15f, 0.0f,  1.0f, 0.75f, 0.25f, 0.7f }, // 왼쪽 위
        { 0.0f, 0.05f, 0.25f,   1.0f, 0.65f, 0.2f,  0.7f }, // 앞쪽
        { 0.0f, 0.10f, -0.25f,  1.0f, 0.7f, 0.3f,  0.7f }  // 뒤쪽
    };

    for (int i = 0; i < 4; i++) {
        glPushMatrix();

        glColor3f(flames[i].r, flames[i].g, flames[i].b);
        glTranslatef(flames[i].x, flames[i].y, flames[i].z);

        // 세로로 세우기
        glRotatef(-90, 1, 0, 0);

        glScalef(flames[i].scale, flames[i].scale, flames[i].scale);
        glutSolidCone(0.3, 0.6, 16, 16);

        glPopMatrix();
    }
}