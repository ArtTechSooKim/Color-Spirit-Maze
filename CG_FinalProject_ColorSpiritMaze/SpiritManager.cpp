#include "SpiritManager.h"
#include <GL/glut.h>
#include <cmath>

void SpiritManager::initSpirits() {
    model.init();
    spirits.clear();

    std::vector<std::pair<float, float>> pos = {
        {-9,-11},{-5,-3},{-2,6},
        {3,-8},{7,4},{9,-10},
        {-9,8},{5,10},{3.5,2}
    };

    spirits.reserve(pos.size());

    for (int i = 0; i < pos.size(); i++) {
        Spirit s;
        s.x = pos[i].first;
        s.z = pos[i].second;
        s.y = 0;
        s.yOffset = 0.2f;

        // R, G, B 반복
        if (i % 3 == 0) s.type = RED_SPIRIT;
        else if (i % 3 == 1) s.type = GREEN_SPIRIT;
        else                s.type = BLUE_SPIRIT;

        spirits.push_back(s);
    }
}

void SpiritManager::drawSpirits() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 300.0f;

    for (auto& s : spirits) {
        if (s.collected) continue;

        glPushMatrix();
        glTranslatef(s.x, s.yOffset + std::sin(time) * 0.1f, s.z);
        glRotatef(time * 20.0f, 0, 1, 0);

        model.draw(s.type);

        glPopMatrix();
    }
}

// 녹색정령 수집 시 true 반환
bool SpiritManager::updateSpiritCollision(float px, float py, float pz) {
    bool gotSpeed = false;

    for (auto& s : spirits) {
        if (s.collected) continue;

        float dx = px - s.x;
        float dz = pz - s.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        if (dist < 0.8f) {
            s.collected = true;

            if (s.type == RED_SPIRIT)   Rcount++;
            if (s.type == GREEN_SPIRIT) {
                Gcount++;
                gotSpeed = true;
            }
            if (s.type == BLUE_SPIRIT)  Bcount++;
        }
    }

    return gotSpeed;
}
