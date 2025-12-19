
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "model.h"

CModel::CModel()
{
    vao = 0;
}

CModel::~CModel()
{

}

void CModel::Generate(int vertex_size, int* stride, float** vertices, int* size, int* count)
{
    int k, allocsize;
    float x1, y1, x2, y2, z, l, w;

    *count = 4;
    allocsize = (*count) * vertex_size;
    *vertices = new float[allocsize];

    *stride = vertex_size * sizeof(float);
    *size = (*count) * (*stride);

    //         0               1
    //      y2 +---------------+
    //         |               |
    //         |s              |
    //         |        s      |
    //      y1 +---------------+
    //         x1              x2
    //         2               3
    //

    l = 40.0f;
    w = 15.0f;

    x1 = -l / 2.0f;
    x2 = x1 + l;

    y1 = -w / 2.0f;
    y2 = y1 + w;

    z = 0.0f;

    k = 0;

    // 0
    (*vertices)[k++] = x1;
    (*vertices)[k++] = y2;
    (*vertices)[k++] = z;

    // 1
    (*vertices)[k++] = x2;
    (*vertices)[k++] = y2;
    (*vertices)[k++] = z;

    // 3
    (*vertices)[k++] = x2;
    (*vertices)[k++] = y1;
    (*vertices)[k++] = z;

    // 2
    (*vertices)[k++] = x1;
    (*vertices)[k++] = y1;
    (*vertices)[k++] = z;
}

void CModel::Create(GLuint handle)
{
    float* vertices = NULL;
    int vertex_size, stride, size, vertex_index;
    long long vertex_offset;
    GLuint vbo;

    vertex_size = 3;
    vertex_offset = 0LL;

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    location = glGetUniformLocation(handle, "m_matrix");

    Generate(vertex_size, &stride, &vertices, &size, &count);

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

    if (vertices != NULL) delete[] vertices;
}

void CModel::Destroy()
{
    glDeleteVertexArrays(1, &vao);
}

void CModel::Draw(float* matrix)
{
    glUniformMatrix4fv(location, 1, false, matrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_QUADS, 0, count);
    glBindVertexArray(0);
}
