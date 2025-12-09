#include "Face_Crystal.h"
#include "Mesh.h"
#include <GL/glut.h>

extern bool g_isMixedFace;
extern float g_mixedR, g_mixedG, g_mixedB;

// CrystalFace.dat
static Mesh g_crystalFaceMesh("assets/SOR/CrystalFace.dat");

void FaceCrystal::draw()
{
    // ±âº» »ö
    if (g_isMixedFace)
        glColor3f(g_mixedR, g_mixedG, g_mixedB);
    else
        glColor3f(0.0f, 0.0f, 1.0f);

    // ¾Æ·¡ÂÊ cone (ÅÎ)
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // À§ÂÊ cone (Á¤¼ö¸®)
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0f, 0.0f, 1.0f);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // Mini cone (ÅÎ)
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f);
    glRotatef(90, 1, 0, 0);
    glTranslatef(0.0f, 0.0f, -1.0f);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // Mini cone (Á¤¼ö¸®)
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0.0f, 0.0f, 1.0f);
    glutSolidCone(0.5, 0.8, 20, 20);
    glPopMatrix();

    // °¡¿îµ¥ ÄÅÈ¦´õ/¶ì (SOR ¾ó±¼)
    glPushMatrix();
    if (!g_isMixedFace)
        glColor3f(0.8f, 0.9f, 1.0f);

    glTranslatef(0.0f, 0.5f, 0.0f);  // µÎ cone »çÀÌ
    glScalef(0.6f, 0.6f, 0.6f);
    g_crystalFaceMesh.draw();
    glPopMatrix();
}
