#include "Face_Leaf.h"
#include "Mesh.h"
#include <GL/glut.h>

extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;

// LeafFace.dat
static Mesh g_leafFaceMesh("assets/SOR/LeafFace.dat");

void FaceLeaf::init()
{
    // 지금은 필요 없음 (lazy load라 draw()에서 처리)
}

static void drawEars();

void FaceLeaf::draw()
{
    // 기본 색
    if (g_isMixedFace)
        glColor3f(g_mixedR, g_mixedG, g_mixedB);
    else
        glColor3f(0.0f, 1.0f, 0.0f);

    // 얼굴 중심부 (SOR)
    glPushMatrix();
    glTranslatef(0.0f, -0.4f, 0.0f);
    glScalef(0.45f, 0.45f, 0.45f);
    g_leafFaceMesh.draw();
    glPopMatrix();
    // 허리 (SOR)
    glPushMatrix();
    glTranslatef(0.0f, -1.8f, 0.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    g_leafFaceMesh.draw();
    glPopMatrix();

    // 양쪽 귀
    drawEars();
}

static void drawEars()
{
    // 귀는 기존처럼 타원 구 두 개
    glPushMatrix();
    glTranslatef(0.0f, -0.4f, 0.0f);
    glTranslatef(-0.4f, 0.35f, 0.0f);
    glRotatef(40, 0, 0, 1);
    glScalef(0.2f, 0.6f, 0.2f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.4f, 0.0f);
    glTranslatef(0.4f, 0.35f, 0.0f);
    glRotatef(-40, 0, 0, 1);
    glScalef(0.2f, 0.6f, 0.2f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}
