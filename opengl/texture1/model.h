
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#pragma once

class CModel
{
private:
	int location;
	int count;
	GLuint vao;

	void Generate(int vertex_size, int texture_size, int* stride, float** vertices, int* size, int* count, float width, float height, float iwidth);

public:
	CModel();
	~CModel();

	void Create(GLuint handle, float width, float height, float iwidth);
	void Destroy();
	void Draw(float* matrix);
};
