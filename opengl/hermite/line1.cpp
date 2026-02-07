
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "line1.h"

CLine1::CLine1()
{
    vao = vbo = 0;
    vertex_size = 3;
}

CLine1::~CLine1()
{

}

void CLine1::Create(GLuint handle, float* color)
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

void CLine1::Destroy()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void CLine1::Draw(float* matrix)
{
    glUniformMatrix4fv(location1, 1, false, matrix);
    glUniform4fv(location2, 1, color);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, count);
    glBindVertexArray(0);
}

void CLine1::Update(float* vertices, int count, int size)
{
    this->count = count;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CLine1::Map(float x, float y, int index)
{
    float* vertices;
    int k;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    k = vertex_size * index;

    vertices[k++] = x;
    vertices[k++] = y;
    vertices[k] = 0.0f;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void CLine1::Map(float x1, float y1, float x2, float y2, int index)
{
    float* vertices;
    int k;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    k = vertex_size * index;

    vertices[k++] = x1;
    vertices[k++] = y1;
    vertices[k++] = 0.0f;

    vertices[k++] = x2;
    vertices[k++] = y2;
    vertices[k] = 0.0f;

    glUnmapBuffer(GL_ARRAY_BUFFER);
}
