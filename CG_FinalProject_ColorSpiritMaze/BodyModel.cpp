#include "BodyModel.h"
#include <GL/glut.h>

void BodyModel::init() {
    // 현재는 별도의 초기화 필요 없음 (GLUT 기본 도형 사용)
}

void BodyModel::draw() {
    // 몸통 색 (흰색 계열) - 원하는 색으로 바꿔도 됨
    glColor3f(1.0f, 1.0f, 1.0f);

    // 🟡 몸통 (가운데 큰 구)
    glPushMatrix();
    glScalef(1.0f, 1.2f, 1.0f);      // 세로로 조금 늘린 구
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();

    // 🟡 왼팔
    glPushMatrix();
    glTranslatef(-0.5f, 0.2f, 0.0f);   // 0.7 → 0.5: 몸쪽으로
    glRotatef(30, 0, 0, -2);           // 약간 기울임
    glScalef(0.3f, 0.7f, 0.3f);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // 🟡 오른팔
    glPushMatrix();
    glTranslatef(0.5f, 0.2f, 0.0f);    // 0.7 → 0.5
    glRotatef(-30, 0, 0, -2);          // 반대 방향으로 기울임
    glScalef(0.3f, 0.7f, 0.3f);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // 🟡 왼다리
    glPushMatrix();
    glTranslatef(-0.3f, -0.8f, 0.0f);
    glScalef(0.4f, 0.9f, 0.4f);
    glutSolidSphere(0.35, 20, 20);
    glPopMatrix();

    // 🟡 오른다리
    glPushMatrix();
    glTranslatef(0.3f, -0.8f, 0.0f);
    glScalef(0.4f, 0.9f, 0.4f);
    glutSolidSphere(0.35, 20, 20);
    glPopMatrix();
}
