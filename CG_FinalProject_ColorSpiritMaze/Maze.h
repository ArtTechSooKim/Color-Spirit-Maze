#pragma once
#include <vector>

class Maze
{
public:
    Maze();

    void init();
    void draw();

    bool checkCollision(float px, float pz, float radius);

    std::vector<std::vector<int>> map;
};
