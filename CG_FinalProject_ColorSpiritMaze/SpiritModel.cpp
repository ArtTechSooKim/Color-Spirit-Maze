#include "SpiritModel.h"
#include <GL/glut.h>

void SpiritModel::init() {
    body.init();
    leafFace.init();
    // fireFace, crystalFace는 init 필요 없음
}

// ----------------------------
// 전체 정령 통합 렌더(기존 방식)
// ----------------------------
void SpiritModel::draw(SpiritType type) {
    glPushMatrix();

    glScalef(0.5f, 0.5f, 0.5f);

    // 몸통
    body.draw();

    // 얼굴 위치
    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);

    drawFace(type);   // 🔥 통합 → drawFace 사용

    glPopMatrix(); // 얼굴
    glPopMatrix(); // 전체
}

// ----------------------------
// 몸통 단독 렌더
// ----------------------------
void SpiritModel::drawBody() {
    glPushMatrix();
    glScalef(0.5f, 0.5f, 0.5f);
    body.draw();
    glPopMatrix();
}

// ----------------------------
// 얼굴 단독 렌더 (혼합도 지원)
// ----------------------------
void SpiritModel::drawFace(SpiritType type)
{
    glPushMatrix();
    glScalef(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, 0.2f, 0.0f);

    switch (type) {
    case RED_SPIRIT:
        fireFace.draw();
        break;
    case GREEN_SPIRIT:
        leafFace.draw();
        break;
    case BLUE_SPIRIT:
        crystalFace.draw();
        break;
    case MIXED_SPIRIT:
        // 혼합은 밖에서 색만 바꾸고,
        // 여기서는 어떤 face 모양을 쓸지 정할 때만 사용해도 됨.
        fireFace.draw();
        break;
    }

    glPopMatrix();
}
