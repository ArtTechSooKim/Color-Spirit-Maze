#pragma once
#include <vector>
#include "Maze.h"
#include "SpiritModel.h"

// 정령 데이터
struct Spirit {
    float x, z;
    float y = 0.0f;
    float yOffset = 0.2f;
    SpiritType type;    // SpiritModel.h에서 정의된 enum 사용
    bool collected = false;
};

class SpiritManager {
public:
    std::vector<Spirit> spirits;
    Maze* maze = nullptr;

    SpiritModel model;

    // RGB 카운트
    int Rcount = 0;
    int Gcount = 0;
    int Bcount = 0;

    void initSpirits();
    void drawSpirits();

    bool updateSpiritCollision(float px, float py, float pz);
};
