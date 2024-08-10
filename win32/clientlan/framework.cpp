
// Windows Sockets Error Codes
// https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2

#include "framework.h"

wchar_t* GetErrorMessage(int err)
{
	const int n = 825;
	static wchar_t str[n];

	switch (err)
	{
	case WSA_INVALID_HANDLE:
		wcscpy_s(str, n, L"Specified event object handle is invalid.\r\nAn application attempts to use an event object, but the specified handle is not valid.");
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		wcscpy_s(str, n, L"Insufficient memory available.\r\nAn application used a Windows Sockets function that directly maps to a Windows function. The Windows function is indicating a lack of required memory resources.");
		break;
	case WSA_INVALID_PARAMETER:
		wcscpy_s(str, n, L"One or more parameters are invalid.\r\nAn application used a Windows Sockets function which directly maps to a Windows function. The Windows function is indicating a problem with one or more parameters.");
		break;
	case WSA_OPERATION_ABORTED:
		wcscpy_s(str, n, L"Overlapped operation aborted.\r\nAn overlapped operation was canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl.");
		break;
	case WSA_IO_INCOMPLETE:
		wcscpy_s(str, n, L"Overlapped I/O event object not in signaled state.\r\nThe application has tried to determine the status of an overlapped operation which is not yet completed. Applications that use WSAGetOverlappedResult (with the fWait flag set to FALSE) in a polling mode to determine when an overlapped operation has completed, get this error code until the operation is complete.");
		break;
	case WSA_IO_PENDING:
		wcscpy_s(str, n, L"Overlapped operations will complete later.\r\nThe application has initiated an overlapped operation that cannot be completed immediately. A completion indication will be given later when the operation has been completed.");
		break;
	case WSAEINTR:
		wcscpy_s(str, n, L"Interrupted function call.\r\nA blocking operation was interrupted by a call to WSACancelBlockingCall.");
		break;
	case WSAEBADF:
		wcscpy_s(str, n, L"File handle is not valid.\r\nThe file handle supplied is not valid.");
		break;
	case WSAEACCES:
		wcscpy_s(str, n, L"Permission denied.\r\nAn attempt was made to access a socket in a way forbidden by its access permissions. An example is using a broadcast address for sendto without broadcast permission being set using setsockopt(SO_BROADCAST).\r\nAnother possible reason for the WSAEACCES error is that when the bind function is called (on Windows NT 4.0 with SP4 and later), another application, service, or kernel mode driver is bound to the same address with exclusive access. Such exclusive access is a new feature of Windows NT 4.0 with SP4 and later, and is implemented by using the SO_EXCLUSIVEADDRUSE option.");
		break;
	case WSAEFAULT:
		wcscpy_s(str, n, L"Bad address.\r\nThe system detected an invalid pointer address in attempting to use a pointer argument of a call. This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small. For instance, if the length of an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr).");
		break;
	case WSAEINVAL:
		wcscpy_s(str, n, L"Invalid argument.\r\nSome invalid argument was supplied (for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socketófor instance, calling accept on a socket that is not listening.");
		break;
	case WSAEMFILE:
		wcscpy_s(str, n, L"Too many open files.\r\nToo many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process, or per thread.");
		break;
	case WSAEWOULDBLOCK:
		wcscpy_s(str, n, L"Resource temporarily unavailable.\r\nThis error is returned from operations on nonblocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a nonfatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a nonblocking SOCK_STREAM socket, since some time must elapse for the connection to be established.");
		break;
	case WSAEINPROGRESS:
		wcscpy_s(str, n, L"Operation now in progress.\r\nA blocking operation is currently executing. Windows Sockets only allows a single blocking operationóper- task or threadóto be outstanding, and if any other function call is made (whether or not it references that or any other socket) the function fails with the WSAEINPROGRESS error.");
		break;
	case WSAEALREADY:
		wcscpy_s(str, n, L"Operation already in progress.\r\nAn operation was attempted on a nonblocking socket with an operation already in progressóthat is, calling connect a second time on a nonblocking socket that is already connecting, or canceling an asynchronous request (WSAAsyncGetXbyY) that has already been canceled or completed.");
		break;
	case WSAENOTSOCK:
		wcscpy_s(str, n, L"Socket operation on nonsocket.\r\nAn operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid.");
		break;
	case WSAEDESTADDRREQ:
		wcscpy_s(str, n, L"Destination address required.\r\nA required address was omitted from an operation on a socket. For example, this error is returned if sendto is called with the remote address of ADDR_ANY.");
		break;
	case WSAEMSGSIZE:
		wcscpy_s(str, n, L"Message too long.\r\nA message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram was smaller than the datagram itself.");
		break;
	case WSAEPROTOTYPE:
		wcscpy_s(str, n, L"Protocol wrong type for socket.\r\nA protocol was specified in the socket function call that does not support the semantics of the socket type requested. For example, the ARPA Internet UDP protocol cannot be specified with a socket type of SOCK_STREAM.");
		break;
	case WSAENOPROTOOPT:
		wcscpy_s(str, n, L"Bad protocol option.\r\nAn unknown, invalid or unsupported option or level was specified in a getsockopt or setsockopt call.");
		break;
	case WSAEPROTONOSUPPORT:
		wcscpy_s(str, n, L"Protocol not supported.\r\nThe requested protocol has not been configured into the system, or no implementation for it exists. For example, a socket call requests a SOCK_DGRAM socket, but specifies a stream protocol.");
		break;
	case WSAESOCKTNOSUPPORT:
		wcscpy_s(str, n, L"Socket type not supported.\r\nThe support for the specified socket type does not exist in this address family. For example, the optional type SOCK_RAW might be selected in a socket call, and the implementation does not support SOCK_RAW sockets at all.");
		break;
	case WSAEOPNOTSUPP:
		wcscpy_s(str, n, L"Operation not supported.\r\nThe attempted operation is not supported for the type of object referenced. Usually this occurs when a socket descriptor to a socket that cannot support this operation is trying to accept a connection on a datagram socket.");
		break;
	case WSAEPFNOSUPPORT:
		wcscpy_s(str, n, L"Protocol family not supported.\r\nThe protocol family has not been configured into the system or no implementation for it exists. This message has a slightly different meaning from WSAEAFNOSUPPORT. However, it is interchangeable in most cases, and all Windows Sockets functions that return one of these messages also specify WSAEAFNOSUPPORT.");
		break;
	case WSAEAFNOSUPPORT:
		wcscpy_s(str, n, L"Address family not supported by protocol family.\r\nAn address incompatible with the requested protocol was used. All sockets are created with an associated address family (that is, AF_INET for Internet Protocols) and a generic protocol type (that is, SOCK_STREAM). This error is returned if an incorrect protocol is explicitly requested in the socket call, or if an address of the wrong family is used for a socket, for example, in sendto.");
		break;
	case WSAEADDRINUSE:
		wcscpy_s(str, n, L"Address already in use.\r\nTypically, only one usage of each socket address (protocol/IP address/port) is permitted. This error occurs if an application attempts to bind a socket to an IP address/port that has already been used for an existing socket, or a socket that was not closed properly, or one that is still in the process of closing. For server applications that need to bind multiple sockets to the same port number, consider using setsockopt (SO_REUSEADDR). Client applications usually need not call bind at allóconnect chooses an unused port automatically. When bind is called with a wildcard address (involving ADDR_ANY), a WSAEADDRINUSE error could be delayed until the specific address is committed. This could happen with a call to another function later, including connect, listen, WSAConnect, or WSAJoinLeaf.");
		break;
	case WSAEADDRNOTAVAIL:
		wcscpy_s(str, n, L"Cannot assign requested address.\r\nThe requested address is not valid in its context. This normally results from an attempt to bind to an address that is not valid for the local computer. This can also result from connect, sendto, WSAConnect, WSAJoinLeaf, or WSASendTo when the remote address or port is not valid for a remote computer (for example, address or port 0).");
		break;
	case WSAENETDOWN:
		wcscpy_s(str, n, L"Network is down.\r\nA socket operation encountered a dead network. This could indicate a serious failure of the network system (that is, the protocol stack that the Windows Sockets DLL runs over), the network interface, or the local network itself.");
		break;
	case WSAENETUNREACH:
		wcscpy_s(str, n, L"Network is unreachable.\r\nA socket operation was attempted to an unreachable network. This usually means the local software knows no route to reach the remote host.");
		break;
	case WSAENETRESET:
		wcscpy_s(str, n, L"Network dropped connection on reset.\r\nThe connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. It can also be returned by setsockopt if an attempt is made to set SO_KEEPALIVE on a connection that has already failed.");
		break;
	case WSAECONNABORTED:
		wcscpy_s(str, n, L"Software caused connection abort.\r\nAn established connection was aborted by the software in your host computer, possibly due to a data transmission time-out or protocol error.");
		break;
	case WSAECONNRESET:
		wcscpy_s(str, n, L"Connection reset by peer.\r\nAn existing connection was forcibly closed by the remote host. This normally results if the peer application on the remote host is suddenly stopped, the host is rebooted, the host or remote network interface is disabled, or the remote host uses a hard close (see setsockopt for more information on the SO_LINGER option on the remote socket). This error may also result if a connection was broken due to keep-alive activity detecting a failure while one or more operations are in progress. Operations that were in progress fail with WSAENETRESET. Subsequent operations fail with WSAECONNRESET.");
		break;
	case WSAENOBUFS:
		wcscpy_s(str, n, L"No buffer space available.\r\nAn operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.");
		break;
	case WSAEISCONN:
		wcscpy_s(str, n, L"Socket is already connected.\r\nA connect request was made on an already-connected socket. Some implementations also return this error if sendto is called on a connected SOCK_DGRAM socket (for SOCK_STREAM sockets, the to parameter in sendto is ignored) although other implementations treat this as a legal occurrence.");
		break;
	case WSAENOTCONN:
		wcscpy_s(str, n, L"Socket is not connected.\r\nA request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using sendto) no address was supplied. Any other type of operation might also return this errorófor example, setsockopt setting SO_KEEPALIVE if the connection has been reset.");
		break;
	case WSAESHUTDOWN:
		wcscpy_s(str, n, L"Cannot send after socket shutdown.\r\nA request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call. By calling shutdown a partial close of a socket is requested, which is a signal that sending or receiving, or both have been discontinued.");
		break;
	case WSAETOOMANYREFS:
		wcscpy_s(str, n, L"Too many references.\r\nToo many references to some kernel object.");
		break;
	case WSAETIMEDOUT:
		wcscpy_s(str, n, L"Connection timed out.\r\nA connection attempt failed because the connected party did not properly respond after a period of time, or the established connection failed because the connected host has failed to respond.");
		break;
	case WSAECONNREFUSED:
		wcscpy_s(str, n, L"Connection refused.\r\nNo connection could be made because the target computer actively refused it. This usually results from trying to connect to a service that is inactive on the foreign hostóthat is, one with no server application running.");
		break;
	case WSAELOOP:
		wcscpy_s(str, n, L"Cannot translate name.\r\nCannot translate a name.");
		break;
	case WSAENAMETOOLONG:
		wcscpy_s(str, n, L"Name too long.\r\nA name component or a name was too long.");
		break;
	case WSAEHOSTDOWN:
		wcscpy_s(str, n, L"Host is down.\r\nA socket operation failed because the destination host is down. A socket operation encountered a dead host. Networking activity on the local host has not been initiated. These conditions are more likely to be indicated by the error WSAETIMEDOUT.");
		break;
	case WSAEHOSTUNREACH:
		wcscpy_s(str, n, L"No route to host.\r\nA socket operation was attempted to an unreachable host. See WSAENETUNREACH.");
		break;
	case WSAENOTEMPTY:
		wcscpy_s(str, n, L"Directory not empty.\r\nCannot remove a directory that is not empty.");
		break;
	case WSAEPROCLIM:
		wcscpy_s(str, n, L"Too many processes.\r\nA Windows Sockets implementation may have a limit on the number of applications that can use it simultaneously. WSAStartup may fail with this error if the limit has been reached.");
		break;
	case WSAEUSERS:
		wcscpy_s(str, n, L"User quota exceeded.\r\nRan out of user quota.");
		break;
	case WSAEDQUOT:
		wcscpy_s(str, n, L"Disk quota exceeded.\r\nRan out of disk quota.");
		break;
	case WSAESTALE:
		wcscpy_s(str, n, L"Stale file handle reference.\r\nThe file handle reference is no longer available.");
		break;
	case WSAEREMOTE:
		wcscpy_s(str, n, L"Item is remote.\r\nThe item is not available locally.");
		break;
	case WSASYSNOTREADY:
		wcscpy_s(str, n, L"Network subsystem is unavailable.\r\nThis error is returned by WSAStartup if the Windows Sockets implementation cannot function at this time because the underlying system it uses to provide network services is currently unavailable. Users should check:\r\nThat the appropriate Windows Sockets DLL file is in the current path.\r\nThat they are not trying to use more than one Windows Sockets implementation simultaneously. If there is more than one Winsock DLL on your system, be sure the first one in the path is appropriate for the network subsystem currently loaded.\r\nThe Windows Sockets implementation documentation to be sure all necessary components are currently installed and configured correctly.");
		break;
	case WSAVERNOTSUPPORTED:
		wcscpy_s(str, n, L"Winsock.dll version out of range.\r\nThe current Windows Sockets implementation does not support the Windows Sockets specification version requested by the application. Check that no old Windows Sockets DLL files are being accessed.");
		break;
	case WSANOTINITIALISED:
		wcscpy_s(str, n, L"Successful WSAStartup not yet performed.\r\nEither the application has not called WSAStartup or WSAStartup failed. The application may be accessing a socket that the current active task does not own (that is, trying to share a socket between tasks), or WSACleanup has been called too many times.");
		break;
	case WSAEDISCON:
		wcscpy_s(str, n, L"Graceful shutdown in progress.\r\nReturned by WSARecv and WSARecvFrom to indicate that the remote party has initiated a graceful shutdown sequence.");
		break;
	case WSAENOMORE:
		wcscpy_s(str, n, L"No more results.\r\nNo more results can be returned by the WSALookupServiceNext function.");
		break;
	case WSAECANCELLED:
		wcscpy_s(str, n, L"Call has been canceled.\r\nA call to the WSALookupServiceEnd function was made while this call was still processing. The call has been canceled.");
		break;
	case WSAEINVALIDPROCTABLE:
		wcscpy_s(str, n, L"Procedure call table is invalid.\r\nThe service provider procedure call table is invalid. A service provider returned a bogus procedure table to Ws2_32.dll. This is usually caused by one or more of the function pointers being NULL.");
		break;
	case WSAEINVALIDPROVIDER:
		wcscpy_s(str, n, L"Service provider is invalid.\r\nThe requested service provider is invalid. This error is returned by the WSCGetProviderInfo and WSCGetProviderInfo32 functions if the protocol entry specified could not be found. This error is also returned if the service provider returned a version number other than 2.0.");
		break;
	case WSAEPROVIDERFAILEDINIT:
		wcscpy_s(str, n, L"Service provider failed to initialize.\r\nThe requested service provider could not be loaded or initialized. This error is returned if either a service provider's DLL could not be loaded (LoadLibrary failed) or the provider's WSPStartup or NSPStartup function failed.");
		break;
	case WSASYSCALLFAILURE:
		wcscpy_s(str, n, L"System call failure.\r\nA system call that should never fail has failed. This is a generic error code, returned under various conditions.\r\nReturned when a system call that should never fail does fail. For example, if a call to WaitForMultipleEvents fails or one of the registry functions fails trying to manipulate the protocol/namespace catalogs.\r\nReturned when a provider does not return SUCCESS and does not provide an extended error code. Can indicate a service provider implementation error.");
		break;
	case WSASERVICE_NOT_FOUND:
		wcscpy_s(str, n, L"Service not found.\r\nNo such service is known. The service cannot be found in the specified name space.");
		break;
	case WSATYPE_NOT_FOUND:
		wcscpy_s(str, n, L"Class type not found.\r\nThe specified class was not found.");
		break;
	case WSA_E_NO_MORE:
		wcscpy_s(str, n, L"No more results.\r\nNo more results can be returned by the WSALookupServiceNext function.");
		break;
	case WSA_E_CANCELLED:
		wcscpy_s(str, n, L"Call was canceled.\r\nA call to the WSALookupServiceEnd function was made while this call was still processing. The call has been canceled.");
		break;
	case WSAEREFUSED:
		wcscpy_s(str, n, L"Database query was refused.\r\nA database query failed because it was actively refused.");
		break;
	case WSAHOST_NOT_FOUND:
		wcscpy_s(str, n, L"Host not found.\r\nNo such host is known. The name is not an official host name or alias, or it cannot be found in the database(s) being queried. This error may also be returned for protocol and service queries, and means that the specified name could not be found in the relevant database.");
		break;
	case WSATRY_AGAIN:
		wcscpy_s(str, n, L"Nonauthoritative host not found.\r\nThis is usually a temporary error during host name resolution and means that the local server did not receive a response from an authoritative server. A retry at some time later may be successful.");
		break;
	case WSANO_RECOVERY:
		wcscpy_s(str, n, L"This is a nonrecoverable error.\r\nThis indicates that some sort of nonrecoverable error occurred during a database lookup. This may be because the database files (for example, BSD-compatible HOSTS, SERVICES, or PROTOCOLS files) could not be found, or a DNS request was returned by the server with a severe error.");
		break;
	case WSANO_DATA:
		wcscpy_s(str, n, L"Valid name, no data record of requested type.\r\nThe requested name is valid and was found in the database, but it does not have the correct associated data being resolved for. The usual example for this is a host name-to-address translation attempt (using gethostbyname or WSAAsyncGetHostByName) which uses the DNS (Domain Name Server). An MX record is returned but no A recordóindicating the host itself exists, but is not directly reachable.");
		break;
	case WSA_QOS_RECEIVERS:
		wcscpy_s(str, n, L"QoS receivers.\r\nAt least one QoS reserve has arrived.");
		break;
	case WSA_QOS_SENDERS:
		wcscpy_s(str, n, L"QoS senders.\r\nAt least one QoS send path has arrived.");
		break;
	case WSA_QOS_NO_SENDERS:
		wcscpy_s(str, n, L"No QoS senders.\r\nThere are no QoS senders.");
		break;
	case WSA_QOS_NO_RECEIVERS:
		wcscpy_s(str, n, L"QoS no receivers.\r\nThere are no QoS receivers.");
		break;
	case WSA_QOS_REQUEST_CONFIRMED:
		wcscpy_s(str, n, L"QoS request confirmed.\r\nThe QoS reserve request has been confirmed.");
		break;
	case WSA_QOS_ADMISSION_FAILURE:
		wcscpy_s(str, n, L"QoS admission error.\r\nA QoS error occurred due to lack of resources.");
		break;
	case WSA_QOS_POLICY_FAILURE:
		wcscpy_s(str, n, L"QoS policy failure.\r\nThe QoS request was rejected because the policy system couldn't allocate the requested resource within the existing policy.");
		break;
	case WSA_QOS_BAD_STYLE:
		wcscpy_s(str, n, L"QoS bad style.\r\nAn unknown or conflicting QoS style was encountered.");
		break;
	case WSA_QOS_BAD_OBJECT:
		wcscpy_s(str, n, L"QoS bad object.\r\nA problem was encountered with some part of the filterspec or the provider-specific buffer in general.");
		break;
	case WSA_QOS_TRAFFIC_CTRL_ERROR:
		wcscpy_s(str, n, L"QoS traffic control error.\r\nAn error with the underlying traffic control (TC) API as the generic QoS request was converted for local enforcement by the TC API. This could be due to an out of memory error or to an internal QoS provider error.");
		break;
	case WSA_QOS_GENERIC_ERROR:
		wcscpy_s(str, n, L"QoS generic error.\r\nA general QoS error.");
		break;
	case WSA_QOS_ESERVICETYPE:
		wcscpy_s(str, n, L"QoS service type error.\r\nAn invalid or unrecognized service type was found in the QoS flowspec.");
		break;
	case WSA_QOS_EFLOWSPEC:
		wcscpy_s(str, n, L"QoS flowspec error.\r\nAn invalid or inconsistent flowspec was found in the QOS structure.");
		break;
	case WSA_QOS_EPROVSPECBUF:
		wcscpy_s(str, n, L"Invalid QoS provider buffer.\r\nAn invalid QoS provider-specific buffer.");
		break;
	case WSA_QOS_EFILTERSTYLE:
		wcscpy_s(str, n, L"Invalid QoS filter style.\r\nAn invalid QoS filter style was used.");
		break;
	case WSA_QOS_EFILTERTYPE:
		wcscpy_s(str, n, L"Invalid QoS filter type.\r\nAn invalid QoS filter type was used.");
		break;
	case WSA_QOS_EFILTERCOUNT:
		wcscpy_s(str, n, L"Incorrect QoS filter count.\r\nAn incorrect number of QoS FILTERSPECs were specified in the FLOWDESCRIPTOR.");
		break;
	case WSA_QOS_EOBJLENGTH:
		wcscpy_s(str, n, L"Invalid QoS object length.\r\nAn object with an invalid ObjectLength field was specified in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_EFLOWCOUNT:
		wcscpy_s(str, n, L"Incorrect QoS flow count.\r\nAn incorrect number of flow descriptors was specified in the QoS structure.");
		break;
	case WSA_QOS_EUNKOWNPSOBJ:
		wcscpy_s(str, n, L"Unrecognized QoS object.\r\nAn unrecognized object was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_EPOLICYOBJ:
		wcscpy_s(str, n, L"Invalid QoS policy object.\r\nAn invalid policy object was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_EFLOWDESC:
		wcscpy_s(str, n, L"Invalid QoS flow descriptor.\r\nAn invalid QoS flow descriptor was found in the flow descriptor list.");
		break;
	case WSA_QOS_EPSFLOWSPEC:
		wcscpy_s(str, n, L"Invalid QoS provider-specific flowspec.\r\nAn invalid or inconsistent flowspec was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_EPSFILTERSPEC:
		wcscpy_s(str, n, L"Invalid QoS provider-specific filterspec.\r\nAn invalid FILTERSPEC was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_ESDMODEOBJ:
		wcscpy_s(str, n, L"Invalid QoS shape discard mode object.\r\nAn invalid shape discard mode object was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_ESHAPERATEOBJ:
		wcscpy_s(str, n, L"Invalid QoS shaping rate object.\r\nAn invalid shaping rate object was found in the QoS provider-specific buffer.");
		break;
	case WSA_QOS_RESERVED_PETYPE:
		wcscpy_s(str, n, L"Reserved policy QoS element type.\r\nA reserved policy element was found in the QoS provider-specific buffer.");
		break;
	default:
		swprintf_s(str, n, L"Hindi ko alam kung saan ang mali. Error code %d", err);
	}

	return str;
}
