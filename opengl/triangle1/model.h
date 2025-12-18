
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#pragma once

class CModel
{
private:
	int location1, location2;
	int count;
	GLuint vao;

	void Generate(int vertex_size, int* stride, float** vertices, int* size, int* count);

public:
	CModel();
	~CModel();

	void Create(GLuint handle, float* color);
	void Destroy();
	void Draw(float* matrix);
};
