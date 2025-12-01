#pragma once
#include <vector>
#include "Maze.h"
#include "SpiritModel.h"

// 정령 데이터
struct Spirit {
    float x = 0, z = 0;
    float y = 0.0f;
    float yOffset = 1.0f;
    SpiritType type;
    bool collected = false;

    // 혼합 정령용
    bool mixR = false;
    bool mixG = false;
    bool mixB = false;

    // 최종 색상
    float mixColorR = 1.0f;
    float mixColorG = 1.0f;
    float mixColorB = 1.0f;
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

    // 최종 결과 정령
    bool showResultSpirit = false;
    Spirit resultSpirit;

    void initSpirits();
    void drawSpirits();
    bool updateSpiritCollision(float px, float py, float pz);

    // TimeUp 결과 정령 생성
    void spawnResultSpirit(float px, float py, float pz, float dirX, float dirZ);
};
