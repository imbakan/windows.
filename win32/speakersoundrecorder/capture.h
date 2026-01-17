
// Capturing a Stream
// https://learn.microsoft.com/en-us/windows/win32/coreaudio/capturing-a-stream

#include "mediafile.h"

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
	IMMDeviceEnumerator* pMMDeviceEnumerator;
	IMMDevice* pMMDevice;
	IAudioCaptureClient* pAudioCaptureClient;
	IAudioClient* pAudioClient;
	WAVEFORMATEX* pwfx;

public:
	CCapture();
	~CCapture();

	HRESULT Open(WAVEFORMATEX* wfx);
	HRESULT Read(CMediaFile* file, LONGLONG* timestamp);
	HRESULT Close();

};
