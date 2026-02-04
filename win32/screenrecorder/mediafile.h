
#pragma once
class CMediaFile
{
private:
	IMFSinkWriter* pSinkWriter;
	DWORD dwStreamIndex;
	int width, height, stride;

	HRESULT SetInputFormat(IMFMediaType** ppMediaType, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator);
	HRESULT SetTargetFormat(IMFMediaType** ppMediaType, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator, UINT32 nAvgBitrate);

public:
	CMediaFile();
	~CMediaFile();

	HRESULT Open(wchar_t* filename, UINT32 width, UINT32 height, LONG stride, UINT32 numerator, UINT32 denominator, UINT32 nAvgBitrate);
	HRESULT Write(BYTE* buffer, DWORD buffersize, LONGLONG nSampleTime, LONGLONG nSampleDuration);
	HRESULT Close();
};

