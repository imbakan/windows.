
#pragma once

class CMediaFile
{
private:
	IMFSinkWriter* pSinkWriter;
	DWORD dwStreamIndex;
	int width, height, stride;

	HRESULT SetInputFormat(IMFMediaType** ppMediaType, WORD nChannels, DWORD nSamplesPerSec, WORD nBlockAlign, DWORD nAvgBytesPerSec, WORD wBitsPerSample);
	HRESULT SetTargetFormat(IMFMediaType** ppMediaType, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample);

public:
	CMediaFile();
	~CMediaFile();

	HRESULT Open(wchar_t* filename, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample);
	HRESULT Write(BYTE* buffer, DWORD buffersize, LONGLONG nSampleTime, LONGLONG nSampleDuration);
	HRESULT Close();
};

