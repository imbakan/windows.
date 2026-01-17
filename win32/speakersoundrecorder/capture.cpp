
// Capturing a Stream
// https://learn.microsoft.com/en-us/windows/win32/coreaudio/capturing-a-stream

#include "framework.h"
#include "capture.h"

CCapture::CCapture()
{
    pMMDeviceEnumerator = NULL;
    pMMDevice = NULL;
    pAudioCaptureClient = NULL;
    pAudioClient = NULL;

    pwfx = NULL;
}

CCapture::~CCapture()
{

}

HRESULT CCapture::Open(WAVEFORMATEX* wfx)
{
    HRESULT hr = S_OK;
    PWAVEFORMATEXTENSIBLE pWFX;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pMMDeviceEnumerator);

    if (SUCCEEDED(hr))
        hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pMMDevice);

    if (SUCCEEDED(hr))
        hr = pMMDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);

    if (SUCCEEDED(hr))
        hr = pAudioClient->GetMixFormat(&pwfx);

    if (SUCCEEDED(hr)) {

        switch (pwfx->wFormatTag) {

        case WAVE_FORMAT_IEEE_FLOAT:
            OutputDebugString(L"WAVE_FORMAT_IEEE_FLOAT\n");
            break;

        case WAVE_FORMAT_EXTENSIBLE:

            OutputDebugString(L"WAVE_FORMAT_EXTENSIBLE\n");

            pWFX = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);

            if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pWFX->SubFormat)) {

                OutputDebugString(L"     KSDATAFORMAT_SUBTYPE_IEEE_FLOAT\n");

                pWFX->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                pWFX->Samples.wValidBitsPerSample = 16;
                pwfx->wBitsPerSample = 16;

                pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

                memcpy(wfx, &pWFX->Format, sizeof(WAVEFORMATEX));
            }
            else {
                OutputDebugString(L"     KSDATAFORMAT_SUBTYPE_EWAN\n");
            }

            break;

        default:
            OutputDebugString(L"WAVE_FORMAT_EWAN\n");
        }
    }

    if (SUCCEEDED(hr))
        hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0LL, 0LL, pwfx, 0);

    if (SUCCEEDED(hr))
        hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);

    if (SUCCEEDED(hr))
        hr = pAudioClient->Start();

    return hr;
}

HRESULT CCapture::Read(CMediaFile* file, LONGLONG* timestamp)
{
    HRESULT hr;
    UINT32 nPacketSize, nFramesToRead;
    BYTE* buffer;
    DWORD dwFlags, buffersize;
    LONGLONG duration;

    hr = pAudioCaptureClient->GetNextPacketSize(&nPacketSize);

    if (nPacketSize == 0) return hr;

    hr = pAudioCaptureClient->GetBuffer(&buffer, &nFramesToRead, &dwFlags, NULL, NULL);

    if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT)
        OutputDebugString(L"AUDCLNT_BUFFERFLAGS_SILENT\n");;

    buffersize = nFramesToRead * (DWORD)pwfx->nBlockAlign;
    duration = ((LONGLONG)nFramesToRead * 10000000LL) / (LONGLONG)pwfx->nSamplesPerSec;

    file->Write(buffer, buffersize, *timestamp, duration);
    *timestamp += duration;

    hr = pAudioCaptureClient->ReleaseBuffer(nFramesToRead);

    return hr;
}

HRESULT CCapture::Close()
{
    if (pAudioClient != NULL)
        pAudioClient->Stop();

    SafeRelease(&pMMDeviceEnumerator);
    SafeRelease(&pMMDevice);
    SafeRelease(&pAudioCaptureClient);
    SafeRelease(&pAudioClient);

    if (pwfx != NULL)
        CoTaskMemFree(pwfx);

    return S_OK;
}
