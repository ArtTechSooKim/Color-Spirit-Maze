#include "SpiritManager.h"
#include "Camera.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

void SpiritManager::initSpirits() {
    // 정령 모델 생성
    sorModel.generateTorchSpirit();

    // 정령 위치 (지금은 기본값 0,0 → 네가 수정할 부분)
    std::vector<std::pair<float, float>> positions = {
        {-10.0f, -12.0f},  // 1번
        {5.0f, 8.0f},  // 2번
        {-5.0f, -8.0f},  // 3번
        {10.0f, 12.0f},  // 4번
        {1.0f, 10.0f},  // 5번
        {9.0f, -8.0f},  // 6번
        {-13.0f, 4.0f},  // 7번
        {3.0f, -6.0f}   // 8번
    };

    for (int i = 0; i < 8; i++) {
        Spirit s;
        s.x = positions[i].first;
        s.z = positions[i].second;
        s.y = -0.5f;

        // 번호별 색 할당
        switch (i) {
        case 0: s.r = 1; s.g = 0; s.b = 0; break;       // 1번 빨
        case 1: s.r = 0; s.g = 1; s.b = 0; break;       // 2번 초 (속도 아이템)
        case 2: s.r = 0; s.g = 0; s.b = 1; break;       // 3번 пар
        case 3: s.r = 1; s.g = 0; s.b = 0; break;       // 4번 빨
        case 4: s.r = 0; s.g = 1; s.b = 0; break;       // 5번 초 (속도 아이템)
        case 5: s.r = 0; s.g = 0; s.b = 1; break;       // 6번 파
        case 6: s.r = 1; s.g = 0; s.b = 0; break;       // 7번 빨
        case 7: s.r = 0; s.g = 1; s.b = 0; break;       // 8번 초 (속도 아이템)
        }

        spirits.push_back(s);
    }
}

void SpiritManager::drawSpirits() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 300.0f;

    for (auto& s : spirits) {
        if (s.collected) continue;

        glPushMatrix();
        glTranslatef(s.x, s.y + sin(time) * 0.1f, s.z);
        glRotatef(time * 20, 0, 1, 0);

        glColor3f(s.r, s.g, s.b);
        sorModel.draw();
        glPopMatrix();
    }
}

// 변경: 수집 시 녹색이면 true 반환하여 속도버프 트리거
bool SpiritManager::updateSpiritCollision(float px, float py, float pz) {
    bool gotSpeedItem = false;

    for (auto& s : spirits) {
        if (s.collected) continue;

        float dx = px - s.x;
        float dz = pz - s.z;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < 0.7f) {
            s.collected = true;

            if (s.r) Rcount++;
            if (s.g) Gcount++;
            if (s.b) Bcount++;

            // 녹색 정령이 속도 아이템 역할
            if (s.g) {
                gotSpeedItem = true;
            }
        }
    }

    return gotSpeedItem;
}
