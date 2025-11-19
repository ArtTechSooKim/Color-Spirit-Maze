#pragma once

class Maze {
public:
    static const int SIZE = 30;
    int mazeData[SIZE][SIZE];

    Maze();
    void draw();
};
