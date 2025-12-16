
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

class CPlayer : public IMFAsyncCallback
{
private:
	HWND                    m_hWnd;
	HWND                    m_hVideoWnd;
	long                    m_cRef;
	IMFMediaSession*		m_pSession;
	IMFSourceResolver*		m_pSourceResolver;
	IMFMediaSource*			m_pMediaSource;
	IMFPresentationClock*	m_pPresentationClock;
	IMFVideoDisplayControl* m_pVideoDisplay;
	MFTIME                  m_Duration;

	void PrintMediaFoundationEventInfo(MediaEventType meType);
	void PrintObjectTypeInfo(MF_OBJECT_TYPE	objType);
	void PrintTopoStatusInfo(UINT32 status);

	void PrintPresentationInfo(IMFMediaSource* pSource);
	void PrintStreamInfo(IMFStreamDescriptor* pStream);

	HRESULT CreateMediaSource(PCWSTR pszURL, IMFMediaSource** ppSource);
	HRESULT CompleteRequest(IMFMediaSource* pMediaSource);
	HRESULT GetSourceDuration(IMFMediaSource* pMediaSource, MFTIME* pDuration);
	HRESULT CreateSourceNodes(IMFMediaSource* pMediaSource, IMFTopologyNode** ppVideoSourceNode, IMFTopologyNode** ppAudioSourceNode);
	HRESULT CreateSourceNode(IMFMediaSource* pMediaSource, IMFPresentationDescriptor* pPresentationDescriptor, IMFStreamDescriptor* pStreamDescriptor, IMFTopologyNode** ppNode);
	HRESULT CreateSinkNode(IMFActivate* pActivate, DWORD dwId, IMFTopologyNode** ppNode);
	HRESULT CreatePlaybackTopology(IMFTopology** ppTopology, IMFTopologyNode* pVideoSourceNode, IMFTopologyNode* pAudioSourceNode, IMFTopologyNode* pVideoSinkNode, IMFTopologyNode* pAudioSinkNode);

public:
	CPlayer();
	~CPlayer();

	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFAsyncCallback methods
	STDMETHODIMP  GetParameters(DWORD* pdwFlags, DWORD* pdwQueue);
	STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);

	HRESULT Create(HWND hWnd, HWND hVideoWnd);
	HRESULT Destroy();

	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	HRESULT Close();

	HRESULT UpdateVideoWindow();

	MFTIME GetSourceDuration();
	HRESULT GetLatestClockTime(MFTIME* pTime);
	HRESULT SkipToPosition(MFTIME SeekTime);
	HRESULT Play(MFTIME StartTime);

	HRESULT Open(const wchar_t* filename);

	BOOL IsNotNullMediSource();
};

