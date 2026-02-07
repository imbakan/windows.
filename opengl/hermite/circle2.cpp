
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#include "framework.h"
#include "circle2.h"

CCircle2::CCircle2()
{
    vao = 0;
}

CCircle2::~CCircle2()
{

}

void CCircle2::Generate(int vertex_size, int* stride, float** vertices, int* size, int* count, double radius)
{
    int i, k, n, allocsize;
    double a, angle, dpd;
    float x, y, z;

    n = 32;

    *count = n;
    allocsize = (*count) * vertex_size;
    *vertices = new float[allocsize];

    *stride = vertex_size * sizeof(float);
    *size = (*count) * (*stride);

    dpd = 360.0 / (double)n;

    k = 0;
    z = 0.0f;
    a = 0.0;

    for (i = 0; i < (*count); i++) {

        angle = a / 180.0 * M_PI;

        x = (float)(radius * cos(angle));
        y = (float)(radius * sin(angle));

        (*vertices)[k++] = x;
        (*vertices)[k++] = y;
        (*vertices)[k++] = z;

        a += dpd;
    }
}

void CCircle2::Create(GLuint handle, double radius, float* color)
{
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
    this->color[3] = color[3];

    float* vertices = NULL;
    int vertex_size, stride, size, vertex_index;
    long long vertex_offset;
    GLuint vbo;

    vertex_size = 3;
    vertex_offset = 0LL;

    vertex_index = glGetAttribLocation(handle, "v_vertex");
    location1 = glGetUniformLocation(handle, "m_matrix");
    location2 = glGetUniformLocation(handle, "v_color");

    Generate(vertex_size, &stride, &vertices, &size, &count, radius);

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

void CCircle2::Destroy()
{
    glDeleteVertexArrays(1, &vao);
}

void CCircle2::Draw(float* matrix)
{
    glUniformMatrix4fv(location1, 1, false, matrix);
    glUniform4fv(location2, 1, color);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    glBindVertexArray(0);
}
