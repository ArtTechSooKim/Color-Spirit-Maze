#include "SpiritManager.h"
#include <GL/glut.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
extern bool g_isGoldBody;
void SpiritManager::initSpirits() {
    model.init();
    spirits.clear();

    std::vector<std::pair<float, float>> pos = {
        {-9,-11},{-5,-3},{-2,6},
        {3,-8},{7,4},{9,-10},
        {-9,8},{5,10},{3.5,2}
    };

    spirits.reserve(pos.size());

    for (int i = 0; i < (int)pos.size(); i++) {
        Spirit s;
        s.x = pos[i].first;
        s.z = pos[i].second;

        // R → G → B 반복
        if (i % 3 == 0)      s.type = SpiritType::RED_SPIRIT;
        else if (i % 3 == 1) s.type = SpiritType::GREEN_SPIRIT;
        else                 s.type = SpiritType::BLUE_SPIRIT;

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

    // ---- 타임업 결과 정령 ----
    if (showResultSpirit) {
        glPushMatrix();

        glTranslatef(
            resultSpirit.x,
            resultSpirit.yOffset + std::sin(time) * 0.1f,
            resultSpirit.z
        );
        glRotatef(time * 20.0f, 0, 1, 0);

        // PURE 정령 → 기존처럼 렌더링
        if (resultSpirit.type != SpiritType::MIXED_SPIRIT) {
            model.draw(resultSpirit.type);
        }
        else {
            // --------------------------------------
            // 🔥🔥 여기서부터가 Face_xxx.cpp로 플래그 전달하는 부분 🔥🔥
            // --------------------------------------
            // 1) 몸통 골드 모드 ON
            g_isGoldBody = true;
            glColor3f(1.0f, 0.84f, 0.0f);   // 금색
            model.drawBody();               // BodyModel.draw() 호출
            g_isGoldBody = false;           // 끝나면 다시 OFF
            // 1) 혼합 색 활성화
            model.setFaceColor(
                resultSpirit.mixColorR,
                resultSpirit.mixColorG,
                resultSpirit.mixColorB
            );

            // 3) 얼굴 렌더링 (Face_xxx.cpp에서 g_isMixedFace 플래그 읽어서 색 덮어씌움)
            glPushMatrix();
            glScalef(0.5f, 0.5f, 0.5f);
            glTranslatef(0.0f, 1.2f, 0.0f);

            model.drawFace(resultSpirit.faceBase);

            glPopMatrix();

            // 4) 끝나면 혼합 색 비활성화 (PURE 정령에 영향 안 가도록)
            model.clearFaceColor();

            glDisable(GL_COLOR_MATERIAL);
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

    // 혼합 플래그 리셋
    resultSpirit.mixR = resultSpirit.mixG = resultSpirit.mixB = false;

    // 필드 정령과 같은 높이 기준 사용
    resultSpirit.yOffset = 1.0f;

    // 플레이어 앞 2m 지점
    resultSpirit.x = px + dirX * 2.0f;
    resultSpirit.z = pz + dirZ * 2.0f;

    int R = Rcount;
    int G = Gcount;
    int B = Bcount;

    // nonZero 개수로 PURE / MIXED 판정
    int nonZero = (R > 0) + (G > 0) + (B > 0);

    // RGB 합 (혼합 색 비율 계산용)
    float total = static_cast<float>(R + G + B);
    if (total <= 0.0f) total = 1.0f;

    resultSpirit.mixColorR = R / total;
    resultSpirit.mixColorG = G / total;
    resultSpirit.mixColorB = B / total;

    // 아무 것도 못 먹었을 때: 기본 RED 하나 뽑아주기
    if (nonZero == 0) {
        resultSpirit.type = SpiritType::RED_SPIRIT;
        resultSpirit.faceBase = SpiritType::RED_SPIRIT;
        resultSpirit.mixColorR = 1.0f;
        resultSpirit.mixColorG = 0.0f;
        resultSpirit.mixColorB = 0.0f;
        return;
    }

    // PURE: 한 색만 존재 (단일 정령)
    if (nonZero == 1) {
        if (R > 0) {
            resultSpirit.type = SpiritType::RED_SPIRIT;
        }
        else if (G > 0) {
            resultSpirit.type = SpiritType::GREEN_SPIRIT;
        }
        else { // B > 0
            resultSpirit.type = SpiritType::BLUE_SPIRIT;
        }
        // PURE 는 faceBase 사용 안 하고, 기본 색과 모델 그대로 사용
        return;
    }

    // MIXED: 두 색 이상 존재 (특별 정령)
    resultSpirit.type = SpiritType::MIXED_SPIRIT;

    // 가장 강한 색 찾기
    int maxRGB = std::max({ R, G, B });

    // 가장 큰 값과 같은 색 후보들 모으기 (동률 처리용)
    std::vector<SpiritType> candidates;
    if (R == maxRGB) candidates.push_back(SpiritType::RED_SPIRIT);
    if (G == maxRGB) candidates.push_back(SpiritType::GREEN_SPIRIT);
    if (B == maxRGB) candidates.push_back(SpiritType::BLUE_SPIRIT);

    // 후보가 1개면 그대로, 2~3개면 랜덤
    if (candidates.size() == 1) {
        resultSpirit.faceBase = candidates[0];
    } else {
        int idx = rand() % candidates.size();
        resultSpirit.faceBase = candidates[idx];
    }
}
