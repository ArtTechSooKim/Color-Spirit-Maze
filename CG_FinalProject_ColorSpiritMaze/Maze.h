#pragma once
#include <GL/gl.h>

class Maze {
public:
    Maze();

    void initTextures(); // 텍스처 로드 (main에서 한 번 호출)
    void draw();

    // 충돌 검사: 플레이어의 x,z 좌표와 반지름(r)로 벽과 충돌하는지 검사
    bool checkCollision(float px, float pz, float radius = 0.2f);

private:
    GLuint floorTex;
    GLuint wallTex;
};