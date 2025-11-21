#pragma once

class Maze {
public:
    static const int SIZE = 30;
    int mazeData[SIZE][SIZE];

    Maze();
    void init();            // 새로 추가: 텍스처 등 초기화용
    void draw();

    bool isWallAt(int mx, int mz);                  // 격자 좌표에 벽이 있는지
    bool checkCollision(float px, float pz, float radius); // 플레이어 원(circle) vs 모든 AABB 검사
};