#pragma once
#include <vector>
#include "Maze.h"
#include "SpiritModel.h"

// 정령 데이터
struct Spirit {
    float x = 0.0f, z = 0.0f;
    float y = 0.0f;
    float yOffset = 1.0f;
    SpiritType type;
    bool collected = false;

    // 혼합 정령용 플래그 (필요시 사용)
    bool mixR = false;
    bool mixG = false;
    bool mixB = false;

    // 최종 혼합 색상 (RGB 비율)
    float mixColorR = 1.0f;
    float mixColorG = 1.0f;
    float mixColorB = 1.0f;

    // 혼합 정령일 때, "얼굴 모양"으로 사용할 기준 타입 (가장 강한 색)
    SpiritType faceBase = RED_SPIRIT;
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

    // 타임업 결과 정령
    bool showResultSpirit = false;
    Spirit resultSpirit;

    void initSpirits();
    void drawSpirits();
    bool updateSpiritCollision(float px, float py, float pz);

    // TimeUp 결과 정령 생성
    void spawnResultSpirit(float px, float py, float pz, float dirX, float dirZ);
};
