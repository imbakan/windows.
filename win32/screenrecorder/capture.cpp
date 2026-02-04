
// i-capture ang screen gamit ang DirectX

#include "framework.h"
#include "capture.h"

CCapture::CCapture()
{

}

CCapture::~CCapture()
{

}

// mode.Width = 1920
// mode.Height = 1080
// rect.Pitch = 7680
// rect.Pitch / mode.Width = 7680 / 1920 = 4
// rect.Pitch % mode.Width = 7680 % 1920 = 0
//
HRESULT CCapture::Open(UINT32* width, UINT32* height, LONG* stride, UINT* buffersize)
{
	HRESULT hr = S_OK;
	D3DDISPLAYMODE mode;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	D3DLOCKED_RECT rect;

	// Create an IDirect3D9 object and return an interface to it.
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	// Retrieves the current display mode of the adapter.
	if (d3d != nullptr)
		hr = d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = mode.Height;
	parameters.BackBufferWidth = mode.Width;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;

	// Creates a device to represent the display adapter.
	if (SUCCEEDED(hr))
		hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &device);

	// Create an off-screen surface.
	if (SUCCEEDED(hr))
		hr = device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);

	// Locks a rectangle on a surface.
	if (SUCCEEDED(hr))
		hr = surface->LockRect(&rect, NULL, 0);

	// Unlocks a rectangle on a surface.
	if (SUCCEEDED(hr))
		hr = surface->UnlockRect();

	// Copy the following.
	if (SUCCEEDED(hr))
	{
		*width = mode.Width;
		*height = mode.Height;

		*stride = rect.Pitch;

		this->height = mode.Height;
		this->stride = rect.Pitch;

		this->buffersize = rect.Pitch * mode.Height;
		*buffersize = this->buffersize;

		buffer = new BYTE[this->buffersize];
	}

	return hr;
}

HRESULT CCapture::Read(BYTE** buffer)
{
	HRESULT hr;
	D3DLOCKED_RECT rect;
	int i, i1, i2;

	// Generates a copy of the device's front buffer and places that copy in a system memory buffer provided by the application.
	hr = device->GetFrontBufferData(0, surface);

	// Locks a rectangle on a surface.
	if (SUCCEEDED(hr))
		hr = surface->LockRect(&rect, NULL, 0);

	// Flip the frame vertically.
	if (SUCCEEDED(hr))
	{
		i1 = 0;
		i2 = buffersize - stride;

		for (i = 0; i < height; i++)
		{
			CopyMemory(&this->buffer[i1], ((BYTE*)rect.pBits + i2), stride);

			i1 += stride;
			i2 -= stride;
		}
	}

	// Unlocks a rectangle on a surface.
	if (SUCCEEDED(hr))
		hr = surface->UnlockRect();

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
		*buffer = this->buffer;

	return hr;
}

HRESULT CCapture::Close()
{
	SafeRelease(&surface);
	SafeRelease(&device);
	SafeRelease(&d3d);

	return S_OK;
}
