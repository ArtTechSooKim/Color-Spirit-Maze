#pragma once
#include <GL/gl.h>

class Maze {
public:
    static const int SIZE = 30;
    int mazeData[SIZE][SIZE];

    Maze();
    void draw();

    // �浹 �˻�: �÷��̾��� x,z ��ǥ�� ������(r)�� ���� �浹�ϴ��� �˻�
    bool checkCollision(float px, float pz, float radius = 0.2f);

private:
    GLuint floorTex;
    GLuint wallTex;
};