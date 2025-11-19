#pragma once
#include <vector>

struct Vertex {
    float x, y, z;
};

class SOR {
public:
    std::vector<Vertex> vertices;

    void draw();
    void generateTorchSpirit();
};
