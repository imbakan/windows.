
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "sprite.h"

CSprite::CSprite()
{
    vao = 0;
}

CSprite::~CSprite()
{

}

void CSprite::Create(GLuint handle)
{
    float* vertices = NULL;
    int k, allocsize, vertex_size, stride, size, vertex_index;
    long long vertex_offset;
    GLuint vbo;

    vertex_size = 3;
    vertex_offset = 0LL;

    count = 1;
    allocsize = count * vertex_size;
    vertices = new float[allocsize];

    stride = vertex_size * sizeof(float);
    size = count * stride;

    k = 0;

    vertices[k++] = 0.0f;
    vertices[k++] = 0.0f;
    vertices[k++] = 0.0f;

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    location1 = glGetUniformLocation(handle, "m_matrix");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(vertex_index, vertex_size, GL_FLOAT, GL_FALSE, stride, (const void*)vertex_offset);
    glEnableVertexAttribArray(vertex_index);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);

    delete[] vertices;
}

void CSprite::Destroy()
{
    glDeleteVertexArrays(1, &vao);
}

void CSprite::Draw(float* matrix)
{
    glUniformMatrix4fv(location1, 1, false, matrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, count);
    glBindVertexArray(0);
}
