#ifndef HEADER
#define HEADER

GLuint Program;
GLint Attrib_vertex, Attrib_color, Attrib_texture;
GLuint VBO, VAO, EBO;
int drawMode = 0;

float offsetX = 0.0f, offsetY = 0.0f, offsetZ = 0.0f;
float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;
const float offsetStep = 0.1f;
const float angleStep = 5.0f;
const float M_PI = 2 * acos(0.0);
float scaleX = 1.0f, scaleY = 1.0f;
const float scaleStep = 0.1f;
float proportion = 0.5f;

struct Vertex {
    GLfloat x, y, z;
    GLfloat r, g, b;
    GLfloat s, t;
};

struct Image {
    int sizeX;
    int sizeY;
    unsigned char* data;
};


const char* VertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 coord;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;
out vec3 vertexColor;
out vec2 TexCoord;
uniform mat4 translationMatrix;
uniform mat4 rotationMatrix;
uniform mat4 scaleMatrix;
void main() {
    vertexColor = color;
    TexCoord = texCoord;
    gl_Position = translationMatrix * rotationMatrix * scaleMatrix * vec4(coord, 1.0);
}
)";

const char* FragShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 color;
void main() {
    color = vec4(vertexColor, 1.0);
}
)";

const char* FragShaderTextureColor = R"(
#version 330 core
in vec3 vertexColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
uniform float proportion;
void main() {
    color = mix(texture(ourTexture, TexCoord), vec4(vertexColor, 1.0), proportion);
}
)";

const char* FragShaderTextureMix = R"(
#version 330 core
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float proportion;
void main() {
    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), proportion);
}
)";

void Init();
void InitShader();
void InitTetrahedron();
void InitHexahedron();
void InitGradientCircle();
void Draw();
void DrawHexahedronTextureColor();
void DrawHexahedronTextureTexture();
void Release();
void ShaderLog(GLuint shader);
void createTranslationMatrix(float offsetX, float offsetY, float offsetZ, float* matrix);
void createScaleMatrix(float scaleX, float scaleY, float* matrix);
void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);
void rotateY(float angle, float* matrix);
void rotateX(float angle, float* matrix);
void rotateZ(float angle, float* matrix);
void multiplyMatrices(const float* a, const float* b, float* result);
void freeImage(Image* img);

#endif // !HEADER

