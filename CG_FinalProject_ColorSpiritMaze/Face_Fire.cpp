#include "Face_Fire.h"
#include "Mesh.h"
#include <GL/glut.h>

extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;

// SOR FireFace mesh
static Mesh g_fireFaceMesh("Debug/assets/data\\FireFace.dat");

void FaceFire::draw()
{
    // ============================
    // 1) 내부 코어 먼저 그리기
    // ============================

    // Additive glow + (원하면 emission 추가)
    GLfloat coreEm[] = { 1.0f, 0.8f, 0.2f, 1.0f };
    GLfloat noEm[] = { 0,0,0,1 };
    glMaterialfv(GL_FRONT, GL_EMISSION, coreEm);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 빛 번짐

    glPushMatrix();
    glTranslatef(0.0f, -0.25f, 0.0f);

    // 중심부 코어
    glColor4f(1.0f, 0.95f, 0.6f, 0.9f);
    glutSolidSphere(0.14f, 20, 20);

    // 바깥 halo
    glColor4f(1.0f, 0.6f, 0.2f, 1.0f);
    glutSolidSphere(0.22f, 20, 20);

    glPopMatrix();

    glDisable(GL_BLEND);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEm);


    // ============================
    // 2) 그 위에 외곽 SOR 그리기 (반투명)
    // ============================
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 일반 알파 블렌딩

    if (g_isMixedFace)
        glColor4f(g_mixedR, g_mixedG, g_mixedB, 0.7f);
    else
        glColor4f(1.0f, 0.0f, 0.0f, 0.6f);

    glPushMatrix();
    glTranslatef(0.0f, -0.4f, 0.0f);
    glScalef(0.45f, 0.45f, 0.45f);
    g_fireFaceMesh.draw();
    glPopMatrix();

    glDisable(GL_BLEND);
}
