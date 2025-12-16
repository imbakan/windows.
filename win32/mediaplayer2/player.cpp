
#include "framework.h"
#include "player.h"
#include "mediatypeinfo.h"

CPlayer::CPlayer()
{
	m_cRef = 1;
	m_Duration = 0;
	m_hWnd = NULL;
	m_pSession = NULL;
	m_pMediaSource = NULL;
	m_pPresentationClock = NULL;
	m_pVideoDisplay = NULL;
}


CPlayer::~CPlayer()
{

}

STDMETHODIMP CPlayer::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CPlayer, IMFAsyncCallback),
		{ 0 }
	};
	return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) CPlayer::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CPlayer::Release()
{
	long cRef = InterlockedDecrement(&m_cRef);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
}

STDMETHODIMP CPlayer::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
{
	// Implementation of this method is optional.
	return E_NOTIMPL;
}

STDMETHODIMP CPlayer::Invoke(IMFAsyncResult* pAsyncResult)
{
	IMFMediaEvent* pEvent = NULL;
	MediaEventType meType = MEUnknown;
	HRESULT hrStatus = S_OK;
	HRESULT	 hr;
	UINT32 status;

	// Get the event from the event queue. 
	hr = m_pSession->EndGetEvent(pAsyncResult, &pEvent);

	if (SUCCEEDED(hr)) {

		// Retrieves the event type. The event type indicates what happened to trigger the event.
		// It also defines the meaning of the event value.
		hr = pEvent->GetType(&meType);

		if (SUCCEEDED(hr)) {

			PrintMediaFoundationEventInfo(meType);

			// Receives the event status. If the operation that generated the event was successful, the value is a success code.
			// A failure code means that an error condition triggered the event.
			hr = pEvent->GetStatus(&hrStatus);

			if (SUCCEEDED(hr)) {
				if (FAILED(hrStatus)) {
					hr = hrStatus;
				}
			}
		}
	}

	// Handle the event.
	if (SUCCEEDED(hr)) {

		// If not finished, request another event.
		if (meType != MESessionClosed) {
			hr = m_pSession->BeginGetEvent(this, NULL);
		}

		// handle event messagees
		switch (meType) {

		case MESessionStarted:

			PostMessage(m_hWnd, WM_SESSION_STARTED, 0, 0);
			break;

		case MESessionPaused:

			PostMessage(m_hWnd, WM_SESSION_PAUSED, 0, 0);
			break;

		case MESessionStopped:

			PostMessage(m_hWnd, WM_SESSION_STOPPED, 0, 0);
			break;

		case MESessionEnded:

			PostMessage(m_hWnd, WM_SESSION_ENDED, 0, 0);
			break;

		case MESessionClosed:

			PostMessage(m_hWnd, WM_PRE_DESTROY, 0, 0);
			break;

		case MESessionTopologyStatus:

			pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);

			if (status == MF_TOPOSTATUS_READY)
				PostMessage(m_hWnd, WM_TOPO_STATUS_READY, 0, 0);

			break;
		}
	}

	SafeRelease(&pEvent);

	return hr;
}

void CPlayer::PrintMediaFoundationEventInfo(MediaEventType meType)
{
	wchar_t str[100];

	switch (meType)
	{
	case MEAudioSessionDeviceRemoved: wcscpy_s(str, 100, L"MEAudioSessionDeviceRemoved"); break;
	case MEAudioSessionDisconnected: wcscpy_s(str, 100, L"MEAudioSessionDisconnected"); break;
	case MEAudioSessionExclusiveModeOverride: wcscpy_s(str, 100, L"MEAudioSessionExclusiveModeOverride"); break;
	case MEAudioSessionFormatChanged: wcscpy_s(str, 100, L"MEAudioSessionFormatChanged"); break;
	case MEAudioSessionGroupingParamChanged: wcscpy_s(str, 100, L"MEAudioSessionGroupingParamChanged"); break;
	case MEAudioSessionIconChanged: wcscpy_s(str, 100, L"MEAudioSessionIconChanged"); break;
	case MEAudioSessionNameChanged: wcscpy_s(str, 100, L"MEAudioSessionNameChanged"); break;
	case MEAudioSessionServerShutdown: wcscpy_s(str, 100, L"MEAudioSessionServerShutdown"); break;
	case MEAudioSessionVolumeChanged: wcscpy_s(str, 100, L"MEAudioSessionVolumeChanged"); break;
	case MEBufferingStarted: wcscpy_s(str, 100, L"MEBufferingStarted"); break;
	case MEBufferingStopped: wcscpy_s(str, 100, L"MEBufferingStopped"); break;
	case MECaptureAudioSessionDeviceRemoved: wcscpy_s(str, 100, L"MECaptureAudioSessionDeviceRemoved"); break;
	case MECaptureAudioSessionDisconnected: wcscpy_s(str, 100, L"MECaptureAudioSessionDisconnected"); break;
	case MECaptureAudioSessionExclusiveModeOverride: wcscpy_s(str, 100, L"MECaptureAudioSessionExclusiveModeOverride"); break;
	case MECaptureAudioSessionFormatChanged: wcscpy_s(str, 100, L"MECaptureAudioSessionFormatChanged"); break;
	case MECaptureAudioSessionServerShutdown: wcscpy_s(str, 100, L"MECaptureAudioSessionServerShutdown"); break;
	case MECaptureAudioSessionVolumeChanged: wcscpy_s(str, 100, L"MECaptureAudioSessionVolumeChanged"); break;
	case MEConnectEnd: wcscpy_s(str, 100, L"MEConnectEnd"); break;
	case MEConnectStart: wcscpy_s(str, 100, L"MEConnectStart"); break;
	case MEContentProtectionMessage: wcscpy_s(str, 100, L"MEContentProtectionMessage"); break;
	case MEEnablerCompleted: wcscpy_s(str, 100, L"MEEnablerCompleted"); break;
	case MEEnablerProgress: wcscpy_s(str, 100, L"MEEnablerProgress"); break;
	case MEEndOfPresentation: wcscpy_s(str, 100, L"MEEndOfPresentation"); break;
	case MEEndOfPresentationSegment: wcscpy_s(str, 100, L"MEEndOfPresentationSegment"); break;
	case MEEndOfStream: wcscpy_s(str, 100, L"MEEndOfStream"); break;
	case MEError: wcscpy_s(str, 100, L"MEError"); break;
	case MEExtendedType: wcscpy_s(str, 100, L"MEExtendedType"); break;
	case MEIndividualizationCompleted: wcscpy_s(str, 100, L"MEIndividualizationCompleted"); break;
	case MEIndividualizationStart: wcscpy_s(str, 100, L"MEIndividualizationStart"); break;
	case MELicenseAcquisitionCompleted: wcscpy_s(str, 100, L"MELicenseAcquisitionCompleted"); break;
	case MELicenseAcquisitionStart: wcscpy_s(str, 100, L"MELicenseAcquisitionStart"); break;
	case MEMediaSample: wcscpy_s(str, 100, L"MEMediaSample"); break;
	case MENewPresentation: wcscpy_s(str, 100, L"MENewPresentation"); break;
	case MENewStream: wcscpy_s(str, 100, L"MENewStream"); break;
	case MENonFatalError: wcscpy_s(str, 100, L"MENonFatalError"); break;
	case MEPolicyChanged: wcscpy_s(str, 100, L"MEPolicyChanged"); break;
	case MEPolicyError: wcscpy_s(str, 100, L"MEPolicyError"); break;
	case MEPolicyReport: wcscpy_s(str, 100, L"MEPolicyReport"); break;
	case MEPolicySet: wcscpy_s(str, 100, L"MEPolicySet"); break;
	case MEQualityNotify: wcscpy_s(str, 100, L"MEQualityNotify"); break;
	case MEReconnectEnd: wcscpy_s(str, 100, L"MEReconnectEnd"); break;
	case MEReconnectStart: wcscpy_s(str, 100, L"MEReconnectStart"); break;
	case MERendererEvent: wcscpy_s(str, 100, L"MERendererEvent"); break;
	case MESequencerSourceTopologyUpdated: wcscpy_s(str, 100, L"MESequencerSourceTopologyUpdated"); break;
	case MESessionCapabilitiesChanged: wcscpy_s(str, 100, L"MESessionCapabilitiesChanged"); break;
	case MESessionClosed: wcscpy_s(str, 100, L"MESessionClosed"); break;
	case MESessionEnded: wcscpy_s(str, 100, L"MESessionEnded"); break;
	case MESessionNotifyPresentationTime: wcscpy_s(str, 100, L"MESessionNotifyPresentationTime"); break;
	case MESessionPaused: wcscpy_s(str, 100, L"MESessionPaused"); break;
	case MESessionRateChanged: wcscpy_s(str, 100, L"MESessionRateChanged"); break;
	case MESessionScrubSampleComplete: wcscpy_s(str, 100, L"MESessionScrubSampleComplete"); break;
	case MESessionStarted: wcscpy_s(str, 100, L"MESessionStarted"); break;
	case MESessionStopped: wcscpy_s(str, 100, L"MESessionStopped"); break;
	case MESessionStreamSinkFormatChanged: wcscpy_s(str, 100, L"MESessionStreamSinkFormatChanged"); break;
	case MESessionTopologiesCleared: wcscpy_s(str, 100, L"MESessionTopologiesCleared"); break;
	case MESessionTopologySet: wcscpy_s(str, 100, L"MESessionTopologySet"); break;
	case MESessionTopologyStatus: wcscpy_s(str, 100, L"MESessionTopologyStatus"); break;
	case MESinkInvalidated: wcscpy_s(str, 100, L"MESinkInvalidated"); break;
	case MESourceCharacteristicsChanged: wcscpy_s(str, 100, L"MESourceCharacteristicsChanged"); break;
	case MESourceMetadataChanged: wcscpy_s(str, 100, L"MESourceMetadataChanged"); break;
	case MESourcePaused: wcscpy_s(str, 100, L"MESourcePaused"); break;
	case MESourceRateChanged: wcscpy_s(str, 100, L"MESourceRateChanged"); break;
	case MESourceRateChangeRequested: wcscpy_s(str, 100, L"MESourceRateChangeRequested"); break;
	case MESourceSeeked: wcscpy_s(str, 100, L"MESourceSeeked"); break;
	case MESourceStarted: wcscpy_s(str, 100, L"MESourceStarted"); break;
	case MESourceStopped: wcscpy_s(str, 100, L"MESourceStopped"); break;
	case MEStreamFormatChanged: wcscpy_s(str, 100, L"MEStreamFormatChanged"); break;
	case MEStreamPaused: wcscpy_s(str, 100, L"MEStreamPaused"); break;
	case MEStreamSeeked: wcscpy_s(str, 100, L"MEStreamSeeked"); break;
	case MEStreamSinkDeviceChanged: wcscpy_s(str, 100, L"MEStreamSinkDeviceChanged"); break;
	case MEStreamSinkFormatChanged: wcscpy_s(str, 100, L"MEStreamSinkFormatChanged"); break;
	case MEStreamSinkMarker: wcscpy_s(str, 100, L"MEStreamSinkMarker"); break;
	case MEStreamSinkPaused: wcscpy_s(str, 100, L"MEStreamSinkPaused"); break;
	case MEStreamSinkPrerolled: wcscpy_s(str, 100, L"MEStreamSinkPrerolled"); break;
	case MEStreamSinkRateChanged: wcscpy_s(str, 100, L"MEStreamSinkRateChanged"); break;
	case MEStreamSinkRequestSample: wcscpy_s(str, 100, L"MEStreamSinkRequestSample"); break;
	case MEStreamSinkScrubSampleComplete: wcscpy_s(str, 100, L"MEStreamSinkScrubSampleComplete"); break;
	case MEStreamSinkStarted: wcscpy_s(str, 100, L"MEStreamSinkStarted"); break;
	case MEStreamSinkStopped: wcscpy_s(str, 100, L"MEStreamSinkStopped"); break;
	case MEStreamStarted: wcscpy_s(str, 100, L"MEStreamStarted"); break;
	case MEStreamStopped: wcscpy_s(str, 100, L"MEStreamStopped"); break;
	case MEStreamThinMode: wcscpy_s(str, 100, L"MEStreamThinMode"); break;
	case MEStreamTick: wcscpy_s(str, 100, L"MEStreamTick"); break;
	case METransformDrainComplete: wcscpy_s(str, 100, L"METransformDrainComplete"); break;
	case METransformHaveOutput: wcscpy_s(str, 100, L"METransformHaveOutput"); break;
	case METransformMarker: wcscpy_s(str, 100, L"METransformMarker"); break;
	case METransformNeedInput: wcscpy_s(str, 100, L"METransformNeedInput"); break;
	case MEUnknown: wcscpy_s(str, 100, L"MEUnknown"); break;
	case MEUpdatedStream: wcscpy_s(str, 100, L"MEUpdatedStream"); break;
	case MEVideoCaptureDevicePreempted: wcscpy_s(str, 100, L"MEVideoCaptureDevicePreempted"); break;
	case MEVideoCaptureDeviceRemoved: wcscpy_s(str, 100, L"MEVideoCaptureDeviceRemoved"); break;
	default:
		wcscpy_s(str, 100, L"MEEwan");
	}

	OutputDebugString(L".......... ");
	OutputDebugString(str);
	OutputDebugString(L"\n");
}

void CPlayer::PrintObjectTypeInfo(MF_OBJECT_TYPE	objType)
{
	wchar_t str[100];

	switch (objType)
	{
	case MF_OBJECT_MEDIASOURCE: wcscpy_s(str, 100, L"MF_OBJECT_MEDIASOURCE"); break;
	case MF_OBJECT_BYTESTREAM: wcscpy_s(str, 100, L"MF_OBJECT_BYTESTREAM"); break;
	case MF_OBJECT_INVALID: wcscpy_s(str, 100, L"MF_OBJECT_INVALID"); break;
	default:
		wcscpy_s(str, 100, L"MF_OBJECT_EWAN");
	}

	OutputDebugString(L".......... ");
	OutputDebugString(str);
	OutputDebugString(L"\n");
}

void CPlayer::PrintTopoStatusInfo(UINT32 status)
{
	wchar_t str[100];

	switch (status)
	{
	case MF_TOPOSTATUS_INVALID: wcscpy_s(str, 100, L"MF_TOPOSTATUS_INVALID"); break;
	case MF_TOPOSTATUS_READY: wcscpy_s(str, 100, L"MF_TOPOSTATUS_READY"); break;
	case MF_TOPOSTATUS_STARTED_SOURCE: wcscpy_s(str, 100, L"MF_TOPOSTATUS_STARTED_SOURCE"); break;
	case MF_TOPOSTATUS_DYNAMIC_CHANGED: wcscpy_s(str, 100, L"MF_TOPOSTATUS_DYNAMIC_CHANGED"); break;
	case MF_TOPOSTATUS_SINK_SWITCHED: wcscpy_s(str, 100, L"MF_TOPOSTATUS_SINK_SWITCHED"); break;
	case MF_TOPOSTATUS_ENDED: wcscpy_s(str, 100, L"MF_TOPOSTATUS_ENDED"); break;
	default:
		wcscpy_s(str, 100, L"MF_TOPOSTATUS_EWAN");
	}

	OutputDebugString(L".......... ");
	OutputDebugString(str);
	OutputDebugString(L"\n");
}

void CPlayer::PrintPresentationInfo(IMFMediaSource* pSource)
{
	IMFPresentationDescriptor* pPresentation = NULL;
	IMFStreamDescriptor* pStream = NULL;
	DWORD i, dwStreamCount;
	BOOL selected;
	HRESULT hr;
	MFTIME duration;
	wchar_t str[100];

	hr = pSource->CreatePresentationDescriptor(&pPresentation);

	if (SUCCEEDED(hr)) {

		pPresentation->GetUINT64(MF_PD_DURATION, (UINT64*)&duration);

		swprintf_s(str, 100, L"\n\t%-50s %lld\n", L"MF_PD_DURATION", duration);
		OutputDebugString(str);

		// Get the number of streams in the media source.
		hr = pPresentation->GetStreamDescriptorCount(&dwStreamCount);

		if (SUCCEEDED(hr)) {

			for (i = 0; i < dwStreamCount; i++) {

				selected = FALSE;
				hr = pPresentation->GetStreamDescriptorByIndex(i, &selected, &pStream);

				if (SUCCEEDED(hr)) {
					if (selected) {

						OutputDebugString(L"\n");
						PrintStreamInfo(pStream);
					}
				}
			}

			OutputDebugString(L"\n");
		}

		pPresentation->Release();
	}
}

void CPlayer::PrintStreamInfo(IMFStreamDescriptor* pStream)
{
	IMFMediaTypeHandler* pHandler = NULL;
	IMFMediaType* pMediaType = NULL;
	HRESULT hr;

	// Find the media type for the stream.
	hr = pStream->GetMediaTypeHandler(&pHandler);

	if (SUCCEEDED(hr)) {

		hr = pHandler->GetCurrentMediaType(&pMediaType);

		if (SUCCEEDED(hr)) {

			LogMediaType(pMediaType);
		}
	}

	SafeRelease(&pHandler);
	SafeRelease(&pMediaType);
}

// Configuring a Media Source
// https://learn.microsoft.com/en-us/windows/win32/medfound/configuring-a-media-source
//
// Creates a media source from a URL.
HRESULT CPlayer::CreateMediaSource(PCWSTR pszURL, IMFMediaSource** ppSource)
{
	MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

	IMFSourceResolver* pResolver = NULL;
	IUnknown* pSource = NULL;

	// Create the source resolver.
	HRESULT hr = MFCreateSourceResolver(&pResolver);
	if (FAILED(hr))
	{
		goto done;
	}

	// Use the source resolver to create the media source
	hr = pResolver->CreateObjectFromURL(
		pszURL,
		MF_RESOLUTION_MEDIASOURCE,  // Create a media source.
		NULL,
		&ObjectType,				// Receives the object type. 
		&pSource);
	if (FAILED(hr))
	{
		goto done;
	}

	// Get the IMFMediaSource interface from the media source.
	hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));

done:
	SafeRelease(&pResolver);
	SafeRelease(&pSource);

	return hr;
}

// How to Find the Duration of a Media File
// https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-find-the-duration-of-a-media-file
HRESULT CPlayer::GetSourceDuration(IMFMediaSource* pSource, MFTIME* pDuration)
{
	*pDuration = 0;

	IMFPresentationDescriptor* pPD = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (SUCCEEDED(hr))
	{
		hr = pPD->GetUINT64(MF_PD_DURATION, (UINT64*)pDuration);
		pPD->Release();
	}

	return hr;
}

HRESULT CPlayer::CreateSourceNodes(IMFMediaSource* pMediaSource, IMFTopologyNode** ppVideoSourceNode, IMFTopologyNode** ppAudioSourceNode)
{
	IMFPresentationDescriptor* pPresentationDescriptor = NULL;
	IMFStreamDescriptor* pStreamDescriptor = NULL;
	IMFMediaTypeHandler* pHandler = NULL;
	IMFMediaType* pMediaType = NULL;
	GUID MajorType = GUID_NULL;
	DWORD i, dwStreamCount;
	BOOL selected;
	HRESULT hr;

	hr = pMediaSource->CreatePresentationDescriptor(&pPresentationDescriptor);

	if (SUCCEEDED(hr)) {

		// Get the number of streams in the media source.
		hr = pPresentationDescriptor->GetStreamDescriptorCount(&dwStreamCount);

		if (SUCCEEDED(hr)) {

			for (i = 0; i < dwStreamCount; i++) {

				selected = FALSE;
				hr = pPresentationDescriptor->GetStreamDescriptorByIndex(i, &selected, &pStreamDescriptor);

				if (SUCCEEDED(hr)) {
					if (selected) {

						// Get the media type handler for the stream.
						hr = pStreamDescriptor->GetMediaTypeHandler(&pHandler);

						if (SUCCEEDED(hr)) {

							// Get the major media type.
							hr = pHandler->GetMajorType(&MajorType);

							if (SUCCEEDED(hr)) {

								if (MFMediaType_Audio == MajorType) {
									CreateSourceNode(pMediaSource, pPresentationDescriptor, pStreamDescriptor, ppAudioSourceNode);
								}
								else if (MFMediaType_Video == MajorType) {
									CreateSourceNode(pMediaSource, pPresentationDescriptor, pStreamDescriptor, ppVideoSourceNode);
								}
								else {
									// Unknown stream type. 
									hr = E_FAIL;
									// Optionally, you could deselect this stream instead of failing.
								}
							}

							pHandler->Release();
						}
					}

					pStreamDescriptor->Release();
				}
			}
		}

		pPresentationDescriptor->Release();
	}

	return hr;
}

HRESULT CPlayer::CreateSourceNode(IMFMediaSource* pMediaSource, IMFPresentationDescriptor* pPresentationDescriptor, IMFStreamDescriptor* pStreamDescriptor, IMFTopologyNode** ppNode)
{
	IMFTopologyNode* pNode = NULL;
	HRESULT hr;

	hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);

	if (SUCCEEDED(hr)) {

		hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pMediaSource);

		if (SUCCEEDED(hr))
			hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pMediaSource);

		if (SUCCEEDED(hr))
			hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPresentationDescriptor);

		if (SUCCEEDED(hr))
			hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pStreamDescriptor);

		*ppNode = pNode;
		(*ppNode)->AddRef();

		pNode->Release();
	}

	return hr;
}

HRESULT CPlayer::CreateSinkNode(IMFActivate* pActivate, DWORD dwId, IMFTopologyNode** ppNode)
{
	IMFTopologyNode* pNode = NULL;
	HRESULT hr;

	// Create the node.
	hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
	if (FAILED(hr)) {
		goto done;
	}

	// Set the object pointer.
	hr = pNode->SetObject(pActivate);
	if (FAILED(hr)) {
		goto done;
	}

	// TRUE	 When the Media Session switches to a new topology or clears the current topology,
	//       it does not shut down the object that belongs to this topology node.
	// FALSE When the Media Session switches to a new topology or clears the current topology,
	//       it shuts down the node object, as follows:
	//            Output nodes:    The session calls IMFMediaSink::Shutdown on the media sink.
	//            Transform nodes: The session calls IMFShutdown::Shutdown on the MFT.
	hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	if (FAILED(hr)) {
		goto done;
	}

	// Return the pointer to the caller.
	*ppNode = pNode;
	(*ppNode)->AddRef();

done:

	SafeRelease(&pNode);

	return hr;
}

HRESULT CPlayer::CreatePlaybackTopology(IMFTopology** ppTopology, IMFTopologyNode* pVideoSourceNode, IMFTopologyNode* pAudioSourceNode, IMFTopologyNode* pVideoSinkNode, IMFTopologyNode* pAudioSinkNode)
{
	IMFTopology* pTopology = NULL;
	HRESULT hr;

	hr = MFCreateTopology(&pTopology);

	if (SUCCEEDED(hr)) {

		hr = pTopology->AddNode(pVideoSourceNode);

		if (SUCCEEDED(hr))
			hr = pTopology->AddNode(pAudioSourceNode);

		if (SUCCEEDED(hr))
			hr = pTopology->AddNode(pVideoSinkNode);

		if (SUCCEEDED(hr))
			hr = pTopology->AddNode(pAudioSinkNode);

		if (SUCCEEDED(hr))
			hr = pVideoSourceNode->ConnectOutput(0, pVideoSinkNode, 0);

		if (SUCCEEDED(hr))
			hr = pAudioSourceNode->ConnectOutput(0, pAudioSinkNode, 0);

		*ppTopology = pTopology;
		(*ppTopology)->AddRef();

		pTopology->Release();
	}

	return hr;
}

HRESULT CPlayer::Create(HWND hWnd, HWND hVideoWnd)
{
	IMFClock* pClock = NULL;
	HRESULT hr;

	m_hWnd = hWnd;
	m_hVideoWnd = hVideoWnd;

	// Create the media session.
	hr = MFCreateMediaSession(NULL, &m_pSession);

	if (SUCCEEDED(hr))
		hr = m_pSession->GetClock(&pClock);

	if (SUCCEEDED(hr))
		hr = pClock->QueryInterface(IID_PPV_ARGS(&m_pPresentationClock));

	// Begins an asynchronous request for the next event in the queue.
	if (SUCCEEDED(hr))
		hr = m_pSession->BeginGetEvent(this, NULL);

	return hr;
}

HRESULT CPlayer::Destroy()
{
	HRESULT hr = S_OK;

	if (m_pMediaSource != NULL)
		hr = m_pMediaSource->Shutdown();

	// Shuts down the Media Session and releases all the resources used by the Media Session.
	// Call this method when you are done using the Media Session, before the final call to IUnknown::Release.
	// Otherwise, your application will leak memory.
	// After this method is called, other IMFMediaSession methods return MF_E_SHUTDOWN.
	if (m_pSession != NULL)
		hr = m_pSession->Shutdown();

	// release the interface
	SafeRelease(&m_pSession);
	SafeRelease(&m_pMediaSource);
	SafeRelease(&m_pPresentationClock);
	SafeRelease(&m_pVideoDisplay);

	return hr;
}

HRESULT CPlayer::Play()
{
	PROPVARIANT varStart;
	PropVariantInit(&varStart);

	HRESULT hr = m_pSession->Start(&GUID_NULL, &varStart);

	PropVariantClear(&varStart);

	return hr;
}

HRESULT CPlayer::Pause()
{
	return m_pSession->Pause();
}

HRESULT CPlayer::Stop()
{
	return m_pSession->Stop();
}

// Closes the Media Session and releases all of the resources it is using.
// This method is asynchronous. When the operation completes, the Media Session sends an MESessionClosed event.
// After the Close method is called, the only valid methods on the Media Session are the following:
//
//    IMFMediaSession::GetClock
//    IMFMediaSession::GetFullTopology
//    IMFMediaSession::GetSessionCapabilities
//    IMFMediaSession::Shutdown
//
// All other methods return MF_E_INVALIDREQUEST, or else queue an event with that error code.
HRESULT CPlayer::Close()
{
	return m_pSession->Close();
}

HRESULT CPlayer::UpdateVideoWindow()
{
	if (m_pVideoDisplay == NULL) return S_OK;

	RECT rect;
	GetClientRect(m_hVideoWnd, &rect);

	return m_pVideoDisplay->SetVideoPosition(NULL, &rect);
}

// How to Find the Duration of a Media File
// https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-find-the-duration-of-a-media-file
MFTIME CPlayer::GetSourceDuration()
{
	return m_Duration;
}

// IMFPresentationClock::GetTime method
// https://learn.microsoft.com/en-us/windows/win32/api/mfidl/nf-mfidl-imfpresentationclock-gettime
HRESULT CPlayer::GetLatestClockTime(MFTIME* pTime)
{
	return m_pPresentationClock->GetTime(pTime);
}
// How to Perform Scrubbing
// https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-perform-scrubbing
HRESULT CPlayer::SkipToPosition(MFTIME SeekTime)
{
	PROPVARIANT var;
	PropVariantInit(&var);

	IMFRateControl* pRateControl = NULL;

	// Get the rate control service.
	HRESULT hr = MFGetService(m_pSession, MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&pRateControl));

	// Set the playback rate to zero without thinning.
	if (SUCCEEDED(hr)) {
		hr = pRateControl->SetRate(FALSE, 0.0F);
	}

	// Create the Media Session start position.
	if (SeekTime == PRESENTATION_CURRENT_POSITION) {
		var.vt = VT_EMPTY;
	}
	else {
		var.vt = VT_I8;
		var.hVal.QuadPart = SeekTime;
	}

	// Start the Media Session.
	if (SUCCEEDED(hr)) {
		hr = m_pSession->Start(NULL, &var);
	}

	// Clean up.
	SafeRelease(&pRateControl);
	PropVariantClear(&var);

	return hr;
}

HRESULT CPlayer::Play(MFTIME StartTime)
{
	PROPVARIANT var;
	PropVariantInit(&var);

	var.vt = VT_I8;
	var.hVal.QuadPart = StartTime;

	HRESULT hr = m_pSession->Start(&GUID_NULL, &var);

	PropVariantClear(&var);

	return hr;
}

HRESULT CPlayer::Open(const wchar_t* szFile)
{
	IMFTopologyNode* pVideoSourceNode = NULL;
	IMFTopologyNode* pAudioSourceNode = NULL;
	IMFActivate* pVideoActivate = NULL;
	IMFActivate* pAudioActivate = NULL;
	IMFTopologyNode* pVideoSinkNode = NULL;
	IMFTopologyNode* pAudioSinkNode = NULL;
	IMFTopology* pTopology = NULL;

	HRESULT hr;

	hr = CreateMediaSource(szFile, &m_pMediaSource);
	if (FAILED(hr))
	{
		goto done;
	}

	PrintPresentationInfo(m_pMediaSource);

	hr = GetSourceDuration(m_pMediaSource, &m_Duration);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = CreateSourceNodes(m_pMediaSource, &pVideoSourceNode, &pAudioSourceNode);
	if (FAILED(hr))
	{
		goto done;
	}

	// Create the audio renderer.
	hr = MFCreateAudioRendererActivate(&pAudioActivate);
	if (FAILED(hr))
	{
		goto done;
	}

	// Create the video renderer.
	hr = MFCreateVideoRendererActivate(m_hVideoWnd, &pVideoActivate);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = CreateSinkNode(pAudioActivate, 0, &pAudioSinkNode);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = CreateSinkNode(pVideoActivate, 1, &pVideoSinkNode);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = CreatePlaybackTopology(&pTopology, pVideoSourceNode, pAudioSourceNode, pVideoSinkNode, pAudioSinkNode);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = m_pSession->SetTopology(0, pTopology);

done:

	SafeRelease(&pVideoSourceNode);
	SafeRelease(&pAudioSourceNode);

	SafeRelease(&pVideoActivate);
	SafeRelease(&pAudioActivate);

	SafeRelease(&pVideoSinkNode);
	SafeRelease(&pAudioSinkNode);

	SafeRelease(&pTopology);

	return hr;
}

BOOL CPlayer::IsNotNullMediSource()
{
	return m_pMediaSource != NULL;
}
