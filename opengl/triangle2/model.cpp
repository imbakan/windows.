
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

void CModel::Generate(int vertex_size, int color_size, int* stride, float** vertices, int* size, int* count)
{
    int i, k, allocsize;
    double a, angle, radius;
    float x, y, z, c[3];

    *count = 3;
    allocsize = (*count) * (vertex_size + color_size);
    *vertices = new float[allocsize];

    *stride = (vertex_size + color_size) * sizeof(float);
    *size = (*count) * (*stride);

    //           0
    //           +
    //          / \
    //         /   \
    //        /     \
    //       /       \
    //      +---------+
    //     1           2

    z = 0.0;
    c[0] = c[1] = c[2] = 0.0f;

    radius = 20.0;
    a = 90.0;

    k = 0;
    z = 0.0f;

    for (i = 0; i < (*count); i++) {

        angle = a / 180.0 * M_PI;

        x = (float)(radius * cos(angle));
        y = (float)(radius * sin(angle));

        c[i] = 1.0f;

        (*vertices)[k++] = x;
        (*vertices)[k++] = y;
        (*vertices)[k++] = z;

        (*vertices)[k++] = c[0];
        (*vertices)[k++] = c[1];
        (*vertices)[k++] = c[2];
        (*vertices)[k++] = 1.0f;

        c[i] = 0.0f;

        a += 120.0;
    }
}

void CModel::Create(GLuint handle)
{
    float* vertices = NULL;
    int vertex_size, color_size, stride, size, vertex_index, color_index;
    long long vertex_offset, color_offset;
    GLuint vbo;

    vertex_size = 3;
    color_size = 4;

    vertex_offset = 0LL;
    color_offset = vertex_size * sizeof(float);

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    color_index = glGetAttribLocation(handle, "v_color");
    location = glGetUniformLocation(handle, "m_matrix");

    Generate(vertex_size, color_size, &stride, &vertices, &size, &count);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(vertex_index, vertex_size, GL_FLOAT, GL_FALSE, stride, (const void*)vertex_offset);
    glEnableVertexAttribArray(vertex_index);

    glVertexAttribPointer(color_index, color_size, GL_FLOAT, GL_FALSE, stride, (const void*)color_offset);
    glEnableVertexAttribArray(color_index);

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
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}
