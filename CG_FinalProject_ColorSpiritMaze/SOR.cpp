#include "SOR.h"
#include <GL/glut.h>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
void SOR::generateFakeSphere() {
    vertices.clear();

    int slices = 20, stacks = 20;

    for (int i = 0; i <= stacks; i++) {
        float v = i / (float)stacks;
        float phi = v * M_PI;

        for (int j = 0; j <= slices; j++) {
            float u = j / (float)slices;
            float theta = u * 2 * M_PI;

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            vertices.push_back({ x, y, z });
        }
    }
}

void SOR::draw() {
    glPointSize(3);
    glBegin(GL_POINTS);
    for (auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
}
