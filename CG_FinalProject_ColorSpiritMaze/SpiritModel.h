#pragma once
#include "BodyModel.h"
#include "Face_Fire.h"
#include "Face_Leaf.h"
#include "Face_Crystal.h"

// 정령 타입 (빨/초/파)
enum SpiritType {
    RED_SPIRIT,
    GREEN_SPIRIT,
    BLUE_SPIRIT
};

class SpiritModel {
public:
    BodyModel body;
    FaceFire fireFace;
    FaceLeaf leafFace;
    FaceCrystal crystalFace;

    void init();
    void draw(SpiritType type);
};
