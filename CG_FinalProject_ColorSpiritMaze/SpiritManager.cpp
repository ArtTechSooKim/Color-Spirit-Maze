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

        // R → G → B 반복
        if (i % 3 == 0) s.type = SpiritType::RED_SPIRIT;
        else if (i % 3 == 1) s.type = SpiritType::GREEN_SPIRIT;
        else                s.type = SpiritType::BLUE_SPIRIT;

        spirits.push_back(s);
    }
}

void SpiritManager::drawSpirits() {
    float time = glutGet(GLUT_ELAPSED_TIME) / 300.0f;

    // ---- 필드에 떠다니는 기본 정령들 ----
    for (auto& s : spirits) {
        if (s.collected) continue;

        glPushMatrix();
        glTranslatef(s.x, s.yOffset + std::sin(time) * 0.1f, s.z);
        glRotatef(time * 20.0f, 0, 1, 0);

        model.draw(s.type);

        glPopMatrix();
    }

    // ---- 최종 결과 정령 ----
    if (showResultSpirit) {
        glPushMatrix();
        glTranslatef(resultSpirit.x, resultSpirit.yOffset, resultSpirit.z);

        // 몸통
        model.drawBody();

        // 단일 색일 때: 원래 face 그대로
        if (resultSpirit.type != SpiritType::MIXED_SPIRIT) {
            model.drawFace(resultSpirit.type);
        }
        // 혼합일 때: RGB 비율 색으로 Face 하나 칠하기
        else {
            // 🌈 RGB 비율로 색 설정 (spawnResultSpirit에서 계산해둔 값)
            // ---------------------------
            // 🔥 혼합정령 얼굴
            // ---------------------------
            glDisable(GL_COLOR_MATERIAL);

            glColor3f(
                resultSpirit.mixColorR,
                resultSpirit.mixColorG,
                resultSpirit.mixColorB
            );

            model.drawFace(SpiritType::MIXED_SPIRIT);

            glEnable(GL_COLOR_MATERIAL);
            glColor3f(1, 1, 1);
        }

        glPopMatrix();
    }
}

bool SpiritManager::updateSpiritCollision(float px, float py, float pz) {
    bool gotSpeed = false;

    for (auto& s : spirits) {
        if (s.collected) continue;

        float dx = px - s.x;
        float dz = pz - s.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        if (dist < 1.5f) {
            s.collected = true;

            if (s.type == SpiritType::RED_SPIRIT)   Rcount++;
            if (s.type == SpiritType::GREEN_SPIRIT) {
                Gcount++;
                gotSpeed = true;
            }
            if (s.type == SpiritType::BLUE_SPIRIT)  Bcount++;
        }
    }

    return gotSpeed;
}

void SpiritManager::spawnResultSpirit(float px, float py, float pz, float dirX, float dirZ)
{
    resultSpirit.collected = false;

    // reset 혼합 플래그
    resultSpirit.mixR = resultSpirit.mixG = resultSpirit.mixB = false;

    // 적당한 yOffset
    resultSpirit.yOffset = 1.5f;

    // 플레이어 앞 2m 지점
    resultSpirit.x = px + dirX * 2.0f;
    resultSpirit.z = pz + dirZ * 2.0f;

    int R = Rcount;
    int G = Gcount;
    int B = Bcount;

    // ---------------------------
    // RGB 혼합 색 계산
    // ---------------------------
    float total = R + G + B;
    if (total <= 0) total = 1;  // 0 나눗셈 방지

    resultSpirit.mixColorR = (float)R / total;
    resultSpirit.mixColorG = (float)G / total;
    resultSpirit.mixColorB = (float)B / total;

    // 모두 0이면 RED
    if (R == 0 && G == 0 && B == 0) {
        resultSpirit.type = SpiritType::RED_SPIRIT;
        return;
    }

    // 3개가 모두 같으면 랜덤
    if (R == G && G == B) {
        int t = rand() % 3;
        resultSpirit.type = (t == 0 ? SpiritType::RED_SPIRIT :
            t == 1 ? SpiritType::GREEN_SPIRIT :
            SpiritType::BLUE_SPIRIT);
        return;
    }

    int maxRGB = std::max({ R, G, B });

    // 단일 최대
    if (R == maxRGB && R > G && R > B) {
        resultSpirit.type = SpiritType::RED_SPIRIT;
        return;
    }
    if (G == maxRGB && G > R && G > B) {
        resultSpirit.type = SpiritType::GREEN_SPIRIT;
        return;
    }
    if (B == maxRGB && B > R && B > G) {
        resultSpirit.type = SpiritType::BLUE_SPIRIT;
        return;
    }

    // 두 개가 동률로 최대 → 혼합형
    resultSpirit.type = SpiritType::MIXED_SPIRIT;
    resultSpirit.mixR = (R == maxRGB);
    resultSpirit.mixG = (G == maxRGB);
    resultSpirit.mixB = (B == maxRGB);
}