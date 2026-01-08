
#pragma once

template <class T>

void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

template <class Q>

HRESULT GetCollectionObject(IMFCollection* pCollection, DWORD index, Q** ppObj)
{
	IUnknown* pUnk;
	HRESULT hr = pCollection->GetElement(index, &pUnk);
	if (SUCCEEDED(hr))
	{
		hr = pUnk->QueryInterface(IID_PPV_ARGS(ppObj));
		pUnk->Release();
	}
	return hr;
}

class CTranscoder : public IMFAsyncCallback
{
private:
	HWND                    m_hWnd;
	long                    m_cRef;
	IMFMediaSession* m_pSession;
	IMFSourceResolver* m_pSourceResolver;
	IMFMediaSource* m_pMediaSource;
	IMFPresentationClock* m_pPresentationClock;
	MFTIME                  m_Duration;
	WCHAR					m_szOutputFile[MAX_PATH];

	void PrintMediaFoundationEventInfo(MediaEventType meType);
	void PrintObjectTypeInfo(MF_OBJECT_TYPE	objType);
	void PrintTopoStatusInfo(UINT32 status);

	void PrintPresentationInfo(IMFMediaSource* pSource);
	void PrintStreamInfo(IMFStreamDescriptor* pStream);

	HRESULT GetSourceDuration(IMFMediaSource* pMediaSource, MFTIME* pDuration);
	HRESULT CopyAttributes(IMFAttributes** ppAttributes, IMFMediaSource* pMediaSource);
	HRESULT CreateTranscodeProfile(IMFTranscodeProfile** ppProfile, IMFAttributes* pAttributes);

public:
	CTranscoder();
	~CTranscoder();

	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFAsyncCallback methods
	STDMETHODIMP  GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
	STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);

	HRESULT Create(HWND hWnd);
	HRESULT Destroy();

	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	HRESULT Close();

	MFTIME GetSourceDuration();
	HRESULT GetLatestClockTime(MFTIME* pTime);

	HRESULT Transcode(LPCWSTR szInputFile, LPCWSTR szOutputFile);
};

