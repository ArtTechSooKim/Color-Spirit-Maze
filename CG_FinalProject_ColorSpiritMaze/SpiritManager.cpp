#include "SpiritManager.h"
#include <GL/glut.h>
#include <cmath>

void SpiritManager::initSpirits() {
    model.init();

    // 9개 정령 위치 (원하는대로 수정 가능)
    std::vector<std::pair<float, float>> pos = {
        {-9.0f, -11.0f},
        {-5.0f,   -3.0f},
        { -2.0f,   6.0f},
        {  3.0f,  -8.0f},
        {  7.0f,   4.0f},
        { 9.0f, -10.0f},
        { -9.0f,   8.0f},
        {  5.0f,  10.0f},
        {  3.5f,  2.0f}
    };

    spirits.clear();
    spirits.reserve(9);

    for (int i = 0; i < 9; i++) {
        Spirit s;
        s.x = pos[i].first;
        s.z = pos[i].second;
        s.yOffset = 0.2f;

        // 타입: R → G → B 반복
        if (i % 3 == 0)      s.type = RED_SPIRIT;
        else if (i % 3 == 1) s.type = GREEN_SPIRIT;
        else                 s.type = BLUE_SPIRIT;

        spirits.push_back(s);
    }
}

void SpiritManager::drawSpirits() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 300.0f;

    for (auto& s : spirits) {
        if (s.collected) continue;

        glPushMatrix();
        // 통통하게 둥둥 떠다니는 느낌
        float bob = std::sin(time) * 0.1f;
        glTranslatef(s.x, s.yOffset + bob, s.z);

        // 천천히 회전
        glRotatef(time * 20.0f, 0, 1, 0);

        model.draw(s.type);
        glPopMatrix();
    }
}

void SpiritManager::updateSpiritCollision(float px, float py, float pz) {
    for (auto& s : spirits) {
        if (s.collected) continue;

        float dx = px - s.x;
        float dz = pz - s.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        if (dist < 0.8f) {
            s.collected = true;
            // TODO: 필요하면 여기에서 RGB 카운트 증가 등 처리
        }
    }
}
