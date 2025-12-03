#pragma once

#include <vector>
#include <string>

// 간단한 정점/면 구조체
struct MeshVertex {
    float x, y, z;
};

struct MeshTriangle {
    unsigned int i0, i1, i2; // 0-based index
};

class Mesh {
public:
    Mesh();
    explicit Mesh(const std::string& path);

    void setFile(const std::string& path);

    // draw()가 처음 호출될 때 .dat 로드 + 디스플레이 리스트 생성
    void draw();

private:
    bool loaded;
    std::string filePath;
    std::vector<MeshVertex> vertices;
    std::vector<MeshTriangle> faces;
    unsigned int displayListId;

    void loadFromFile();
    void buildDisplayList();
};
