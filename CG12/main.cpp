#include <GL/glew.h>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Header.h"

Image* loadImage(const char* filePath);

int main() {
    sf::Window window(sf::VideoMode(800, 800), "Lab12", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    glewInit();
    Init();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Num1:
                    drawMode = 0;
                    InitShader();
                    InitTetrahedron();
                    break;
                case sf::Keyboard::Num2:
                    drawMode = 1;
                    InitShader();
                    InitHexahedron();
                    break;
                case sf::Keyboard::Num3:
                    drawMode = 2;
                    InitShader();
                    InitHexahedron();
                    break;
                case sf::Keyboard::Num4:
                    drawMode = 3;
                    InitShader();
                    InitGradientCircle();
                    break;
                case sf::Keyboard::W: offsetY += offsetStep; break;
                case sf::Keyboard::S: offsetY -= offsetStep; break;
                case sf::Keyboard::A: offsetX -= offsetStep; break;
                case sf::Keyboard::D: offsetX += offsetStep; break;
                case sf::Keyboard::Q: offsetZ += offsetStep; break;
                case sf::Keyboard::E: offsetZ -= offsetStep; break;
                case sf::Keyboard::Up: angleY -= angleStep; break;
                case sf::Keyboard::Down: angleY += angleStep; break;
                case sf::Keyboard::Left: angleX -= angleStep; break;
                case sf::Keyboard::Right: angleX += angleStep; break;
                case sf::Keyboard::V: angleZ -= angleStep; break;
                case sf::Keyboard::B: angleZ += angleStep; break;
                case sf::Keyboard::I: scaleY += scaleStep; break;
                case sf::Keyboard::K: scaleY -= scaleStep; break;
                case sf::Keyboard::J: scaleX -= scaleStep; break;
                case sf::Keyboard::L: scaleX += scaleStep; break;
                case sf::Keyboard::X: proportion = (proportion + 0.05f < 1.0f) ? proportion + 0.05f : 1.0f; break; // Пропорция смешивания
                case sf::Keyboard::C: proportion = (proportion - 0.05f > 0.0f) ? proportion - 0.05f : 0.0f; break; // Пропорция смешивания
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }

    Release();
    return 0;
}


void Init() {
    glEnable(GL_DEPTH_TEST);
    InitShader();
    InitTetrahedron();
}

void InitShader() {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vShader);
    ShaderLog(vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (drawMode == 1)
        glShaderSource(fShader, 1, &FragShaderTextureColor, NULL);
    else if (drawMode == 2)
        glShaderSource(fShader, 1, &FragShaderTextureMix, NULL);
    else
        glShaderSource(fShader, 1, &FragShaderSource, NULL);

    glCompileShader(fShader);
    ShaderLog(fShader);

    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    glLinkProgram(Program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    Attrib_vertex = glGetAttribLocation(Program, "coord");
    Attrib_color = glGetAttribLocation(Program, "color");
    Attrib_texture = glGetAttribLocation(Program, "texCoord");
}

void InitTetrahedron() {
    std::vector<Vertex> vertices = {
        {  0.0f,   0.5f,  0.0f,  0.0f,  0.0f,  1.0f }, // Верхняя вершина
        { -0.5f,  -0.5f,  0.0f,  1.0f,  0.0f,  0.0f }, // Левая нижняя вершина 
        {  0.5f,  -0.5f,  0.0f,  0.0f,  1.0f,  0.0f }, // Правая нижняя вершина
        {  0.0f,  -0.5f,  -0.5f, 1.0f,  1.0f,  1.0f }  // Задняя нижняя вершина 
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 2, 3
    };

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(Attrib_vertex);

    glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(Attrib_color);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InitHexahedron()
{
    std::vector<Vertex> vertices = {
        // Верхняя
        {  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        // Нижняя 
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        // Задняя 
        {  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        // Передняя
        {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
        // Правая
        {  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        {  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        {  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
        // Левая
        { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        { -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
        { -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
    };

    GLuint indices[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15,
        16, 17, 18, 19,
        20, 21, 22, 23
    };

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(Attrib_vertex);

    glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(Attrib_color);

    glVertexAttribPointer(Attrib_texture, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(Attrib_texture);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void InitGradientCircle() {
    const int segments = 100;
    const float radius = 0.5f;
    const float PI = 3.14159265359f;

    std::vector<Vertex> vertices;
    vertices.push_back({ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f });

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        float hue = static_cast<float>(i) / segments;
        float r, g, b;
        HSVtoRGB(hue, 1.0f, 1.0f, r, g, b);

        vertices.push_back({ x, y, 0.0f, r, g, b });
    }

    std::vector<GLuint> indices;
    for (int i = 1; i < segments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(segments);
    indices.push_back(1);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(Attrib_vertex);

    glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(Attrib_color);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    float p, q, t, ff;
    int i;
    if (s <= 0.0f) {
        r = g = b = v;
        return;
    }
    h *= 6.0f;
    i = static_cast<int>(h);
    ff = h - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * ff));
    t = v * (1.0f - (s * (1.0f - ff)));

    switch (i) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
    }
}


void Draw() {
    glUseProgram(Program);
    glBindVertexArray(VAO);

    float translationMatrix[16];
    createTranslationMatrix(offsetX, offsetY, offsetZ, translationMatrix);
    GLint translationMatrixLocation = glGetUniformLocation(Program, "translationMatrix");
    glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, translationMatrix);

    float scaleMatrix[16];
    createScaleMatrix(scaleX, scaleY, scaleMatrix);
    GLint scaleMatrixLocation = glGetUniformLocation(Program, "scaleMatrix");
    glUniformMatrix4fv(scaleMatrixLocation, 1, GL_FALSE, scaleMatrix);

    float rotationX[16], rotationY[16], rotationZ[16], combinedRotation[16], tempMatrix[16];
    rotateX(angleX * M_PI / 180.0f, rotationX);
    rotateY(angleY * M_PI / 180.0f, rotationY);
    rotateZ(angleZ * M_PI / 180.0f, rotationZ);
    multiplyMatrices(rotationY, rotationX, tempMatrix); // Y * X
    multiplyMatrices(rotationZ, tempMatrix, combinedRotation); // Z * (Y * X)
    GLint rotMatrixLoc = glGetUniformLocation(Program, "rotationMatrix");
    glUniformMatrix4fv(rotMatrixLoc, 1, GL_TRUE, combinedRotation);

    if (drawMode == 1)
    {
        DrawHexahedronTextureColor();
    }
    else if (drawMode == 2)
    {
        DrawHexahedronTextureTexture();
    }
    else
    {
        glDrawElements(GL_TRIANGLES, 300, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}


void DrawHexahedronTextureColor()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Image* photo_img = loadImage("stesha.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, photo_img->sizeX, photo_img->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, photo_img->data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint textureLocation = glGetUniformLocation(Program, "ourTexture");
    glUniform1i(textureLocation, 0);

    glUniform1f(glGetUniformLocation(Program, "proportion"), proportion);

    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);

    if (photo_img != nullptr)
        freeImage(photo_img);

    glDeleteTextures(1, &texture);
}

void DrawHexahedronTextureTexture()
{
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Image* photo_img1 = loadImage("stesha.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, photo_img1->sizeX, photo_img1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, photo_img1->data);

    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Image* photo_img2 = loadImage("tisha.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, photo_img2->sizeX, photo_img2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, photo_img2->data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUniform1i(glGetUniformLocation(Program, "ourTexture1"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(Program, "ourTexture2"), 1);

    glUniform1f(glGetUniformLocation(Program, "proportion"), proportion);

    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, 0);

    if (photo_img1 != nullptr)
        freeImage(photo_img1);
    if (photo_img2 != nullptr)
        freeImage(photo_img2);

    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
}

void Release() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(Program);
}

void ShaderLog(GLuint shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
}

Image* loadImage(const char* filePath) {
    Image* img = new Image();

    int channels;
    img->data = stbi_load(filePath, &img->sizeX, &img->sizeY, &channels, 3);
    if (!img->data) {
        std::cerr << "Не удалось загрузить изображение: " << filePath << std::endl;
        delete img;
        return nullptr;
    }

    return img;
}

void freeImage(Image* img) {
    if (img) {
        stbi_image_free(img->data);
        delete img;
    }
}

// ========= Matrices =========

void createTranslationMatrix(float offsetX, float offsetY, float offsetZ, float* matrix) {
    float temp[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        offsetX, offsetY, offsetZ, 1.0f
    };
    std::copy(temp, temp + 16, matrix);
}

void createScaleMatrix(float scaleX, float scaleY, float* matrix) {
    float scaleMatrix[16] = {
        scaleX, 0.0f,   0.0f, 0.0f,
        0.0f,   scaleY, 0.0f, 0.0f,
        0.0f,   0.0f,   1.0f, 0.0f,
        0.0f,   0.0f,   0.0f, 1.0f
    };

    for (int i = 0; i < 16; ++i)
        matrix[i] = scaleMatrix[i];
}

void rotateX(float angle, float* matrix) {
    float cosA = cos(angle), sinA = sin(angle);
    float temp[16] = {
        cosA, 0, -sinA, 0,
        0,    1, 0,     0,
        sinA, 0, cosA,  0,
        0,    0, 0,     1
    };
    std::copy(temp, temp + 16, matrix);
}

void rotateY(float angle, float* matrix) {
    float cosA = cos(angle), sinA = sin(angle);
    float temp[16] = {
        1, 0,     0,     0,
        0, cosA,  sinA,  0,
        0, -sinA, cosA,  0,
        0, 0,     0,     1
    };
    std::copy(temp, temp + 16, matrix);
}

void rotateZ(float angle, float* matrix) {
    float cosA = cos(angle), sinA = sin(angle);
    float temp[16] = {
        cosA,  sinA,  0, 0,
        -sinA, cosA,  0, 0,
        0,     0,     1, 0,
        0,     0,     0, 1
    };
    std::copy(temp, temp + 16, matrix);
}

void multiplyMatrices(const float* a, const float* b, float* result) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; ++k)
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
        }
    }
}