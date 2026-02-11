
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "model.h"

CModel::CModel()
{
    vao = 0;
    opacity = 1.0f;
}

CModel::~CModel()
{

}

void CModel::Generate(int vertex_size, int texture_size, int* stride, float** vertices, int* size, int* count, float width, float height, float iwidth)
{
    float x1, y1, x2, y2, z, w, h, xo, yo;
    int k, allocsize;
    float s1, t1, s2, t2;

    *count = 4;
    allocsize = (*count) * (vertex_size + texture_size);
    *vertices = new float[allocsize];

    *stride = (vertex_size + texture_size) * sizeof(float);
    *size = (*count) * (*stride);

    w = iwidth;
    h = w * (height / width);

    xo = w / 2.0f;
    yo = h / 2.0f;

    x1 = -xo;
    y1 = -yo;

    x2 = xo;
    y2 = yo;

    z = 0.0f;

    //            t2 y2 +---------------------+
    //                  |                     |
    //                  |                     |
    //                  |                     |
    //            t1 y1 +---------------------+
    //                  x1                    x2
    //                  s1                    s2

    s1 = 0.0f;
    s2 = 1.0f;

    // para di baligtad ang image pagpalitin ito
    t1 = 1.0f;
    t2 = 0.0f;

    k = 0;

    // 0
    (*vertices)[k++] = x1;
    (*vertices)[k++] = y2;
    (*vertices)[k++] = z;
    (*vertices)[k++] = s1;
    (*vertices)[k++] = t2;

    // 1
    (*vertices)[k++] = x2;
    (*vertices)[k++] = y2;
    (*vertices)[k++] = z;
    (*vertices)[k++] = s2;
    (*vertices)[k++] = t2;

    // 2
    (*vertices)[k++] = x2;
    (*vertices)[k++] = y1;
    (*vertices)[k++] = z;
    (*vertices)[k++] = s2;
    (*vertices)[k++] = t1;

    // 3
    (*vertices)[k++] = x1;
    (*vertices)[k++] = y1;
    (*vertices)[k++] = z;
    (*vertices)[k++] = s1;
    (*vertices)[k++] = t1;
}

void CModel::Create(GLuint handle, float width, float height, float iwidth)
{
    float* vertices = NULL;
    int vertex_size, texture_size, stride, size, vertex_index, texture_index;
    int loc0, loc1;
    long long vertex_offset, texture_offset;
    GLuint vbo;

    loc0 = glGetUniformLocation(handle, "sampler0");
    loc1 = glGetUniformLocation(handle, "sampler1");

    glUniform1i(loc0, 0);
    glUniform1i(loc1, 1);

    vertex_size = 3;
    texture_size = 2;

    vertex_offset = 0LL;
    texture_offset = vertex_size * sizeof(float);

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    texture_index = glGetAttribLocation(handle, "v_texture");
    location1 = glGetUniformLocation(handle, "m_matrix");
    location2 = glGetUniformLocation(handle, "opacity");

    Generate(vertex_size, texture_size, &stride, &vertices, &size, &count, width, height, iwidth);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(vertex_index, vertex_size, GL_FLOAT, GL_FALSE, stride, (const void*)vertex_offset);
    glEnableVertexAttribArray(vertex_index);

    glVertexAttribPointer(texture_index, texture_size, GL_FLOAT, GL_FALSE, stride, (const void*)texture_offset);
    glEnableVertexAttribArray(texture_index);

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
    glUniformMatrix4fv(location1, 1, false, matrix);
    glUniform1f(location2, opacity);

    glBindVertexArray(vao);
    glDrawArrays(GL_QUADS, 0, count);
    glBindVertexArray(0);
}

void CModel::SetOpacity(float opacity)
{
    this->opacity = opacity;
}
