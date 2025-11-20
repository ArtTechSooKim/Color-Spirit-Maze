#include "Face_Crystal.h"
#include <GL/glut.h>

void FaceCrystal::draw() {
    glColor3f(0.4f, 0.7f, 1.0f);

    // 위쪽 뾰족
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);  // Z→Y 방향으로 바로 세우기
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // 아래쪽 뾰족
    glPushMatrix();
    glRotatef(90, 1, 0, 0);   // 반대 방향
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();
}
