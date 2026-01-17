
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

// Set input media format (PCM).
HRESULT CMediaFile::SetInputFormat(IMFMediaType** ppMediaType, WORD nChannels, DWORD nSamplesPerSec, WORD nBlockAlign, DWORD nAvgBytesPerSec, WORD wBitsPerSample)
{
	*ppMediaType = NULL;

	IMFMediaType* pMediaType = NULL;

	// Creates an empty media type.
	HRESULT hr = MFCreateMediaType(&pMediaType);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, nChannels);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, nSamplesPerSec);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, nBlockAlign);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, nAvgBytesPerSec);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, wBitsPerSample);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppMediaType = pMediaType;
		(*ppMediaType)->AddRef();
	}

	if (pMediaType != NULL) pMediaType->Release();

	return hr;
}

// Set target media format (AAC).
HRESULT CMediaFile::SetTargetFormat(IMFMediaType** ppMediaType, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	*ppMediaType = NULL;

	IMFMediaType* pMediaType = NULL;

	HRESULT hr = MFCreateMediaType(&pMediaType);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, wBitsPerSample);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, nSamplesPerSec);

	if (SUCCEEDED(hr))
		hr = pMediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, nChannels);

	// Return the pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppMediaType = pMediaType;
		(*ppMediaType)->AddRef();
	}

	if (pMediaType != NULL) pMediaType->Release();

	return hr;
}

HRESULT CMediaFile::Open(wchar_t* filename, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	IMFMediaType* pInputMediaType = NULL;
	IMFMediaType* pTargetMediaType = NULL;
	HRESULT hr = S_OK;

	WORD nBlockAlign = nChannels * (wBitsPerSample / 8);
	DWORD nAvgBytesPerSec = nBlockAlign * nSamplesPerSec;

	// Creates the sink writer from a URL.
	hr = MFCreateSinkWriterFromURL(filename, NULL, NULL, &pSinkWriter);

	// Set input media format (RGB).
	if (SUCCEEDED(hr))
		hr = SetTargetFormat(&pTargetMediaType, nChannels, nSamplesPerSec, wBitsPerSample);

	// Adds a stream to the sink writer.
	// This media type specifies the format of the samples that will be written to the file.
	// It does not need to match the input format.
	if (SUCCEEDED(hr))
		hr = pSinkWriter->AddStream(pTargetMediaType, &dwStreamIndex);

	// Set target media format (MP4).
	if (SUCCEEDED(hr))
		hr = SetInputFormat(&pInputMediaType, nChannels, nSamplesPerSec, nBlockAlign, nAvgBytesPerSec, wBitsPerSample);

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

	// Copies audio block from one buffer (buffer) to another (data).
	if (SUCCEEDED(hr))
		memcpy(data, buffer, buffersize);

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
