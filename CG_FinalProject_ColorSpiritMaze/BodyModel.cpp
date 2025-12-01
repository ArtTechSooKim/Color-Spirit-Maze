#include "BodyModel.h"
#include <GL/glut.h>

// 🔥 실제 전역 변수 정의 (여기가 원본)
bool g_isGoldBody = false;

void BodyModel::init() {
    // 현재는 별도의 초기화 필요 없음 (GLUT 기본 도형 사용)
}

void BodyModel::draw() {
    // 몸통 색 (기본은 흰색, 골드 모드면 금색)
    if (g_isGoldBody) {
        // 5,4,2 는 너무 커서 전부 1로 클램프됨 → 1.0 기준으로 써야 함
        glColor3f(1.0f, 0.84f, 0.0f);   // 골드 느낌
    }
    else {
        glColor3f(1.0f, 1.0f, 1.0f);    // 기본 흰색
    }

    // 🟡 몸통 (가운데 큰 구)
    glPushMatrix();
    glScalef(1.0f, 1.2f, 1.0f);      // 세로로 조금 늘린 구
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();

    // 🟡 왼팔
    glPushMatrix();
    glTranslatef(-0.5f, 0.2f, 0.0f);
    glRotatef(30, 0, 0, -2);
    glScalef(0.3f, 0.7f, 0.3f);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // 🟡 오른팔
    glPushMatrix();
    glTranslatef(0.5f, 0.2f, 0.0f);
    glRotatef(-30, 0, 0, -2);
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