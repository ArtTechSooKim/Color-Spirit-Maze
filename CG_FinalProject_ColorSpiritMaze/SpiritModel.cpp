#include "SpiritModel.h"

// 🔥 전역 변수 실제 정의
bool g_isMixedFace = false;
float g_mixedR = 1.0f;
float g_mixedG = 1.0f;
float g_mixedB = 1.0f;

void SpiritModel::setFaceColor(float r, float g, float b)
{
    g_isMixedFace = true;
    g_mixedR = r;
    g_mixedG = g;
    g_mixedB = b;
}

void SpiritModel::clearFaceColor()
{
    g_isMixedFace = false;
}
void SpiritModel::init() {
    body.init();
    leafFace.init();
}


void SpiritModel::drawBody()
{
    glPushMatrix();
    glScalef(0.5f, 0.5f, 0.5f);
    body.draw();
    glPopMatrix();
}

void SpiritModel::drawFace(SpiritType type)
{
    glPushMatrix();
    glScalef(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, 0.2f, 0.0f);

    // drawFace 내부에서는 색을 직접 지정하지 않음
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
        // geometry만 fireFace 사용한다고 가정
        fireFace.draw();
        break;
    }

    glPopMatrix();
}

void SpiritModel::draw(SpiritType type)
{
    glPushMatrix();
    glScalef(0.5f, 0.5f, 0.5f);
    body.draw();

    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);
    drawFace(type);
    glPopMatrix();

    glPopMatrix();
}
