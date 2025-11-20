#include "Face_Leaf.h"
#include <GL/glut.h>

void FaceLeaf::init() {
    // 현재 별도 초기화 필요 없음
}

void FaceLeaf::draw() {
    glColor3f(0.1f, 1.0f, 0.3f);

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

    // 작은 새싹 머리
    glPushMatrix();
    glColor3f(0.3f, 0.9f, 0.2f);
    glTranslatef(0.0f, -0.2f, 0.0f);
    glutSolidSphere(0.25, 16, 16);
    glPopMatrix();
}
