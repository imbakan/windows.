
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#pragma once

class CCircle1
{
private:
	int location1, location2;
	float color[4];
	int count;
	GLuint vao;

	void Generate(int vertex_size, int* stride, float** vertices, int* size, int* count, double radius);

public:
	CCircle1();
	~CCircle1();

	void Create(GLuint handle, double radius, float* color);
	void Destroy();
	void Draw(float* matrix);
};
