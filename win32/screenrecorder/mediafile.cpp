
#include "framework.h"
#include "mediafile.h"

// constructor
CMediaFile::CMediaFile()
{
	dwStreamIndex = 0;
	pSinkWriter = NULL;
}

// destructor
CMediaFile::~CMediaFile()
{

}

// Set input media format (RGB).
HRESULT CMediaFile::SetInputFormat(IMFMediaType** ppMediaType, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator)
{
	*ppMediaType = NULL;

	IMFMediaType* pMediaType = NULL;

	// Creates an empty media type.
	HRESULT hr = MFCreateMediaType(&pMediaType);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, width, height);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, numerator, denominator);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeRatio(pMediaType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppMediaType = pMediaType;
		(*ppMediaType)->AddRef();
	}

	if (pMediaType != NULL) pMediaType->Release();

	return hr;
}

// Set target media format (MP4).
HRESULT CMediaFile::SetTargetFormat(IMFMediaType** ppMediaType, UINT32 width, UINT32 height, UINT32 numerator, UINT32 denominator, UINT32 nAvgBitrate)
{
	*ppMediaType = NULL;

	IMFMediaType* pMediaType = NULL;

	HRESULT hr = MFCreateMediaType(&pMediaType);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AVG_BITRATE, nAvgBitrate);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, width, height);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, numerator, denominator);

	if (SUCCEEDED(hr))
		hr = MFSetAttributeRatio(pMediaType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppMediaType = pMediaType;
		(*ppMediaType)->AddRef();
	}

	if (pMediaType != NULL) pMediaType->Release();

	return hr;
}

HRESULT CMediaFile::Open(wchar_t* filename, UINT32 width, UINT32 height, LONG stride, UINT32 numerator, UINT32 denominator, UINT32 nAvgBitrate)
{
	IMFMediaType* pInputMediaType = NULL;
	IMFMediaType* pTargetMediaType = NULL;
	HRESULT hr = S_OK;

	this->width = width;
	this->height = height;
	this->stride = stride;

	// Creates the sink writer from a URL.
	hr = MFCreateSinkWriterFromURL(filename, NULL, NULL, &pSinkWriter);

	// Set input media format (RGB).
	if (SUCCEEDED(hr))
		hr = SetTargetFormat(&pTargetMediaType, width, height, numerator, denominator, nAvgBitrate);

	// Adds a stream to the sink writer.
	// This media type specifies the format of the samples that will be written to the file.
	// It does not need to match the input format.
	if (SUCCEEDED(hr))
		hr = pSinkWriter->AddStream(pTargetMediaType, &dwStreamIndex);

	// Set target media format (MP4).
	if (SUCCEEDED(hr))
		hr = SetInputFormat(&pInputMediaType, width, height, numerator, denominator);

	// Sets the input format for a stream on the sink writer.
	if (SUCCEEDED(hr))
		hr = pSinkWriter->SetInputMediaType(dwStreamIndex, pInputMediaType, NULL);

	// Release the interfaces.
	if (pInputMediaType != NULL) pInputMediaType->Release();
	if (pTargetMediaType != NULL) pTargetMediaType->Release();

	// Initializes the sink writer for writing.
	if (SUCCEEDED(hr))
		hr = pSinkWriter->BeginWriting();

	return hr;
}

HRESULT CMediaFile::Write(BYTE* buffer, DWORD buffersize, LONGLONG nSampleTime, LONGLONG nSampleDuration)
{
	IMFSample* pSample = NULL;
	IMFMediaBuffer* pBuffer = NULL;
	BYTE* data = NULL;

	// Allocates system memory and creates a media buffer to manage it.
	HRESULT hr = MFCreateMemoryBuffer(buffersize, &pBuffer);

	// Gives the caller access to the memory in the buffer, for reading or writing
	if (SUCCEEDED(hr))
		hr = pBuffer->Lock(&data, NULL, NULL);

	// Copies an image or image plane from one buffer (buffer) to another (data).
	if (SUCCEEDED(hr))
		hr = MFCopyImage(data, stride, buffer, stride, stride, height);

	// Unlocks a buffer that was previously locked.
	if (pBuffer)
		pBuffer->Unlock();

	// Sets the length of the valid data in the buffer.
	if (SUCCEEDED(hr))
		hr = pBuffer->SetCurrentLength(buffersize);

	// Creates an empty media sample.
	if (SUCCEEDED(hr))
		hr = MFCreateSample(&pSample);

	// Adds a buffer to the end of the list of buffers in the sample.
	if (SUCCEEDED(hr))
		hr = pSample->AddBuffer(pBuffer);

	// Sets the presentation time of the sample.
	if (SUCCEEDED(hr))
		hr = pSample->SetSampleTime(nSampleTime);

	// Sets the duration of the sample.
	if (SUCCEEDED(hr))
		hr = pSample->SetSampleDuration(nSampleDuration);

	// Delivers a sample to the sink writer.
	if (SUCCEEDED(hr))
		hr = pSinkWriter->WriteSample(dwStreamIndex, pSample);

	// Release the interfaces.
	if (pSample != NULL) pSample->Release();
	if (pBuffer != NULL) pBuffer->Release();

	return hr;
}

HRESULT CMediaFile::Close()
{
	HRESULT hr = S_OK;

	// Completes all writing operations on the sink writer.
	hr = pSinkWriter->Finalize();

	if (pSinkWriter != NULL)
	{
		pSinkWriter->Release();
		pSinkWriter = NULL;
	}

	return hr;
}
