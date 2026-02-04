
// i-capture ang screen gamit ang DirectX

#pragma once

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


class CCapture
{
private:
	BYTE* buffer;
	UINT buffersize;
	int height, stride;

	IDirect3D9* d3d;
	IDirect3DDevice9* device;
	IDirect3DSurface9* surface;

public:
	CCapture();
	~CCapture();

	HRESULT Open(UINT32* width, UINT32* height, LONG* stride, UINT* buffersize);
	HRESULT Read(BYTE** buffer);
	HRESULT Close();
};

