#pragma once
#include "BodyModel.h"
#include "Face_Fire.h"
#include "Face_Leaf.h"
#include "Face_Crystal.h"
#include <GL/glut.h>
extern bool g_isMixedFace;
extern float g_mixedR;
extern float g_mixedG;
extern float g_mixedB;
// 정령 타입
enum SpiritType {
    RED_SPIRIT,
    GREEN_SPIRIT,
    BLUE_SPIRIT,
    MIXED_SPIRIT
};

// 🔥 글로벌 혼합색 상태 전달용 extern 선언
extern bool g_useCustomFaceColor;
extern float g_faceR;
extern float g_faceG;
extern float g_faceB;

class SpiritModel {
public:
    BodyModel  body;
    FaceFire   fireFace;
    FaceLeaf   leafFace;
    FaceCrystal crystalFace;

    void init();
    void draw(SpiritType type);

    void drawBody();
    void drawFace(SpiritType type);

    // 🔥 혼합 정령용 얼굴 색 적용/해제 함수
    void setFaceColor(float r, float g, float b);
    void clearFaceColor();
};
