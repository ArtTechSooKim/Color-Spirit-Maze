// Maze.cpp - replace/extend existing Maze.cpp
#include "Maze.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

// texture util header (아래에서 제공할 Texture 클래스)
#include "Texture.h"

// 전역 텍스처 핸들 (private 멤버 대신 파일 스코프로 단순화)
static Texture g_floorTex;
static Texture g_wallTex;

// directional light (정규화된 벡터)
static float g_lightDir[3] = { -0.5f, -1.0f, -0.3f };

Maze::Maze() {
    int temp[30][30] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,0,1,0,1,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,1,1,0,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // ����
    for (int z = 0; z < 30; z++) {
        for (int x = 0; x < 30; x++) {
            mazeData[z][x] = temp[z][x];
        }
    }
}


void Maze::init() {
    // 텍스처 파일 경로: 프로젝트에서 assets/ 폴더 사용
    g_floorTex.loadFromFile("Debug/assets/floor_texture.png");
    g_wallTex.loadFromFile("Debug/assets/wall_texture.png");

    // 정규화
    float len = std::sqrt(g_lightDir[0] * g_lightDir[0] + g_lightDir[1] * g_lightDir[1] + g_lightDir[2] * g_lightDir[2]);
    if (len > 0.0001f) {
        g_lightDir[0] /= len; g_lightDir[1] /= len; g_lightDir[2] /= len;
    }
}

// (유틸) AABB와 원(circle) 충돌 검사: player는 y축 높이 무시, xz 평면에서 검사
bool Maze::isWallAt(int mx, int mz) {
    if (mx < 0 || mx >= SIZE || mz < 0 || mz >= SIZE) return false;
    return mazeData[mz][mx] == 1;
}

bool Maze::checkCollision(float px, float pz, float radius) {
    // 격자 n,m 위치마다 벽이 있으면 AABB를 검사
    for (int z = 0; z < SIZE; ++z) {
        for (int x = 0; x < SIZE; ++x) {
            if (mazeData[z][x] != 1) continue;

            // wall cube center: (x - 15, 0, z - 15), cube size = 1.0 => extent = 0.5
            float cx = x - 15.0f;
            float cz = z - 15.0f;
            float half = 0.5f;

            // AABB min/max
            float minx = cx - half;
            float maxx = cx + half;
            float minz = cz - half;
            float maxz = cz + half;

            // 원-사각형 최근점 계산 (closest point on AABB to circle center(px,pz)):
            float closestX = std::max(minx, std::min(px, maxx));
            float closestZ = std::max(minz, std::min(pz, maxz));

            float dx = px - closestX;
            float dz = pz - closestZ;
            float dist2 = dx * dx + dz * dz;

            if (dist2 < radius * radius) {
                return true; // 충돌
            }
        }
    }
    return false;
}

// draw face with texture coords and simple shading
static void drawTexturedCubeFace(float cx, float cy, float cz, // center
    float half, int faceIndex,
    GLuint texID) {
    // faceIndex: 0=+X,1=-X,2=+Y,3=-Y,4=+Z,5=-Z
    // compute normal
    float nx = 0, ny = 0, nz = 0;
    if (faceIndex == 0) { nx = 1; }
    if (faceIndex == 1) { nx = -1; }
    if (faceIndex == 2) { ny = 1; }
    if (faceIndex == 3) { ny = -1; }
    if (faceIndex == 4) { nz = 1; }
    if (faceIndex == 5) { nz = -1; }

    // shading: dot(normal, -lightDir) -> (면이 빛을 얼마나 받는지)
    float dot = std::max(0.0f, nx * (-g_lightDir[0]) + ny * (-g_lightDir[1]) + nz * (-g_lightDir[2]));
    // ambient + diffuse
    float brightness = 0.6f + 0.4f * dot;
    if (brightness > 1.0f) brightness = 1.0f;

    glBindTexture(GL_TEXTURE_2D, texID);
    glColor4f(brightness, brightness, brightness, 1.0f);

    glBegin(GL_QUADS);
    if (faceIndex == 0) { // +X
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0); glVertex3f(cx + half, cy - half, cz - half);
        glTexCoord2f(1, 0); glVertex3f(cx + half, cy - half, cz + half);
        glTexCoord2f(1, 1); glVertex3f(cx + half, cy + half, cz + half);
        glTexCoord2f(0, 1); glVertex3f(cx + half, cy + half, cz - half);
    }
    else if (faceIndex == 1) { // -X
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0); glVertex3f(cx - half, cy - half, cz + half);
        glTexCoord2f(1, 0); glVertex3f(cx - half, cy - half, cz - half);
        glTexCoord2f(1, 1); glVertex3f(cx - half, cy + half, cz - half);
        glTexCoord2f(0, 1); glVertex3f(cx - half, cy + half, cz + half);
    }
    else if (faceIndex == 2) { // +Y top
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0); glVertex3f(cx - half, cy + half, cz - half);
        glTexCoord2f(1, 0); glVertex3f(cx + half, cy + half, cz - half);
        glTexCoord2f(1, 1); glVertex3f(cx + half, cy + half, cz + half);
        glTexCoord2f(0, 1); glVertex3f(cx - half, cy + half, cz + half);
    }
    else if (faceIndex == 3) { // -Y bottom
        glNormal3f(0, -1, 0);
        glTexCoord2f(0, 0); glVertex3f(cx - half, cy - half, cz + half);
        glTexCoord2f(1, 0); glVertex3f(cx + half, cy - half, cz + half);
        glTexCoord2f(1, 1); glVertex3f(cx + half, cy - half, cz - half);
        glTexCoord2f(0, 1); glVertex3f(cx - half, cy - half, cz - half);
    }
    else if (faceIndex == 4) { // +Z
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0); glVertex3f(cx - half, cy - half, cz + half);
        glTexCoord2f(1, 0); glVertex3f(cx + half, cy - half, cz + half);
        glTexCoord2f(1, 1); glVertex3f(cx + half, cy + half, cz + half);
        glTexCoord2f(0, 1); glVertex3f(cx - half, cy + half, cz + half);
    }
    else if (faceIndex == 5) { // -Z
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0); glVertex3f(cx + half, cy - half, cz - half);
        glTexCoord2f(1, 0); glVertex3f(cx - half, cy - half, cz - half);
        glTexCoord2f(1, 1); glVertex3f(cx - half, cy + half, cz - half);
        glTexCoord2f(0, 1); glVertex3f(cx + half, cy + half, cz - half);
    }
    glEnd();
}

void Maze::draw() {
    glDisable(GL_CULL_FACE);
    // floor: 큰 quad (텍스처 바닥)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_floorTex.getID());
    glColor3f(1, 1, 1);

    float tiling = 6.0f;

    glBegin(GL_QUADS);
    // 텍스처 타일링: 정수 크기마다 반복
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-15.0f - 0.5f, 0.0f, -15.0f - 0.5f);
    glTexCoord2f(tiling / 2, 0.0f); glVertex3f(15.0f + 0.5f, 0.0f, -15.0f - 0.5f);
    glTexCoord2f(tiling / 2, tiling / 2); glVertex3f(15.0f + 0.5f, 0.0f, 15.0f + 0.5f);
    glTexCoord2f(0.0f, tiling / 2); glVertex3f(-15.0f - 0.5f, 0.0f, 15.0f + 0.5f);
    glEnd();

    // walls: 각 셀마다 텍스처 매핑 및 쉐이딩
    glBindTexture(GL_TEXTURE_2D, g_wallTex.getID());
    for (int z = 0; z < SIZE; z++) {
        for (int x = 0; x < SIZE; x++) {
            if (mazeData[z][x] == 1) {
                float cx = x - 15.0f;
                float cy = 1.0f; // 중심 y
                float cz = z - 15.0f;
                float half = 0.5f;

                // 6 faces
                for (int f = 0; f < 6; ++f) {
                    drawTexturedCubeFace(cx, cy, cz, half, f, g_wallTex.getID());
                }
            }
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
}