
// convert video file to video only file
// This method is asynchronous.

// Tutorial: Encoding a WMA File
// https://learn.microsoft.com/en-us/windows/win32/medfound/tutorial--converting-an-mp3-file-to-a-wma-file

// Tutorial: Encoding an MP4 File
// https://learn.microsoft.com/en-us/windows/win32/medfound/tutorial--encoding-an-mp4-file-

// Audio Subtype GUIDs
// https://learn.microsoft.com/en-us/windows/win32/medfound/audio-subtype-guids

// MPEG-4 File Source
// https://learn.microsoft.com/en-us/windows/win32/medfound/mpeg-4-file-source

// MF_TRANSCODE_CONTAINERTYPE attribute
// https://learn.microsoft.com/en-us/windows/win32/medfound/mf-transcode-containertype

#include "framework.h"
#include "transcoder.h"
#include "mediatypeinfo.h"

CTranscoder::CTranscoder()
{
	m_cRef = 1;
	m_Duration = 0;
	m_hWnd = NULL;
	m_pSession = NULL;
	m_pSourceResolver = NULL;
	m_pMediaSource = NULL;
	m_pPresentationClock = NULL;
}


CTranscoder::~CTranscoder()
{

}

STDMETHODIMP CTranscoder::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CTranscoder, IMFAsyncCallback),
		{ 0 }
	};
	return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) CTranscoder::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CTranscoder::Release()
{
	long cRef = InterlockedDecrement(&m_cRef);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
}

STDMETHODIMP CTranscoder::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
{
	// Implementation of this method is optional.
	return E_NOTIMPL;
}

STDMETHODIMP CTranscoder::Invoke(IMFAsyncResult* pAsyncResult)
{
	IMFMediaEvent* pEvent = NULL;
	MediaEventType meType = MEUnknown;
	MF_OBJECT_TYPE ObjectType;
	IUnknown* pUnknown = NULL;
	IMFAttributes* pAttributes = NULL;
	IMFTranscodeProfile* pProfile = NULL;
	IMFTopology* pTopology = NULL;
	HRESULT hrStatus = S_OK;
	HRESULT	 hr;
	UINT32 status;

	if (m_pMediaSource == NULL) {

		// Completes an asynchronous request to create an object from a URL.
		hr = m_pSourceResolver->EndCreateObjectFromURL(pAsyncResult, &ObjectType, &pUnknown);

		if (ObjectType == MF_OBJECT_MEDIASOURCE) {

			// Get the IMFMediaSource interface from the media source.
			hr = pUnknown->QueryInterface(IID_PPV_ARGS(&m_pMediaSource));

			if (SUCCEEDED(hr)) {

				PrintPresentationInfo(m_pMediaSource);

				GetSourceDuration(m_pMediaSource, &m_Duration);

				hr = CopyAttributes(&pAttributes, m_pMediaSource);

				if (SUCCEEDED(hr)) {

					hr = CreateTranscodeProfile(&pProfile, pAttributes);

					if (SUCCEEDED(hr)) {

						// Call MFCreateTranscodeTopology to create the partial transcode topology.
						hr = MFCreateTranscodeTopology(m_pMediaSource, m_szOutputFile, pProfile, &pTopology);

						if (SUCCEEDED(hr)) {

							hr = m_pSession->SetTopology(0, pTopology);
						}
					}
				}
			}
		}
	}

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

			PostMessage(m_hWnd, WM_SESSION_CLOSED, 0, 0);
			break;

		case MESessionTopologyStatus:

			pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);

			PrintTopoStatusInfo(status);

			if (status == MF_TOPOSTATUS_READY)
				PostMessage(m_hWnd, WM_TOPO_STATUS_READY, 0, (LPARAM)&m_Duration);

			break;
		}
	}

	SafeRelease(&pEvent);
	SafeRelease(&pProfile);
	SafeRelease(&pAttributes);
	SafeRelease(&pTopology);

	return hr;
}

void CTranscoder::PrintMediaFoundationEventInfo(MediaEventType meType)
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

void CTranscoder::PrintObjectTypeInfo(MF_OBJECT_TYPE	objType)
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

void CTranscoder::PrintTopoStatusInfo(UINT32 status)
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

void CTranscoder::PrintPresentationInfo(IMFMediaSource* pSource)
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

void CTranscoder::PrintStreamInfo(IMFStreamDescriptor* pStream)
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

// How to Find the Duration of a Media File
// https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-find-the-duration-of-a-media-file
HRESULT CTranscoder::GetSourceDuration(IMFMediaSource* pSource, MFTIME* pDuration)
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

HRESULT  CTranscoder::CopyAttributes(IMFAttributes** ppAttributes, IMFMediaSource* pMediaSource)
{
	IMFPresentationDescriptor* pPresentationDescriptor;
	IMFStreamDescriptor* pStreamDescriptor = NULL;
	IMFMediaTypeHandler* pHandler = NULL;
	IMFMediaType* pMediaType = NULL;
	IMFAttributes* pAttributes = NULL;
	GUID MajorType;
	BOOL selected;
	DWORD i, n;
	HRESULT hr;

	GUID SubType;
	UINT32 profile;
	UINT32 width, height;
	UINT32 numerator, denominator;
	UINT32 BitRate;

	// Retrieves a copy of the media source's presentation descriptor.
	// Applications use the presentation descriptor to select streams and to get information about the source content.
	hr = m_pMediaSource->CreatePresentationDescriptor(&pPresentationDescriptor);

	if (SUCCEEDED(hr)) {

		hr = pPresentationDescriptor->GetStreamDescriptorCount(&n);

		if (SUCCEEDED(hr)) {

			for (i = 0; i < n; i++) {

				hr = pPresentationDescriptor->GetStreamDescriptorByIndex(i, &selected, &pStreamDescriptor);

				if (SUCCEEDED(hr)) {

					hr = pStreamDescriptor->GetMediaTypeHandler(&pHandler);

					if (SUCCEEDED(hr)) {

						hr = pHandler->GetCurrentMediaType(&pMediaType);

						if (SUCCEEDED(hr)) {

							pMediaType->GetGUID(MF_MT_MAJOR_TYPE, &MajorType);

							if (MajorType == MFMediaType_Video) {

								pMediaType->GetGUID(MF_MT_SUBTYPE, &SubType);
								pMediaType->GetUINT32(MF_MT_MPEG2_PROFILE, &profile);
								MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &width, &height);
								MFGetAttributeSize(pMediaType, MF_MT_FRAME_RATE, &numerator, &denominator);
								pMediaType->GetUINT32(MF_MT_AVG_BITRATE, &BitRate);
							}

							pMediaType->Release();
						}

						pHandler->Release();
					}

					pStreamDescriptor->Release();
				}
			}
		}

		pPresentationDescriptor->Release();
	}

	// Copies the attributes from this object into another attribute store. 
	if (SUCCEEDED(hr)) {

		hr = MFCreateAttributes(&pAttributes, 0);

		if (SUCCEEDED(hr)) {

			pAttributes->SetGUID(MF_MT_SUBTYPE, SubType);
			pAttributes->SetUINT32(MF_MT_MPEG2_PROFILE, profile);
			MFSetAttributeSize(pAttributes, MF_MT_FRAME_SIZE, width, height);
			MFSetAttributeRatio(pAttributes, MF_MT_FRAME_RATE, numerator, denominator);
			pAttributes->SetUINT32(MF_MT_AVG_BITRATE, BitRate);

			// Return the pointer to the caller.
			*ppAttributes = pAttributes;
			(*ppAttributes)->AddRef();

			pAttributes->Release();
		}
	}

	return hr;
}

// the transcode profile that contains the configuration settings for the audio stream, the video stream,
// and the container to which the file is written. The transcode profile object exposes the IMFTranscodeProfile interface
// and must be created by calling the MFCreateTranscodeProfile function. After the object has been created
// the caller must provide the configuration settings by calling appropriate the IMFTranscodeProfile methods.
HRESULT CTranscoder::CreateTranscodeProfile(IMFTranscodeProfile** ppProfile, IMFAttributes* Attributes)
{
	IMFTranscodeProfile* pProfile = NULL;     // Transcode profile.
	IMFAttributes* pContainer = NULL;       // Container attributes.

	// Create an empty transcode profile.
	HRESULT hr = MFCreateTranscodeProfile(&pProfile);
	if (FAILED(hr))
	{
		goto done;
	}


	// Set the audio attributes on the profile.
	hr = pProfile->SetVideoAttributes(Attributes);
	if (FAILED(hr))
	{
		goto done;
	}

	// Set the container attributes.
	hr = MFCreateAttributes(&pContainer, 1);
	if (FAILED(hr))
	{
		goto done;
	}

	// MF_TRANSCODE_CONTAINERTYPE attribute
	// https://learn.microsoft.com/en-us/windows/win32/medfound/mf-transcode-containertype
	hr = pContainer->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pProfile->SetContainerAttributes(pContainer);
	if (FAILED(hr))
	{
		goto done;
	}

	*ppProfile = pProfile;
	(*ppProfile)->AddRef();

done:
	SafeRelease(&pProfile);
	SafeRelease(&pContainer);

	return hr;
}

HRESULT CTranscoder::Create(HWND hWnd)
{
	IMFClock* pClock = NULL;
	HRESULT hr;

	m_hWnd = hWnd;

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

HRESULT CTranscoder::Destroy()
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
	SafeRelease(&m_pSourceResolver);
	SafeRelease(&m_pMediaSource);
	SafeRelease(&m_pPresentationClock);

	return hr;
}

HRESULT CTranscoder::Play()
{
	PROPVARIANT varStart;
	PropVariantInit(&varStart);

	HRESULT hr = m_pSession->Start(&GUID_NULL, &varStart);

	PropVariantClear(&varStart);

	return hr;
}

HRESULT CTranscoder::Pause()
{
	return m_pSession->Pause();
}

HRESULT CTranscoder::Stop()
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
HRESULT CTranscoder::Close()
{
	return m_pSession->Close();
}

// How to Find the Duration of a Media File
// https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-find-the-duration-of-a-media-file
MFTIME CTranscoder::GetSourceDuration()
{
	return m_Duration;
}

// IMFPresentationClock::GetTime method
// https://learn.microsoft.com/en-us/windows/win32/api/mfidl/nf-mfidl-imfpresentationclock-gettime
HRESULT CTranscoder::GetLatestClockTime(MFTIME* pTime)
{
	return m_pPresentationClock->GetTime(pTime);
}

HRESULT CTranscoder::Transcode(LPCWSTR szInputFile, LPCWSTR szOutputFile)
{
	IUnknown* pUnknownCancelCookie = NULL;
	HRESULT hr;

	wcscpy_s(m_szOutputFile, MAX_PATH, szOutputFile);

	// Create the source resolver.
	hr = MFCreateSourceResolver(&m_pSourceResolver);

	if (FAILED(hr))
		goto done;

	// Begins an asynchronous request to create a media source from a URL.
	hr = m_pSourceResolver->BeginCreateObjectFromURL(
		szInputFile,								// URL of the source.
		MF_RESOLUTION_MEDIASOURCE,				// Create a source object.
		NULL,										// Optional property store.
		&pUnknownCancelCookie,			// 
		this,									// Pointer to the IMFAsyncCallback interface of a callback object.
		NULL);									//

	if (FAILED(hr))
		goto done;

	// cancel the asynchronous operation PAG MERON NITO FAILED ANG EndCreateObjectFromURL sa Invoke
	//if (pUnknownCancelCookie != NULL)
	//	hr = m_pSourceResolver->CancelObjectCreation(pUnknownCancelCookie);

done:

	SafeRelease(&pUnknownCancelCookie);

	return hr;
}
