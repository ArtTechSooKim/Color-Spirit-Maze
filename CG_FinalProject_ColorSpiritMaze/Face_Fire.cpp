#include "Face_Fire.h"
#include <GL/glut.h>

extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;
void FaceFire::draw()
{
    // 메인 불꽃 색
    if (g_isMixedFace)
        glColor3f(g_mixedR, g_mixedG, g_mixedB);
    else
        glColor3f(1.0f, 0.0f, 0.0f);

    // 메인 불꽃
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.6, 1.0, 20, 20);
    glPopMatrix();

    // 작은 불꽃 4개
    struct SmallFlame {
        float x, y, z;
        float r, g, b;
        float scale;
    };

    SmallFlame flames[4] = {
        { 0.25f, 0.15f, 0.0f,   1.0f,0.8f,0.3f,  0.7f },
        { -0.25f,0.15f,0.0f,    1.0f,1.75f,0.25f,0.7f },
        { 0.0f,0.05f,0.25f,     1.0f,0.65f,1.2f, 0.7f },
        { 0.0f,0.10f,-0.25f,    2.0f,0.7f,0.3f,  0.7f }
    };

    for (int i = 0; i < 4; i++) {
        glPushMatrix();

        if (g_isMixedFace)
            glColor3f(g_mixedR, g_mixedG, g_mixedB);
        else
            glColor3f(flames[i].r, flames[i].g, flames[i].b);

        glTranslatef(flames[i].x, flames[i].y, flames[i].z);
        glRotatef(-90, 1, 0, 0);
        glScalef(flames[i].scale, flames[i].scale, flames[i].scale);

        glutSolidCone(0.3, 0.6, 16, 16);

        glPopMatrix();
    }
}
