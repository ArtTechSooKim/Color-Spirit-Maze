#include "SpiritModel.h"
#include <GL/glut.h>

void SpiritModel::init() {
    body.init();
    leafFace.init();
    // Fire, Crystal은 초기화 필요 없음
}

void SpiritModel::draw(SpiritType type) {
    glPushMatrix();

    // 전체 정령 크기 축소
    glScalef(0.25f, 0.25f, 0.25f);

    // 몸통
    body.draw();

    // 얼굴은 위로 띄우기 (스케일 후에도 동일하게 비례)
    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);

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
    }

    glPopMatrix(); // 얼굴 pop
    glPopMatrix(); // 전체 pop
}