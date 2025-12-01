#include "Face_Leaf.h"
#include <GL/glut.h>

// SpiritModel에서 설정해주는 전역 상태
extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;
void FaceLeaf::init() {
    // 필요없음
}
void FaceLeaf::draw() {

    if (g_isMixedFace)
        glColor3f(g_mixedR, g_mixedG, g_mixedB);
    else
        glColor3f(0.0f, 1.0f, 0.0f);

    // 왼쪽 잎
    glPushMatrix();
    glTranslatef(-0.25f, 0.0f, 0.0f);
    glRotatef(40, 0, 0, 1);
    glScalef(0.2f, 0.6f, 0.2f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    // 오른쪽 잎
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, 0.0f);
    glRotatef(-40, 0, 0, 1);
    glScalef(0.2f, 0.6f, 0.2f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    // 머리
    glPushMatrix();
    if (!g_isMixedFace)
        glColor3f(0.3f, 0.9f, 0.2f); // PURE일 때만
    glutSolidSphere(0.25, 16, 16);
    glPopMatrix();
}
