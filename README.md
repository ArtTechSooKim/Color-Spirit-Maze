🎮 Color Spirit Maze

OpenGL 기반 3D 미로 탐험 게임으로, RGB 정령을 수집하여 최종 색 조합을 만드는 Computer Graphics 기말 프로젝트입니다.
카메라 컨트롤, 미로 충돌 처리, SOR 모델링, 텍스처 매핑 등 그래픽스 핵심 기능을 직접 구현했습니다.

🧩 게임 개요

플레이 방식:
WASD 이동 + 마우스 드래그로 카메라 회전

목표:
미로 곳곳에 배치된 R/G/B 정령(SOR 모델)을 수집해 최종 색을 완성

특징:

OpenGL 기반 3D 렌더링

플레이어 시점 이동 + 마우스 카메라

실제 미로 충돌 처리

정령(SOR) 모델링 및 수집 로직

텍스처 매핑 및 간단한 그림자 구현

⚙️ 기술 스택

C++

OpenGL / GLUT

Visual Studio 2022

SOR(Surface of Revolution) 기반 모델링

Git / GitHub 협업

📁 프로젝트 구조
/src
 ├── main.cpp
 ├── Camera.cpp / Camera.h
 ├── Maze.cpp   / Maze.h
 ├── SOR.cpp    / SOR.h
 └── textures/  (바닥, 벽 텍스처 예정)

/docs
 └── report/ (보고서)

👥 팀 구성

김수 — 프로젝트 리드, 미로/카메라/SOR 기반 구현

김주은 — 미로 충돌 처리 및 텍스처 매핑

최원준 — SOR 확장, 정령 배치/수집 시스템

팀 전체 — 보고서 + 발표 자료 공동 제작

🚀 실행 방법

Visual Studio로 솔루션 열기

freeglut 라이브러리 세팅

Debug → Local Windows Debugger 실행

🎯 기말 프로젝트 마일스톤

 1차 : 카메라 + 미로 렌더링 + 기본 SOR 모델

 2차 : 충돌 처리, 텍스처 매핑

 3차 : RGB 정령 시스템 + UI

 4차 : 보고서 및 발표 준비