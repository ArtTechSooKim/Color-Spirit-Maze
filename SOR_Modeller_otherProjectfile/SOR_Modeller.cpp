#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

// =========================
// 기본 설정
// =========================
int winWidth = 1000;
int winHeight = 660;

struct Point2 {
    float x;
    float y;
};

struct Vec3 {
    float x, y, z;
};

struct Face {
    int i0, i1, i2; // 0-based 인덱스
};

std::vector<Point2> profilePoints;

int lastMouseX = 0;
int lastMouseY = 0;

// -------------------------
// 회전 분할 개수 (360의 약수만 사용)
// -------------------------
static int allowedSlices[] = {
     1,  2,  3,  4,  5,  6,
     8,  9, 10, 12, 15, 18,
    20, 24, 30, 36, 40, 45,
    60, 72, 90, 120, 180, 360
};
static const int allowedCount = sizeof(allowedSlices) / sizeof(int);

// 시작값은 12로 두었습니다 (원하시면 다른 값으로 바꾸셔도 됩니다)
int g_slices = 12;

// =========================
// 텍스트 출력 유틸
// =========================
void drawBitmapString(float x, float y, const std::string& str)
{
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}

// =========================
// SOR 생성 + .dat 저장
// =========================
void saveSORToDAT(const std::string& filename)
{
    if (profilePoints.size() < 2) {
        std::cout << "[SOR] 점이 최소 2개 이상 필요합니다. (현재 "
            << profilePoints.size() << "개)\n";
        return;
    }

    int slices = g_slices;
    int rows = (int)profilePoints.size();

    std::cout << "[SOR] 프로파일 점 개수: " << rows << "\n";
    std::cout << "[SOR] 회전 분할(slices): " << slices << " (360의 약수)\n";

    // 1. 2D 점 → (radius, height)로 변환
    float centerX = winWidth * 0.5f;
    float centerY = winHeight * 0.5f;
    float scale = 0.1f; // 화면 → 모델 스케일 (원하면 나중에 조정)

    std::vector<float> radii(rows);
    std::vector<float> heights(rows);

    for (int i = 0; i < rows; ++i) {
        float sx = profilePoints[i].x;
        float sy = profilePoints[i].y;

        float r = std::fabs(sx - centerX) * scale;     // 축까지의 거리
        float h = (centerY - sy) * scale;              // 위/아래 → +/-

        radii[i] = r;
        heights[i] = h;

        std::cout << "  profile[" << i << "] screen("
            << sx << ", " << sy << ") -> r=" << r << ", h=" << h << "\n";
    }

    // 2. 회전해서 3D 정점 생성
    std::vector<Vec3> vertices;
    vertices.reserve(rows * slices);

    const float TWO_PI = 6.28318530718f;

    for (int i = 0; i < rows; ++i) {
        float r = radii[i];
        float h = heights[i];

        for (int k = 0; k < slices; ++k) {
            float theta = TWO_PI * k / slices;
            float cx = std::cos(theta);
            float sxin = std::sin(theta);

            Vec3 v;
            v.x = r * cx;
            v.y = h;
            v.z = r * sxin;
            vertices.push_back(v);
        }
    }

    int totalVerts = (int)vertices.size();
    std::cout << "[SOR] 생성된 정점 개수: " << totalVerts << "\n";

    // 3. 삼각형 면 생성
    std::vector<Face> faces;
    faces.reserve((rows - 1) * slices * 2);

    for (int i = 0; i < rows - 1; ++i) {
        for (int k = 0; k < slices; ++k) {
            int nextK = (k + 1) % slices;

            int i00 = i * slices + k;
            int i01 = i * slices + nextK;
            int i10 = (i + 1) * slices + k;
            int i11 = (i + 1) * slices + nextK;

            // Quad -> 두 개의 삼각형
            Face f1{ i00, i01, i10 };
            Face f2{ i01, i11, i10 };
            faces.push_back(f1);
            faces.push_back(f2);
        }
    }

    int totalFaces = (int)faces.size();
    std::cout << "[SOR] 생성된 면 개수: " << totalFaces << "\n";

    // 4. .dat 파일로 저장
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cout << "[SOR] 파일 열기 실패: " << filename << "\n";
        return;
    }

    out << "Vertices list\n";
    out << totalVerts << "\n";
    for (int i = 0; i < totalVerts; ++i) {
        out << vertices[i].x << " "
            << vertices[i].y << " "
            << vertices[i].z << "\n";
    }

    out << "Faces list\n";
    out << totalFaces << "\n";
    for (int i = 0; i < totalFaces; ++i) {
        out << faces[i].i0 << " "
            << faces[i].i1 << " "
            << faces[i].i2 << "\n";
    }

    out.close();

    std::cout << "[SOR] 저장 완료: " << filename << "\n";
    std::cout << "      이 파일을 ReadModel()에서 fname으로 설정해서 열면 됩니다.\n";
}

// =========================
// 화면 그리기
// =========================
void display()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 좌표축(가운데 십자선)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    // 세로선 (회전축)
    glVertex2f(winWidth * 0.5f, 0.0f);
    glVertex2f(winWidth * 0.5f, (float)winHeight);
    // 가로선
    glVertex2f(0.0f, winHeight * 0.5f);
    glVertex2f((float)winWidth, winHeight * 0.5f);
    glEnd();

    // 클릭한 점들
    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : profilePoints) {
        glVertex2f(p.x, p.y);
    }
    glEnd();

    // 안내 텍스트
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string info = "Input: (X:" + std::to_string(lastMouseX)
        + ", Y:" + std::to_string(lastMouseY)
        + "), Count: " + std::to_string(profilePoints.size());
    drawBitmapString(10.0f, 15.0f, info);

    drawBitmapString(10.0f, 35.0f, "[L-Click] Add point, [R-Click] Undo last");

    std::string info2 = "[S] Save SOR to .dat  [C] Clear points  [ESC] Exit";
    drawBitmapString(10.0f, 55.0f, info2);

    std::string info3 = "[A] Slices Down  [D] Slices Up  (slices = " + std::to_string(g_slices) + ")";
    drawBitmapString(10.0f, 75.0f, info3);

    glutSwapBuffers();
}

// =========================
// 창 크기 변경
// =========================
void reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)winWidth, (double)winHeight, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// =========================
// 마우스 콜백
// =========================
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;

        Point2 p;
        p.x = (float)x;
        p.y = (float)y;
        profilePoints.push_back(p);

        std::cout << "Clicked: (" << x << ", " << y << ") "
            << "Count = " << profilePoints.size() << std::endl;

        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (!profilePoints.empty()) {
            profilePoints.pop_back();
            std::cout << "Pop last point. Count = "
                << profilePoints.size() << std::endl;
        }
        glutPostRedisplay();
    }
}

// =========================
// 키보드
// =========================
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) { // ESC
        exit(0);
    }
    else if (key == 'c' || key == 'C') {
        profilePoints.clear();
        std::cout << "[SOR] 모든 점 삭제\n";
        glutPostRedisplay();
    }
    else if (key == 's' || key == 'S') {
        // 여기서 파일 이름을 바꿔서 다른 정령 바디/얼굴도 만들 수 있음
        std::string filename = "LeafFace.dat";
        saveSORToDAT(filename);
    }
    else if (key == 'a' || key == 'A') {
        // slices 감소 (360의 약수 배열 안에서만 이동)
        for (int i = 0; i < allowedCount; ++i) {
            if (allowedSlices[i] == g_slices && i > 0) {
                g_slices = allowedSlices[i - 1];
                break;
            }
        }
        std::cout << "[SOR] slices 감소 → " << g_slices << "\n";
        glutPostRedisplay();
    }
    else if (key == 'd' || key == 'D') {
        // slices 증가
        for (int i = 0; i < allowedCount; ++i) {
            if (allowedSlices[i] == g_slices && i < allowedCount - 1) {
                g_slices = allowedSlices[i + 1];
                break;
            }
        }
        std::cout << "[SOR] slices 증가 → " << g_slices << "\n";
        glutPostRedisplay();
    }
}

// =========================
// main
// =========================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Simple SOR Modeller - Point Input + DAT Export");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
