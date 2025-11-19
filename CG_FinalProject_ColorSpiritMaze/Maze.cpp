#include "Maze.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>
#include <vector>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int mazeData[10][10] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

static float clampf(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

Maze::Maze() : floorTex(0), wallTex(0) {}

// PNG texture loader (stb_image)
static GLuint loadTextureFromFile(const char* path) {
    int width, height, n;
    unsigned char* data = stbi_load(path, &width, &height, &n, 4);
    if (!data) {
        printf("Failed to load texture: %s\n", path);
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // 기본 필터링 (프로젝트에서 필요하면 mipmap 추가)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 래핑
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return tex;
}

void Maze::initTextures() {
    floorTex = loadTextureFromFile("floor_texture.png");
    wallTex = loadTextureFromFile("wall_texture.png");

    if (!floorTex) {
        printf("Warning: floor texture failed to load.\n");
    }

    if (!wallTex) {
        printf("Warning: wall texture failed to load.\n");
    }
}



// 충돌 검사: 플레이어는 원(반지름 r)으로 보고 각 벽 큐브(1x1, 중심 (cx,cz))와 충돌 검사
bool Maze::checkCollision(float px, float pz, float radius) {
    // 각 벽 블록마다 AABB 검사 — 실제로는 원 vs AABB (closest point)
    for (int z = 0; z < 10; ++z) {
        for (int x = 0; x < 10; ++x) {
            if (mazeData[z][x] != 1) continue;
            float cx = (float)x - 5.0f; // maze draw와 동일한 좌표계
            float cz = (float)z - 5.0f;
            // AABB extents for cube of size 1 centered at (cx,cz): [cx-0.5, cx+0.5]
            float nearestX = clampf(px, cx - 0.5f, cx + 0.5f);
            float nearestZ = clampf(pz, cz - 0.5f, cz + 0.5f);
            float dx = px - nearestX;
            float dz = pz - nearestZ;
            float dist2 = dx * dx + dz * dz;
            if (dist2 < radius * radius) {
                return true;
            }
        }
    }
    return false;
}

// draw: 바닥 텍스처 + 벽 텍스처(면별로 조명 기반 색상 적용)
void Maze::draw() {
    // 기본 GL 상태
    glEnable(GL_TEXTURE_2D);

    // --- 바닥 그리기 ---
    if (floorTex) {
        glBindTexture(GL_TEXTURE_2D, floorTex);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glPushMatrix();
    // 바닥을 넓게 그리기: (-5,-5) ~ (5,5)
    float size = 10.0f;
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-5.0f, 0.0f, -5.0f);
    glTexCoord2f(size, 0); glVertex3f(5.0f, 0.0f, -5.0f);
    glTexCoord2f(size, size); glVertex3f(5.0f, 0.0f, 5.0f);
    glTexCoord2f(0, size); glVertex3f(-5.0f, 0.0f, 5.0f);
    glEnd();
    glPopMatrix();

    // 빛의 방향 (월드 단위, 정규화)
    // 이 방향이 "빛이 오는 방향"이다. (예: 빛은 위쪽 약간 + 오른쪽에서 온다)
    float lightDir[3] = { -0.5f, -1.0f, -0.2f };
    // 정규화
    float len = sqrtf(lightDir[0] * lightDir[0] + lightDir[1] * lightDir[1] + lightDir[2] * lightDir[2]);
    if (len > 0.0001f) {
        lightDir[0] /= len; lightDir[1] /= len; lightDir[2] /= len;
    }



    // 벽(큐브) 그리기: 각 면마다 텍스처, 노멀을 정해 색상 보정
    if (wallTex) glBindTexture(GL_TEXTURE_2D, wallTex);

    // Enable blending for shadow-ish gentle effect on faces (not full shadows)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int z = 0; z < 10; ++z) {
        for (int x = 0; x < 10; ++x) {
            if (mazeData[z][x] != 1) continue;
            float cx = (float)x - 5.0f;
            float cz = (float)z - 5.0f;
            float half = 0.5f;

            // 각 면을 수동으로 그림(텍스처 좌표 포함)
            // 면들의 노멀
            struct Face { float nx, ny, nz; float vx[4], vy[4], vz[4]; };

            // We'll draw 6 faces: +X, -X, +Z, -Z, +Y, -Y
            // But we want wall with height 1 (from y=0 to y=1)
            float y0 = 0.0f, y1 = 1.0f;

            // Precompute face vertices and normals inline and draw per-face
            // +X face (right)
            {
                float nx = 1, ny = 0, nz = 0;
                // compute lighting intensity from dot(normal, -lightDir)
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.3f + 0.7f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);

                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx + half, y0, cz - half);
                glTexCoord2f(1, 0); glVertex3f(cx + half, y0, cz + half);
                glTexCoord2f(1, 1); glVertex3f(cx + half, y1, cz + half);
                glTexCoord2f(0, 1); glVertex3f(cx + half, y1, cz - half);
                glEnd();
            }

            // -X face (left)
            {
                float nx = -1, ny = 0, nz = 0;
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.3f + 0.7f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx - half, y0, cz + half);
                glTexCoord2f(1, 0); glVertex3f(cx - half, y0, cz - half);
                glTexCoord2f(1, 1); glVertex3f(cx - half, y1, cz - half);
                glTexCoord2f(0, 1); glVertex3f(cx - half, y1, cz + half);
                glEnd();
            }

            // +Z face (front)
            {
                float nx = 0, ny = 0, nz = 1;
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.3f + 0.7f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx - half, y0, cz + half);
                glTexCoord2f(1, 0); glVertex3f(cx + half, y0, cz + half);
                glTexCoord2f(1, 1); glVertex3f(cx + half, y1, cz + half);
                glTexCoord2f(0, 1); glVertex3f(cx - half, y1, cz + half);
                glEnd();
            }

            // -Z face (back)
            {
                float nx = 0, ny = 0, nz = -1;
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.3f + 0.7f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx + half, y0, cz - half);
                glTexCoord2f(1, 0); glVertex3f(cx - half, y0, cz - half);
                glTexCoord2f(1, 1); glVertex3f(cx - half, y1, cz - half);
                glTexCoord2f(0, 1); glVertex3f(cx + half, y1, cz - half);
                glEnd();
            }

            // Top face (+Y) - roof
            {
                float nx = 0, ny = 1, nz = 0;
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.4f + 0.6f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx - half, y1, cz - half);
                glTexCoord2f(1, 0); glVertex3f(cx + half, y1, cz - half);
                glTexCoord2f(1, 1); glVertex3f(cx + half, y1, cz + half);
                glTexCoord2f(0, 1); glVertex3f(cx - half, y1, cz + half);
                glEnd();
            }

            // Bottom face (-Y) - 보통 보이지 않음; 그리되 어둡게
            {
                float nx = 0, ny = -1, nz = 0;
                float dot = nx * (-lightDir[0]) + ny * (-lightDir[1]) + nz * (-lightDir[2]);
                float intensity = 0.2f + 0.5f * std::max(0.0f, dot);
                glColor4f(intensity, intensity, intensity, 1.0f);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(cx - half, y0, cz + half);
                glTexCoord2f(1, 0); glVertex3f(cx + half, y0, cz + half);
                glTexCoord2f(1, 1); glVertex3f(cx + half, y0, cz - half);
                glTexCoord2f(0, 1); glVertex3f(cx - half, y0, cz - half);
                glEnd();
            }
        }
    }

    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

