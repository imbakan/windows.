
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "line2.h"

CLine2::CLine2()
{
    vao = vbo = 0;
    vertex_size = 3;
}

CLine2::~CLine2()
{

}

void CLine2::Create(GLuint handle, float* color)
{
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
    this->color[3] = color[3];

    float* vertices = NULL;
    int stride, vertex_index;
    long long vertex_offset;

    vertex_offset = 0LL;
    stride = vertex_size * sizeof(float);

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    location1 = glGetUniformLocation(handle, "m_matrix");
    location2 = glGetUniformLocation(handle, "v_color");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(vertex_index, vertex_size, GL_FLOAT, GL_FALSE, stride, (const void*)vertex_offset);
    glEnableVertexAttribArray(vertex_index);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CLine2::Destroy()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void CLine2::Draw(float* matrix)
{
    glUniformMatrix4fv(location1, 1, false, matrix);
    glUniform4fv(location2, 1, color);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, count);
    glBindVertexArray(0);
}

void CLine2::Update(float* vertices, int count, int size)
{
    this->count = count;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CLine2::Verify()
{
    float* vertices;
    float x, y, z;
    int i, k;
    bool failed;
    char str[100];

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

    k = 0;
    failed = false;

    for (i = 0; i < count; i++) {

        x = vertices[k++];
        y = vertices[k++];
        z = vertices[k++];

        if (y > 1.0f) {
            failed = true;
            break;
        }
    }
     
    if (failed) {
        sprintf_s(str, 100, "(y > 1) %70.60f\n", y);
        OutputDebugStringA(str);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}
