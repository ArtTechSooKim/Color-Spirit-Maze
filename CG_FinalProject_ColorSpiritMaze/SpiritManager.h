#pragma once
#include <vector>
#include "Maze.h"
#include "SpiritModel.h"

// 정령 타입, 위치 구조
struct Spirit {
    float x, y, z;
    float yOffset = 0.2f;
    SpiritType type;
    bool collected = false;
};

class SpiritManager {
public:
    std::vector<Spirit> spirits;
    Maze* maze = nullptr;

    SpiritModel model;

    // RGB 카운트 (팀원 기능 유지)
    int Rcount = 0;
    int Gcount = 0;
    int Bcount = 0;

    void initSpirits();
    void drawSpirits();

    // 초록정령 수집 시 true 반환 (속도 버프용)
    bool updateSpiritCollision(float px, float py, float pz);
};
