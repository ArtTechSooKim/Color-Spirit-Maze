#pragma once
#include <GL/glut.h>
#include <string>   // std::string 사용을 위해

class Texture {
public:
    Texture();
    ~Texture();

    // 파일에서 텍스처 로드 (Maze.cpp에서 사용하는 이름과 맞춤)
    bool loadFromFile(const std::string& path);

    // 바인딩/ID 얻기
    void bind() const;
    GLuint getID() const { return id; }

private:
    GLuint id;
};