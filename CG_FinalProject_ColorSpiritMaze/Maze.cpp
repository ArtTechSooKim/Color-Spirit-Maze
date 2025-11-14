#include "Maze.h"
#include <GL/glut.h>

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

void Maze::draw() {
    glColor3f(0.7f, 0.7f, 0.7f);

    for (int z = 0; z < 10; z++) {
        for (int x = 0; x < 10; x++) {
            if (mazeData[z][x] == 1) {
                glPushMatrix();
                glTranslatef(x - 5, 0, z - 5);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }
}
