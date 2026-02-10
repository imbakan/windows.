
// https://gist.github.com/jeroen/10eb17a9fb0e5799b772
// http://www.libpng.org/pub/png/libpng-manual.txt

#pragma once

class CPngFile
{
private:
	unsigned char* buffer;
	unsigned int width, height;

public:
	CPngFile();
	~CPngFile();

	bool Open(wchar_t* filename);
	void Read(unsigned int* width, unsigned int* height, unsigned char** buffer);
	void Close();

	bool Save(wchar_t* filename, size_t width, size_t height, unsigned char* buffer);
};
