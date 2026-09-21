#include "EventSys.h"
#include "Filesys.h"
#include "FrontierX/LegacyInput.h"
#include "FrontierX/LegacyJoystick.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "HKEvent.h"
#include "HkeyRec.h"
#include "HeapObj.h"
#include "IConnection.h"
#include "SYSTEM.H"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

void RegisterHotkeyEventServer(ICOManager * manager);

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

void AppendU32(std::vector<U8> & bytes, U32 value)
{
    for (U32 shift = 0; shift != 32; shift += 8)
        bytes.push_back(static_cast<U8>((value >> shift) & 0xffU));
}

void AppendHotkey(std::vector<U8> & bytes, U32 virtualKey, U32 symbol)
{
    AppendU32(bytes, 0);
    AppendU32(bytes, virtualKey | (1U << 10U) | (symbol << 11U));
}

struct MemoryFileSystem final : IFileSystem
{
    struct Child
    {
        std::string name;
        std::vector<U8> bytes;
        std::size_t position = 0;
        bool open = false;
    };

    explicit MemoryFileSystem(std::vector<Child> initialChildren)
        : children(std::move(initialChildren))
    {
    }

    GENRESULT QueryInterface(const C8 * name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (name && (std::strcmp(name, IID_IFileSystem) == 0 ||
            std::strcmp(name, IID_IComponentFactory) == 0 ||
            std::strcmp(name, IID_IDAComponent) == 0))
        {
            *instance = static_cast<IFileSystem *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    GENRESULT CreateInstance(DACOMDESC *, void ** instance) override
    {
        if (instance) *instance = nullptr;
        return GR_NOT_IMPLEMENTED;
    }

    BOOL CloseHandle(HANDLE handle) override
    {
        Child * child = Resolve(handle);
        if (!child || !child->open) return FALSE;
        child->open = false;
        return TRUE;
    }

    BOOL ReadFile(HANDLE handle, LPVOID buffer, DWORD count, LPDWORD bytesRead,
                  LPOVERLAPPED) override
    {
        if (bytesRead) *bytesRead = 0;
        Child * child = Resolve(handle);
        if (!child || !child->open || (count != 0 && !buffer)) return FALSE;
        const std::size_t available = child->bytes.size() - child->position;
        const std::size_t copied = std::min<std::size_t>(available, count);
        if (copied != 0)
            std::memcpy(buffer, child->bytes.data() + child->position, copied);
        child->position += copied;
        if (bytesRead) *bytesRead = static_cast<DWORD>(copied);
        return TRUE;
    }

    BOOL WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) override { return FALSE; }
    BOOL GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL) override { return FALSE; }
    DWORD SetFilePointer(HANDLE, LONG, PLONG, DWORD) override { return 0xffffffffU; }
    BOOL SetEndOfFile(HANDLE) override { return FALSE; }
    DWORD GetFileSize(HANDLE handle, LPDWORD high) override
    {
        if (high) *high = 0;
        Child * child = Resolve(handle);
        return child && child->open ? static_cast<DWORD>(child->bytes.size()) : 0xffffffffU;
    }
    BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL GetFileTime(HANDLE, LPFILETIME, LPFILETIME, LPFILETIME) override { return FALSE; }
    BOOL SetFileTime(HANDLE, const FILETIME *, const FILETIME *, const FILETIME *) override
    {
        return FALSE;
    }
    HANDLE CreateFileMapping(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD,
                             LPCTSTR) override { return INVALID_HANDLE_VALUE; }
    LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return nullptr; }
    BOOL UnmapViewOfFile(LPCVOID) override { return FALSE; }
    HANDLE FindFirstFile(LPCTSTR, LPWIN32_FIND_DATA) override { return INVALID_HANDLE_VALUE; }
    BOOL FindNextFile(HANDLE, LPWIN32_FIND_DATA) override { return FALSE; }
    BOOL FindClose(HANDLE) override { return FALSE; }
    BOOL CreateDirectory(LPCTSTR, LPSECURITY_ATTRIBUTES) override { return FALSE; }
    BOOL RemoveDirectory(LPCTSTR) override { return FALSE; }
    DWORD GetCurrentDirectory(DWORD, LPTSTR) override { return 0; }
    BOOL SetCurrentDirectory(LPCTSTR) override { return FALSE; }
    BOOL DeleteFile(LPCTSTR) override { return FALSE; }
    BOOL CopyFile(LPCTSTR, LPCTSTR, BOOL) override { return FALSE; }
    BOOL MoveFile(LPCTSTR, LPCTSTR) override { return FALSE; }
    DWORD GetFileAttributes(LPCTSTR) override { return 0xffffffffU; }
    BOOL SetFileAttributes(LPCTSTR, DWORD) override { return FALSE; }
    DWORD GetLastError() override { return 0; }

    HANDLE OpenChild(DAFILEDESC * descriptor) override
    {
        if (!descriptor || !descriptor->lpFileName) return INVALID_HANDLE_VALUE;
        const auto found = std::find_if(children.begin(), children.end(),
            [descriptor](const Child & child)
            {
                return child.name == descriptor->lpFileName;
            });
        if (found == children.end() || found->open) return INVALID_HANDLE_VALUE;
        found->position = 0;
        found->open = true;
        return &*found;
    }
    DWORD GetFilePosition(HANDLE handle, PLONG high) override
    {
        if (high) *high = 0;
        Child * child = Resolve(handle);
        return child ? static_cast<DWORD>(child->position) : 0xffffffffU;
    }
    LONG GetFileName(LPSTR, LONG) override { return 0; }
    DWORD GetAccessType() override { return GENERIC_READ; }
    GENRESULT GetParentSystem(LPFILESYSTEM * parent) override
    {
        if (!parent) return GR_INVALID_PARMS;
        *parent = nullptr;
        return GR_DATA_NOT_FOUND;
    }
    GENRESULT SetPreference(DWORD, DWORD) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT GetPreference(DWORD, PDWORD) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT ReadDirectoryExtension(HANDLE, LPVOID, DWORD, LPDWORD, DWORD) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT WriteDirectoryExtension(HANDLE, LPCVOID, DWORD, LPDWORD, DWORD) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    LONG SerialCall(LPFILESYSTEM, DAFILE_SERIAL_PROC, void *) override { return 0; }
    BOOL GetAbsolutePath(char *, LPCTSTR, LONG) override { return FALSE; }

    Child * Resolve(HANDLE handle)
    {
        if (handle == INVALID_HANDLE_VALUE || handle == nullptr) return nullptr;
        auto * candidate = static_cast<Child *>(handle);
        for (Child & child : children)
            if (&child == candidate) return &child;
        return nullptr;
    }

    std::vector<Child> children;
    U32 references = 0;
};

std::vector<MemoryFileSystem::Child> ValidFiles()
{
    std::vector<U8> version;
    AppendU32(version, 0x101U);

    std::vector<U8> binary;
    AppendU32(binary, 3);
    AppendU32(binary, 8);
    AppendHotkey(binary, 'A', 1);
    AppendHotkey(binary, HKR_VKEY_MOUSELEFT, 2);
    AppendHotkey(binary, HKR_VKEY_JOY0, 3);

    std::vector<U8> text;
    AppendU32(text, 3);
    AppendU32(text, 16);
    for (const char * value : {"Keyboard A", "Mouse left", "Joystick one"})
    {
        std::array<U8, 16> slot{};
        std::memcpy(slot.data(), value, std::strlen(value));
        text.insert(text.end(), slot.begin(), slot.end());
    }
    return {{"Version", std::move(version)}, {"Binary data", std::move(binary)},
            {"Keyboard text", std::move(text)}};
}

BOOL32 COMAPI CountHeapBlock(IHeap *, void *, U32, void * context)
{
    ++*static_cast<U32 *>(context);
    return TRUE;
}

U32 HeapBlocks()
{
    U32 blocks = 0;
    Require(HEAP->EnumerateBlocks(CountHeapBlock, &blocks) == TRUE,
        "shared heap enumeration failed");
    return blocks;
}

struct ProbeCallback final : IEventCallback
{
    GENRESULT QueryInterface(const C8 * name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (name && (std::strcmp(name, "IEventCallback") == 0 ||
            std::strcmp(name, IID_IEventCallback) == 0))
        {
            *instance = static_cast<IEventCallback *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }
    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    GENRESULT Notify(U32 message, void * parameter) override
    {
        messages.push_back(message);
        symbols.push_back(reinterpret_cast<std::uintptr_t>(parameter));
        return GR_OK;
    }
    std::vector<U32> messages;
    std::vector<std::uintptr_t> symbols;
    U32 references = 0;
};

GENRESULT CreateHotkey(FrontierX::NativeComponentManager & manager, IFileSystem & files,
                       IAggregateComponent ** aggregate, IHotkeyEvent ** hotkey)
{
    HKEVENTDESC descriptor;
    descriptor.file = &files;
    descriptor.joyMessage = 0x500U;
    descriptor.hotkeyMessage = 0x501U;
    void * raw = nullptr;
    const GENRESULT result = manager.CreateInstance(&descriptor, &raw);
    if (result != GR_OK) return result;
    *aggregate = static_cast<IAggregateComponent *>(raw);
    const GENRESULT query = (*aggregate)->QueryInterface(IID_IHotkeyEvent,
        reinterpret_cast<void **>(hotkey));
    if (query != GR_OK)
    {
        (*aggregate)->Release();
        *aggregate = nullptr;
    }
    return query;
}

void RequireRejected(FrontierX::NativeComponentManager & manager,
                     std::vector<MemoryFileSystem::Child> files, const char * message)
{
    const U32 before = HeapBlocks();
    MemoryFileSystem fileSystem(std::move(files));
    HKEVENTDESC descriptor;
    descriptor.file = &fileSystem;
    void * instance = nullptr;
    Require(manager.CreateInstance(&descriptor, &instance) != GR_OK && instance == nullptr,
        message);
    Require(HeapBlocks() == before, "rejected hotkey data leaked shared-heap blocks");
}

void ValidateRealArchive(FrontierX::NativeComponentManager & manager,
                         const std::filesystem::path & path)
{
    const U32 before = HeapBlocks();
    IFileSystem * files = nullptr;
    std::string error;
    Require(FrontierX::OpenNativeUtfFileSystem(path, &files, error) && files,
        "checked-in hotkey UTF did not open through the native file-system adapter");

    DAFILEDESC versionDescriptor("Version");
    HANDLE version = files->OpenChild(&versionDescriptor);
    Require(version != INVALID_HANDLE_VALUE && files->GetFileSize(version) == sizeof(U32),
        "native UTF adapter did not expose the version child");
    U32 value = 0;
    DWORD bytesRead = 0;
    Require(files->ReadFile(version, &value, sizeof(value), &bytesRead, nullptr) == TRUE &&
        bytesRead == sizeof(value) && value == HKEVENT_VERSION &&
        files->GetFilePosition(version) == sizeof(value),
        "native UTF adapter did not read the version child sequentially");
    Require(files->SetFilePointer(version, 0, nullptr, FILE_BEGIN) == 0 &&
        files->GetFilePosition(version) == 0,
        "native UTF adapter did not seek within the child");
    Require(files->WriteFile(version, &value, sizeof(value), &bytesRead, nullptr) == FALSE,
        "read-only native UTF adapter accepted a write");
    Require(files->CloseHandle(version) == TRUE &&
        files->ReadFile(version, &value, sizeof(value), &bytesRead, nullptr) == FALSE,
        "native UTF adapter accepted a stale child handle");
    DAFILEDESC missingDescriptor("No such child");
    Require(files->OpenChild(&missingDescriptor) == INVALID_HANDLE_VALUE,
        "native UTF adapter opened a missing child");

    IAggregateComponent * aggregate = nullptr;
    IHotkeyEvent * hotkey = nullptr;
    Require(CreateHotkey(manager, *files, &aggregate, &hotkey) == GR_OK &&
        aggregate && hotkey, "production hotkey owner rejected a checked-in UTF table");
    char text[128]{};
    Require(hotkey->GetHotkeyText(1, text, sizeof(text)) != 0 && text[0] != '\0',
        "checked-in hotkey UTF did not expose its first keyboard label");
    hotkey->Release();
    aggregate->Release();
    files->Release();
    Require(HeapBlocks() == before,
        "checked-in hotkey UTF creation did not restore shared-heap accounting");
}
}

int main(int argc, char ** argv)
{
    try
    {
        Require(argc == 1 || argc == 3,
            "expected either no arguments or Hotkeys.utf and DBHotkeys.utf paths");
        Require(HEAP_Acquire() != nullptr, "native shared heap acquisition failed");
        const U32 heapBaseline = HeapBlocks();
        FrontierX::NativeComponentManager manager;
        RegisterHotkeyEventServer(&manager);
        RegisterTheEventSystem(&manager);
        Require(manager.RegisteredCount() == 2, "hotkey/event factory registration failed");

        MemoryFileSystem fileSystem(ValidFiles());
        IAggregateComponent * aggregate = nullptr;
        IHotkeyEvent * hotkey = nullptr;
        Require(CreateHotkey(manager, fileSystem, &aggregate, &hotkey) == GR_OK &&
            aggregate && hotkey, "production hotkey aggregate creation failed");
        Require(HeapBlocks() > heapBaseline,
            "production hotkey aggregate did not use the shared heap");
        Require(aggregate->Initialize() == GR_OK, "hotkey aggregate initialization failed");

        IEventCallback * inboundCallback = nullptr;
        Require(aggregate->QueryInterface(IID_IEventCallback,
            reinterpret_cast<void **>(&inboundCallback)) == GR_OK && inboundCallback,
            "hotkey inbound event callback query failed");

        IDAConnectionPoint * point = nullptr;
        Require(hotkey->QueryOutgoingInterface("IEventCallback", &point) == GR_OK && point,
            "hotkey callback connection point query failed");
        ProbeCallback callback;
        U32 token = 0;
        Require(point->Advise(&callback, &token) == GR_OK && token == 1,
            "hotkey callback subscription failed");

        char text[16]{};
        Require(hotkey->GetHotkeyText(1, text, sizeof(text)) == 10 &&
            std::strcmp(text, "Keyboard A") == 0,
            "hotkey text lookup changed");
        Require(hotkey->GetHotkeyText(0, text, sizeof(text)) == 0 &&
            hotkey->GetHotkeyText(4, text, sizeof(text)) == 0,
            "out-of-range hotkey text lookup was not rejected");

        hotkey->SystemMessage(0, FrontierX::LegacyMessage::KeyDown, 'A', 0);
        Require(callback.symbols == std::vector<std::uintptr_t>{1} &&
            callback.messages == std::vector<U32>{0x501U} &&
            hotkey->GetVkeyState('A') == HKEF_PRESSED &&
            hotkey->GetHotkeyState(1) == HKEF_PRESSED,
            "keyboard press did not produce the configured hotkey event");
        hotkey->SystemMessage(0, FrontierX::LegacyMessage::KeyUp, 'A',
            static_cast<FrontierX::LegacyLParam>(1U << 31U));
        Require(hotkey->GetVkeyState('A') == HKEF_RELEASED &&
            hotkey->GetHotkeyState(1) == HKEF_RELEASED,
            "keyboard release did not clear hotkey state");

        hotkey->SystemMessage(0, FrontierX::LegacyMessage::LeftButtonDown,
            FrontierX::LegacyMouseMask::Left, 0);
        Require(callback.symbols == std::vector<std::uintptr_t>({1, 2}),
            "mouse press did not produce the configured hotkey event");
        hotkey->SystemMessage(0, FrontierX::LegacyMessage::LeftButtonUp, 0, 0);

        FrontierX::LegacyJoystickState joystick;
        joystick.dwFlags = FrontierX::LegacyJoystickFlag::ReturnButtons;
        joystick.dwButtons = FrontierX::LegacyJoystickButton(1);
        hotkey->SystemMessage(0, 0x500U, 0, FrontierX::LegacyLParamFromPointer(&joystick));
        Require(callback.symbols == std::vector<std::uintptr_t>({1, 2, 3}),
            "joystick press did not produce the configured hotkey event");
        joystick.dwButtons = 0;
        Require(inboundCallback->Notify(0x500U, &joystick) == GR_OK,
            "joystick callback release failed");

        Require(hotkey->Disable() == GR_OK, "hotkey disable failed");
        hotkey->SystemMessage(0, FrontierX::LegacyMessage::KeyDown, 'A', 0);
        Require(callback.symbols.size() == 3 && hotkey->GetHotkeyState(1) == HKEF_PRESSED,
            "disabled hotkey service did not suppress callbacks while tracking state");
        hotkey->SystemMessage(0, FrontierX::LegacyMessage::KeyUp, 'A',
            static_cast<FrontierX::LegacyLParam>(1U << 31U));
        Require(hotkey->Enable() == GR_OK && hotkey->GetVkeyState(0x1000U) == HKEF_RELEASED,
            "hotkey enable or virtual-key range guard failed");

        AGGDESC eventDescriptor("IEventSystem", "EventSystem");
        void * rawEventAggregate = nullptr;
        Require(manager.CreateInstance(&eventDescriptor, &rawEventAggregate) == GR_OK &&
            rawEventAggregate, "event-system pipeline aggregate creation failed");
        auto * eventAggregate = static_cast<IAggregateComponent *>(rawEventAggregate);
        IEventSystem * eventSystem = nullptr;
        ISystemComponent * eventOwner = nullptr;
        IEventCallback * eventInput = nullptr;
        Require(eventAggregate->QueryInterface(IID_IEventSystem,
                reinterpret_cast<void **>(&eventSystem)) == GR_OK && eventSystem &&
            eventAggregate->QueryInterface(IID_ISystemComponent,
                reinterpret_cast<void **>(&eventOwner)) == GR_OK && eventOwner &&
            eventAggregate->QueryInterface(IID_IEventCallback,
                reinterpret_cast<void **>(&eventInput)) == GR_OK && eventInput &&
            eventAggregate->Initialize() == GR_OK && eventSystem->Initialize(0) == GR_OK,
            "event-system pipeline initialization failed");
        IDAConnectionPoint * eventPoint = nullptr;
        Require(eventSystem->QueryOutgoingInterface("IEventCallback", &eventPoint) == GR_OK &&
            eventPoint, "event-system pipeline connection query failed");
        ProbeCallback pipelineCallback;
        U32 hotkeyInputToken = 0;
        U32 pipelineCaptureToken = 0;
        U32 hotkeyOutputToken = 0;
        Require(eventPoint->Advise(inboundCallback, &hotkeyInputToken) == GR_OK &&
            eventPoint->Advise(&pipelineCallback, &pipelineCaptureToken) == GR_OK &&
            point->Advise(eventInput, &hotkeyOutputToken) == GR_OK,
            "hotkey/event-system pipeline subscription failed");
        FrontierX::LegacyWindowMessage keyMessage{
            0, FrontierX::LegacyMessage::KeyDown, 'A', 0};
        Require(eventSystem->Send(keyMessage.message, &keyMessage) == GR_OK &&
            pipelineCallback.messages == std::vector<U32>{FrontierX::LegacyMessage::KeyDown},
            "legacy input did not synchronously reach the hotkey pipeline");
        eventOwner->Update();
        Require(pipelineCallback.messages ==
                std::vector<U32>({FrontierX::LegacyMessage::KeyDown, 0x501U}) &&
            pipelineCallback.symbols.back() == 1,
            "translated hotkey symbol did not traverse the event-system queue");
        keyMessage.message = FrontierX::LegacyMessage::KeyUp;
        keyMessage.lparam = static_cast<FrontierX::LegacyLParam>(1U << 31U);
        Require(eventSystem->Send(keyMessage.message, &keyMessage) == GR_OK,
            "legacy key release did not traverse the event-system pipeline");
        eventOwner->Update();
        Require(point->Unadvise(hotkeyOutputToken) == GR_OK &&
            eventPoint->Unadvise(pipelineCaptureToken) == GR_OK &&
            eventPoint->Unadvise(hotkeyInputToken) == GR_OK,
            "hotkey/event-system pipeline unsubscribe failed");
        eventPoint->Release();
        eventInput->Release();
        eventOwner->Release();
        eventSystem->Release();
        eventAggregate->Release();

        Require(point->Unadvise(token) == GR_OK, "hotkey callback unsubscribe failed");
        point->Release();
        inboundCallback->Release();
        hotkey->Release();
        aggregate->Release();
        Require(HeapBlocks() == heapBaseline,
            "production hotkey aggregate did not return shared-heap allocations");

        auto emptyFiles = ValidFiles();
        emptyFiles[1].bytes.resize(8);
        std::fill(emptyFiles[1].bytes.begin(), emptyFiles[1].bytes.begin() + 4, U8{0});
        emptyFiles[2].bytes.resize(8);
        std::fill(emptyFiles[2].bytes.begin(), emptyFiles[2].bytes.begin() + 4, U8{0});
        MemoryFileSystem emptyFileSystem(std::move(emptyFiles));
        aggregate = nullptr;
        hotkey = nullptr;
        Require(CreateHotkey(manager, emptyFileSystem, &aggregate, &hotkey) == GR_OK &&
            aggregate && hotkey && hotkey->IsAnyHotkeyPressed() == FALSE,
            "empty hotkey table was not handled safely");
        hotkey->Release();
        aggregate->Release();
        Require(HeapBlocks() == heapBaseline, "empty hotkey table leaked shared-heap blocks");

        auto invalidStride = ValidFiles();
        invalidStride[1].bytes[4] = 7;
        RequireRejected(manager, std::move(invalidStride),
            "invalid hotkey record stride was accepted");
        auto truncatedBinary = ValidFiles();
        truncatedBinary[1].bytes.pop_back();
        RequireRejected(manager, std::move(truncatedBinary),
            "truncated hotkey data was accepted");
        auto mismatchedText = ValidFiles();
        mismatchedText[2].bytes[0] = 2;
        RequireRejected(manager, std::move(mismatchedText),
            "mismatched hotkey text count was accepted");
        auto unterminatedText = ValidFiles();
        std::fill(unterminatedText[2].bytes.begin() + 8,
                  unterminatedText[2].bytes.begin() + 24, static_cast<U8>('X'));
        RequireRejected(manager, std::move(unterminatedText),
            "unterminated hotkey text was accepted");
        auto overflowingCount = ValidFiles();
        overflowingCount[1].bytes[0] = 0xff;
        overflowingCount[1].bytes[1] = 0xff;
        overflowingCount[1].bytes[2] = 0xff;
        overflowingCount[1].bytes[3] = 0xff;
        RequireRejected(manager, std::move(overflowingCount),
            "overflowing hotkey count was accepted");

        if (argc == 3)
        {
            ValidateRealArchive(manager, argv[1]);
            ValidateRealArchive(manager, argv[2]);
        }

        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "hotkey factory shutdown failed");
        Require(HeapBlocks() == heapBaseline, "hotkey probe leaked shared-heap blocks");
        std::cout << "production hotkey event owner passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "hotkey event probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
