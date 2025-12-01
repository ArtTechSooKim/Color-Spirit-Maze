#include "Face_Crystal.h"
#include <GL/glut.h>

extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;

void FaceCrystal::draw() {

    if (g_isMixedFace)
        glColor3f(g_mixedR, g_mixedG, g_mixedB);
    else
        glColor3f(0.0f, 0.0f, 1.0f);

    // 위쪽
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // 아래쪽
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();
}
