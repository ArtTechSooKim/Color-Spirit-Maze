#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include <iostream>

Mesh::Mesh()
    : loaded(false), displayListId(0)
{
}

Mesh::Mesh(const std::string& path)
    : loaded(false), filePath(path), displayListId(0)
{
}

void Mesh::setFile(const std::string& path)
{
    filePath = path;
    loaded = false;
}

static MeshVertex computeNormal(
    const MeshVertex& a,
    const MeshVertex& b,
    const MeshVertex& c)
{
    MeshVertex p, q, n;
    p.x = b.x - a.x; p.y = b.y - a.y; p.z = b.z - a.z;
    q.x = c.x - a.x; q.y = c.y - a.y; q.z = c.z - a.z;
    n.x = p.y * q.z - p.z * q.y;
    n.y = p.z * q.x - p.x * q.z;
    n.z = p.x * q.y - p.y * q.x;

    float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 1e-6f) {
        n.x /= len;
        n.y /= len;
        n.z /= len;
    }
    return n;
}

void Mesh::loadFromFile()
{
    vertices.clear();
    faces.clear();

    if (filePath.empty()) {
        std::cerr << "[Mesh] filePath is empty\n";
        return;
    }

    FILE* f = nullptr;
    if (fopen_s(&f, filePath.c_str(), "rt") != 0 || !f) {
        std::cerr << "[Mesh] Failed to open: " << filePath << "\n";
        return;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), f)) {
        std::string s(line);
        for (auto& c : s) c = (char)tolower(c);

        if (s.find("vertices") != std::string::npos) {
            fgets(line, sizeof(line), f);
            count = std::stoi(line);
            vertices.reserve(count);
            for (int i = 0; i < count; ++i) {
                float x, y, z;
                if (!fgets(line, sizeof(line), f)) break;
                std::sscanf(line, "%f %f %f", &x, &y, &z);
                vertices.push_back({ x, y, z });
            }
        }
        else if (s.find("faces") != std::string::npos) {
            fgets(line, sizeof(line), f);
            count = std::stoi(line);
            faces.reserve(count);
            for (int i = 0; i < count; ++i) {
                unsigned int a, b, c;
                if (!fgets(line, sizeof(line), f)) break;
                std::sscanf(line, "%u %u %u", &a, &b, &c);
                faces.push_back({ a, b, c });
            }
        }
    }

    fclose(f);
}

void Mesh::buildDisplayList()
{
    if (displayListId != 0 && glIsList(displayListId)) {
        glDeleteLists(displayListId, 1);
    }

    displayListId = glGenLists(1);
    glNewList(displayListId, GL_COMPILE);

    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (const auto& tri : faces) {
        const MeshVertex& v0 = vertices[tri.i0];
        const MeshVertex& v1 = vertices[tri.i1];
        const MeshVertex& v2 = vertices[tri.i2];

        MeshVertex n = computeNormal(v0, v1, v2);
        glNormal3f(n.x, n.y, n.z);

        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();

    glEndList();
}

void Mesh::draw()
{
    if (!loaded) {
        loadFromFile();
        buildDisplayList();
        loaded = true;
    }

    if (displayListId != 0) {
        glCallList(displayListId);
    }
}
