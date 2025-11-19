#pragma once
#include <vector>
#include "Maze.h"
#include "SpiritModel.h"

struct Spirit {
    float x, y, z;
    float yOffset;        // 약간 띄우기용 높이
    SpiritType type;      // RED, GREEN, BLUE
    bool collected = false;
};

class SpiritManager {
public:
    std::vector<Spirit> spirits;
    Maze* maze = nullptr;

    SpiritModel model;

    void initSpirits();
    void drawSpirits();
    void updateSpiritCollision(float px, float py, float pz);
};
