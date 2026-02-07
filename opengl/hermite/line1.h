
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#pragma once

class CLine1
{
private:
	int location1, location2;
	float color[4];
	int count, vertex_size;
	GLuint vao, vbo;

public:
	CLine1();
	~CLine1();

	void Create(GLuint handle, float* color);
	void Destroy();
	void Draw(float* matrix);

	void Update(float* vertices, int count, int size);
	void Map(float x, float y, int index);
	void Map(float x1, float y1, float x2, float y2, int index);

};
