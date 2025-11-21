#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : id(0) {}

Texture::~Texture() {
    if (id) glDeleteTextures(1, &id);
}

bool Texture::loadFromFile(const std::string& path) {
    int w, h, n;

    unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    GLenum format = (n == 4 ? GL_RGBA : (n == 3 ? GL_RGB : GL_RED));

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        w, h,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);

    std::cout << "Loaded texture: " << path
        << " (" << w << "x" << h << ", " << n << " channels)" << std::endl;

    return true;
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}
