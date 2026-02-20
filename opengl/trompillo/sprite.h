
// GL_VERSION_1_5 GL_VERSION_2_0 GL_VERSION_3_0

#pragma once

class CSprite
{
private:
	int location1, location2;
	int count;
	GLuint vao;

public:
	CSprite();
	~CSprite();

	void Create(GLuint handle);
	void Destroy();
	void Draw(float* matrix);
};
