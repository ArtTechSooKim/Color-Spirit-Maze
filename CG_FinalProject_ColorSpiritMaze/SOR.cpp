#include "SOR.h"
#include <GL/glut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static std::vector<std::pair<float, float>> torchProfile = {
    {0.00f, 1.50f},
    {0.25f, 1.35f},
    {0.40f, 1.20f},
    {0.45f, 1.00f},
    {0.50f, 0.80f},
    {0.20f, 0.40f},
    {0.15f, 0.10f},
    {0.10f, 0.00f}
};

void SOR::generateTorchSpirit() {
    vertices.clear();
    int slices = 40;

    for (auto& p : torchProfile) {
        float r = p.first;
        float y = p.second;

        for (int i = 0; i < slices; i++) {
            float theta = (float)i / slices * 2 * M_PI;
            float x = r * cos(theta);
            float z = r * sin(theta);

            vertices.push_back({ x, y, z });
        }
    }
}

void SOR::draw() {
    int slices = 40;
    int rows = torchProfile.size();


    for (int j = 0; j < rows - 1; j++) {
        glBegin(GL_TRIANGLE_STRIP);

        for (int i = 0; i < slices; i++) {
            int idx1 = j * slices + i;
            int idx2 = (j + 1) * slices + i;

            Vertex v1 = vertices[idx1];
            Vertex v2 = vertices[idx2];

            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
        }

        glEnd();
    }
}
