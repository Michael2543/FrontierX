#include "Include/NativeAppGlobals.h"
#include "NetBuffer.h"
#include "NetPacket.h"
#include "Startup.h"
#include "UserDefaults.h"

#include "DACOM.H"
#include "EventSys.h"
#include "FDump.h"
#include "FrontierX/NativeUserDefaults.h"
#include "HeapObj.h"
#include "IConnection.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>

void CheckMovieCameraSaveRoundTrip();
const DWORD * __fastcall dwordsearch(DWORD len, DWORD value, const DWORD *buffer);

namespace
{
int DACOM_CDECL PacketDiagnostic(ErrorCode, const C8 *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	const int result = std::vfprintf(stderr, format, arguments);
	va_end(arguments);
	return result;
}

void Require(bool condition, const char *message)
{
	if (!condition) throw std::runtime_error(message);
}

struct LocalPacket : BASE_PACKET
{
	U32 marker;
	LocalPacket(U32 value) : marker(value)
	{
		dwSize = sizeof(*this);
		type = PT_USRMOVE;
	}
};

struct PacketObserver : IEventCallback
{
	GENRESULT QueryInterface(const C8 *name, void **result) override
	{
		if (!result) return GR_INVALID_PARMS;
		*result = nullptr;
		if (name && (std::strcmp(name, "IEventCallback") == 0 ||
			std::strcmp(name, IID_IEventCallback) == 0))
		{
			*result = static_cast<IEventCallback *>(this);
			AddRef();
			return GR_OK;
		}
		return GR_INTERFACE_UNSUPPORTED;
	}
	U32 AddRef() override { return ++references; }
	U32 Release() override { return --references; }
	GENRESULT Notify(U32 message, void *parameter) override
	{
		if (message == CQE_NETPACKET)
		{
			const auto *packet = static_cast<const BASE_PACKET *>(parameter);
			if (packet && packet->type == PT_USRMOVE &&
				packet->dwSize == sizeof(LocalPacket))
			{
				++moves;
				std::memcpy(&marker,
					reinterpret_cast<const U8 *>(packet) + sizeof(BASE_PACKET),
					sizeof(marker));
			}
		}
		return GR_OK;
	}
	U32 references = 0;
	U32 moves = 0;
	U32 marker = 0;
};
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = PacketDiagnostic;
}

int main(int argc, char **argv)
{
	try
	{
		Require(argc == 2, "expected isolated defaults path");
		const std::filesystem::path settings(argv[1]);
		std::filesystem::remove(settings);
		DACOM = DACOM_Acquire();
		Require(DACOM && InitializeDAHeap(0x10000, 0x4000,
			DAHEAPFLAG_MULTITHREADED | DAHEAPFLAG_GROWHEAP |
			DAHEAPFLAG_NOMSGS) == TRUE, "native DACOM heap initialization failed");

		FrontierX::StartupPreferences preferences;
		preferences.selection.source = FrontierX::SettingsPathSource::XdgConfigHome;
		FrontierX::RegisterNativeUserDefaults(DACOM, settings, preferences);
		DACOMDESC descriptor("IUserDefaults");
		Require(DACOM->CreateInstance(&descriptor,
			reinterpret_cast<void **>(&DEFAULTS)) == GR_OK && DEFAULTS &&
			DEFAULTS->LoadDefaults() == TRUE, "native user defaults failed");
		RegisterTheEventSystem(DACOM);
		AGGDESC eventDescriptor("IEventSystem", "EventSystem");
		void *rawEvent = nullptr;
		Require(DACOM->CreateInstance(&eventDescriptor, &rawEvent) == GR_OK && rawEvent,
			"production EventSystem creation failed");
		auto *eventAggregate = static_cast<IAggregateComponent *>(rawEvent);
		Require(eventAggregate->QueryInterface(IID_IEventSystem,
			reinterpret_cast<void **>(&EVENTSYS)) == GR_OK && EVENTSYS &&
			eventAggregate->Initialize() == GR_OK && EVENTSYS->Initialize(0) == GR_OK,
			"production EventSystem initialization failed");

		CreateGlobalComponents();
		Require(NETBUFFER && NETPACKET && NETBUFFER->Initialize() == TRUE,
			"production packet owners did not register");
		Require(CAMERAMANAGER != nullptr,
			"production camera manager did not register");
		CheckMovieCameraSaveRoundTrip();
		NETBUFFER->SetMinLatency(0);
		Require(PLAYERID == 0 && HOSTID == 0,
			"offline player and host IDs changed");
		const DWORD selected[] = {7U, 11U, 7U};
		Require(dwordsearch(3U, 7U, selected) == selected &&
			dwordsearch(2U, 7U, selected + 1) == selected + 2 &&
			dwordsearch(3U, 12U, selected) == nullptr &&
			dwordsearch(0U, 7U, selected) == nullptr,
			"native packet/object-set first-match search changed");

		for (U32 marker : {0x12345678U, 0xabcdef01U})
		{
			LocalPacket packet(marker);
			NETBUFFER->Send(PLAYERID, 0, &packet);
		}
		for (U32 marker : {0x12345678U, 0xabcdef01U})
		{
			BASE_PACKET *packet = nullptr;
			Require(NETBUFFER->Receive(0, 0, &packet) == TRUE && packet,
				"local packet missing from production queue");
			static_assert(sizeof(LocalPacket) == sizeof(BASE_PACKET) + sizeof(U32),
				"local packet payload offset changed");
			U32 receivedMarker = 0;
			std::memcpy(&receivedMarker,
				reinterpret_cast<const U8 *>(packet) + sizeof(BASE_PACKET),
				sizeof(receivedMarker));
			Require(packet->dwSize == sizeof(LocalPacket) &&
				packet->type == PT_USRMOVE && packet->fromID == PLAYERID &&
				receivedMarker == marker, "local packet order or payload changed");
			Require(NETBUFFER->FreePacket(packet) == TRUE,
				"production queue packet release failed");
		}
		BASE_PACKET *extra = nullptr;
		Require(NETBUFFER->Receive(0, 0, &extra) == FALSE,
			"production queue retained an extra packet");

		IDAConnectionPoint *point = nullptr;
		Require(EVENTSYS->QueryOutgoingInterface("IEventCallback", &point) == GR_OK &&
			point, "production packet event point unavailable");
		PacketObserver observer;
		U32 observerToken = 0;
		Require(point->Advise(&observer, &observerToken) == GR_OK && observerToken,
			"packet observer subscription failed");
		LocalPacket echoed(0xdeadbeefU);
		NETPACKET->Send(0, NETF_ECHO, &echoed);
		Require(EVENTSYS->Send(CQE_UPDATE, reinterpret_cast<void *>(1024)) == GR_OK,
			"production packet update dispatch failed");
		Require(observer.moves == 1 && observer.marker == echoed.marker,
			"NETF_ECHO did not reach CQE_NETPACKET with intact payload");
		Require(EVENTSYS->Send(CQE_MISSION_CLOSE, nullptr) == GR_OK,
			"production camera manager mission-close event failed");
		Require(point->Unadvise(observerToken) == GR_OK,
			"packet observer cleanup failed");
		point->Release();
		CleanupGlobals();
		Require(!NETBUFFER && !NETPACKET && !CAMERAMANAGER,
			"production packet cleanup left a global slot");
		EVENTSYS->Release();
		EVENTSYS = nullptr;
		eventAggregate->Release();
		DEFAULTS->Release();
		DEFAULTS = nullptr;
		DACOM = nullptr;
		std::filesystem::remove(settings);
		std::cout << "production offline NetBuffer queue delivered two packets in order; NETF_ECHO dispatched one CQE_NETPACKET; camera manager save/load round trip and malformed lengths checked\n";
		return 0;
	}
	catch (const std::exception &error)
	{
		std::cerr << "production NetBuffer probe failed: " << error.what() << '\n';
		return 1;
	}
}
