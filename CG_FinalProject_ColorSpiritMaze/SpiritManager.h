#pragma once
#include <vector>
#include "SOR.h"
#include "Maze.h"

struct Spirit {
    float x, y, z;
    float r, g, b;
    bool collected = false;
};

class SpiritManager {
public:
    std::vector<Spirit> spirits;
    SOR sorModel;
    Maze* maze = nullptr;

    int Rcount = 0;
    int Gcount = 0;
    int Bcount = 0;

    void initSpirits();
    void drawSpirits();
    // 변경: 속도 아이템 획득 여부를 반환
    bool updateSpiritCollision(float px, float py, float pz);
};
