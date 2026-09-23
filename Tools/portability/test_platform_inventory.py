"""Synthetic checks for the lexical platform-touchpoint inventory."""
import tempfile
from pathlib import Path
import re
import unittest

from platform_inventory import COMPILED_CATEGORIES, inventory, owner_for, strip_comments


class PlatformInventoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for relative in ('Code/App/Src', 'Code/Libs/Src', 'Code/Libs/Include'):
            (self.root / relative).mkdir(parents=True, exist_ok=True)

    def write(self, relative, text):
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text)
        return path

    def test_categories_owners_comments_and_determinism(self):
        self.write('Code/App/Src/main.cpp', '''
            #include <windows.h>
            // LoadLibrary("comment-only"); D3DXMatrixIdentity(x)
            void f(HWND window) { LoadLibraryA("x"); RegCloseKey(HKEY_CURRENT_USER); }
        ''')
        self.write('Code/Libs/Src/Render/pipe.cpp', '''
            IDirect3DDevice9 *device;
            D3DXMATRIX matrix;
            /* __asm { nop } */
            void f(HANDLE handle) {
                int pointer_target;
                U32 value = (U32)&pointer_target;
                DWORD index = (DWORD)handle;
            }
        ''')
        self.write('Code/App/Src/Scripts/script.cpp', 'long value; va_list args;')
        self.write('Code/Libs/Src/Tools/ignored.cpp', 'D3DXMATRIX ignored; HWND ignored_window;')

        result = inventory(self.root)
        self.assertEqual(result, inventory(self.root))
        self.assertEqual(result['source_files_scanned'], 3)
        self.assertEqual(result['categories']['win32_headers']['occurrences'], 1)
        self.assertEqual(result['categories']['dynamic_loading']['occurrences'], 1)
        self.assertEqual(result['categories']['d3dx9']['occurrences'], 1)
        self.assertEqual(result['categories']['pointer_integer_casts']['occurrences'], 1)
        self.assertEqual(result['categories']['encoded_handle_casts']['occurrences'], 1)
        self.assertEqual(result['categories']['native_long']['owners'][0]['owner'], 'App/Scripts')
        self.assertEqual(result['categories']['d3d9']['owners'][0]['owner'], 'Libs/Render')

    def test_missing_root_fails(self):
        (self.root / 'Code/Libs/Include').rmdir()
        with self.assertRaisesRegex(ValueError, 'Missing scan root'):
            inventory(self.root)

    def test_comment_scanner_preserves_strings_and_lines(self):
        text = '"//not-comment"; // comment\nvalue /* block\ncomment */ end\n'
        stripped = strip_comments(text)
        self.assertIn('"//not-comment"', stripped)
        self.assertNotIn(' comment', stripped)
        self.assertEqual(stripped.count('\n'), text.count('\n'))

    def test_owner_mapping(self):
        self.assertEqual(owner_for(Path('Code/App/Src/a.cpp')), 'App')
        self.assertEqual(owner_for(Path('Code/App/Src/Scripts/a.cpp')), 'App/Scripts')
        self.assertEqual(owner_for(Path('Code/Libs/Src/DACOM/a.cpp')), 'Libs/DACOM')
        self.assertEqual(owner_for(Path('Code/Libs/Include/a.h')), 'Libs/Include')

    def test_production_dosfile_has_no_legacy_width_handle_casts(self):
        root = Path(__file__).resolve().parents[2]
        for name in ('IFF.cpp', 'UTF.cpp', 'UTFShare.cpp'):
            with self.subTest(source=name):
                source = root / 'Code/Libs/Src/DOSFile' / name
                text = strip_comments(source.read_text(encoding='utf-8'))
                matches = list(COMPILED_CATEGORIES['encoded_handle_casts'].finditer(text))
                self.assertEqual(matches, [])

    def test_production_dosfile_has_no_adjacent_parameter_context_decode(self):
        root = Path(__file__).resolve().parents[2]
        pattern = re.compile(r'\(\([^)]*\*\)\s*lpContext\)\s*\[[1-9]')
        for source in sorted((root / 'Code/Libs/Src/DOSFile').glob('*.cpp')):
            with self.subTest(source=source.name):
                text = strip_comments(source.read_text(encoding='utf-8', errors='replace'))
                self.assertIsNone(pattern.search(text))

    def test_production_dacom_runtime_has_no_pointer_truncation_casts(self):
        root = Path(__file__).resolve().parents[2]
        sources = (
            root / 'Code/Libs/Include/TComponent.h',
            root / 'Code/Libs/Include/TConnContainer.h',
            root / 'Code/Libs/Include/TConnPoint.h',
            root / 'Code/Libs/Include/tdispatch.h',
            root / 'Code/App/Src/TCPoint.h',
            root / 'Code/App/Src/TResource.h',
        )
        for source in sources:
            with self.subTest(source=source.name):
                text = strip_comments(source.read_text(encoding='utf-8', errors='replace'))
                matches = list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(text))
                self.assertEqual(matches, [])

    def test_connection_handles_are_tokens_not_node_addresses(self):
        root = Path(__file__).resolve().parents[2]
        for relative in ('Code/Libs/Include/TConnPoint.h', 'Code/App/Src/TCPoint.h'):
            with self.subTest(source=relative):
                text = strip_comments(
                    (root / relative).read_text(encoding='utf-8', errors='replace'))
                self.assertIn('*handle = connectionToken;', text)
                self.assertIn('pList->connectionToken == handle', text)
                self.assertNotRegex(text, r'\*handle\s*=\s*\(U32\)\s*pPrev')
                self.assertNotRegex(text, r'\([^)]*CONNECTION_NODE[^)]*\*\)\s*handle')

    def test_stack_local_guards_use_pointer_width_helper(self):
        root = Path(__file__).resolve().parents[2]
        sources = (
            root / 'Code/App/Src/Include/MGroupRef.h',
            root / 'Code/App/Src/MPart.h',
            root / 'Code/App/Src/Objwatch.h',
        )
        for source in sources:
            with self.subTest(source=source.name):
                text = strip_comments(source.read_text(encoding='utf-8', errors='replace'))
                self.assertIn('FrontierX::IsAddressNearCurrentStack', text)
                self.assertNotRegex(text, r'\b(?:__asm|_asm)\b[^\n]*\besp\b')
                matches = list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(text))
                self.assertEqual(matches, [])

    def test_particle_callback_context_is_pointer_width(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/Libs/Include/IParticleManager.h').read_text(
                encoding='utf-8', errors='replace'))
        implementation = strip_comments(
            (root / 'Code/Libs/Src/ParticleEffect/Particles.cpp').read_text(
                encoding='utf-8', errors='replace'))
        consumer = strip_comments(
            (root / 'Code/App/Src/EffectPlayer.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('FrontierX::ParticleContext context', interface)
        self.assertIn('FrontierX::ParticleContext context', implementation)
        self.assertIn('ParticleContextFromPointer(this)', consumer)
        self.assertIn('PointerFromParticleContext<EffectInstance>(context)', consumer)
        self.assertNotRegex(interface, r'\bDWORD\s+context\b')
        self.assertNotRegex(implementation, r'\bDWORD\s+context\b')
        self.assertEqual(
            list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(consumer)), [])

    def test_richedit_stream_cookie_is_pointer_width(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/App/Src/DumpView.cpp').read_text(
                encoding='utf-8', errors='replace'))
        self.assertIn('editStreamCallback (DWORD_PTR cookie', source)
        self.assertIn('reinterpret_cast<DWORD_PTR>(this)', source)
        self.assertIn('editStream.pfnCallback = &DumpView::editStreamCallback;', source)
        self.assertNotIn('DUMP_PROC', source)
        self.assertEqual(
            list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(source)), [])

    def test_directshow_notification_context_is_pointer_width(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/App/Src/DSStream.cpp').read_text(
                encoding='utf-8', errors='replace'))
        self.assertIn('reinterpret_cast<LONG_PTR>(this)', source)
        self.assertEqual(
            list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(source)), [])

    def test_hotkey_message_parameters_are_pointer_width(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/Libs/Include/HKEvent.h').read_text(
                encoding='utf-8', errors='replace'))
        implementation = strip_comments(
            (root / 'Code/Libs/Src/DAHOTKEY/HKEvent.cpp').read_text(
                encoding='utf-8', errors='replace'))
        modal = strip_comments(
            (root / 'Code/App/Src/Modal.cpp').read_text(
                encoding='utf-8', errors='replace'))

        for text in (interface, implementation):
            self.assertIn('FrontierX::LegacyWindowToken', text)
            self.assertIn('FrontierX::LegacyLParam', text)
        self.assertIn('LegacyLParamFromPointer(&joyinfo)', modal)
        self.assertIn('PointerFromLegacyLParam<JOYINFOEX>(lParam)', implementation)
        self.assertEqual(
            list(COMPILED_CATEGORIES['pointer_integer_casts'].finditer(modal)), [])

    def test_dacom_public_headers_have_a_non_windows_interface_path(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/Libs/Include/DACOM.H').read_text(
                encoding='utf-8', errors='replace'))
        connection = strip_comments(
            (root / 'Code/Libs/Include/IConnection.h').read_text(
                encoding='utf-8', errors='replace'))
        profile_interface = strip_comments(
            (root / 'Code/Libs/Include/IProfileParser.h').read_text(
                encoding='utf-8', errors='replace'))
        component = strip_comments(
            (root / 'Code/Libs/Include/TComponent.h').read_text(
                encoding='utf-8', errors='replace'))

        self.assertNotIn('#error Must define WIN32 or _WIN32', interface)
        self.assertIn('#include "TYPEDEFS.H"', interface)
        self.assertIn('#include "Results.h"', interface)
        self.assertIn('#include "DACOM.H"', connection)
        self.assertRegex(
            connection, r'BOOL32\s+\(COMAPI \* CONNECTION_ENUM_PROC\)')
        self.assertIn('#include "DACOM.H"', profile_interface)
        self.assertIn('(COMAPI * ENUM_PROC)', profile_interface)
        self.assertIn('BOOL32 COMAPI ProfileCallback', profile_interface)
        self.assertIn('#include "DACOM.H"', component)
        self.assertIn('Base::_GetEntriesIn()', component)
        self.assertIn('this->className', component)
        self.assertIn('DAComponentInner<DAComponentAggregate<Base> > innerComponent;', component)
        self.assertNotIn('DAComponentInner<Base> innerComponent;', component)

    def test_window_manager_public_contract_has_a_pointer_width_native_path(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/Libs/Include/WindowManager.h').read_text(
                encoding='utf-8', errors='replace'))
        native_types = strip_comments(
            (root / 'Code/Libs/Include/FrontierX/LegacyWindowTypes.h').read_text(
                encoding='utf-8', errors='replace'))

        self.assertNotIn('#error Windows.h required for this to compile!', interface)
        self.assertIn('#include "DACOM.H"', interface)
        self.assertIn('#include "FrontierX/LegacyWindowTypes.h"', interface)
        self.assertIn('#include "FrontierX/LegacyMessage.h"', native_types)
        self.assertIn('using HWND = FrontierX::LegacyWindowToken;', native_types)
        self.assertIn('using WPARAM = FrontierX::LegacyWParam;', native_types)
        self.assertIn('using LPARAM = FrontierX::LegacyLParam;', native_types)
        self.assertIn('using LONG = std::int32_t;', native_types)
        self.assertIn('static_assert(sizeof(RECT) == 16U', native_types)
        self.assertIn('typedef void (DACOM_CDECL * WMEXITCB)', interface)
        self.assertIn('typedef LRESULT (COMAPI * WNDPROC)', interface)

    def test_profile_parser_uses_pointer_width_handles_and_shared_callbacks(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/Libs/Src/DACOM/ProfileParser.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/profile_parser_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertNotRegex(source, r'\(U32\)\s*hSection')
        self.assertIn('FrontierX::DecodeEncodedHandle(hSection)', source)
        self.assertIn('FrontierX::EncodeHandleValue(', source)
        self.assertIn('ProfileBuffer::EnumerateSections', source)
        self.assertIn('ProfileBuffer::EnumerateKeys', source)
        self.assertIn('#ifdef _WIN32', source)
        self.assertIn('RegisterProfileParsers', source)
        self.assertIn('InitializeBuffer(buffer, bufferSize)', source)
        self.assertNotIn('ACCESS(bufferSize | 0x80000000)', source)
        self.assertIn('NativeComponentManager manager', probe)
        self.assertIn('RegisterProfileParsers(&manager)', probe)
        self.assertIn('Initialize2(nullptr, 1)', probe)

    def test_user_defaults_boundary_and_legacy_value_adapter_contract(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/App/Src/UserDefaults.h').read_text(
                encoding='utf-8', errors='replace'))
        owner = strip_comments(
            (root / 'Code/App/Src/UserDefaults.cpp').read_text(
                encoding='utf-8', errors='replace'))
        adapter = strip_comments(
            (root / 'Code/Libs/Src/Platform/LegacyPreferenceValues.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('#include <DACOM.H>', interface)
        self.assertNotIn('virtual BOOL32 __stdcall', interface)
        self.assertIn('virtual BOOL32 COMAPI LoadDefaults', interface)
        self.assertIn('#define MAX_MRU\t4', owner)
        self.assertRegex(owner, r'case\s+ERROR_MORE_DATA:\s+cbData\s*=\s*0xFFFFFFFF')
        self.assertIn('RegDeleteValue(hkey, szKey)', owner)
        self.assertIn('BufferTooSmall', adapter)
        self.assertIn('return false;', adapter)

    def test_native_user_settings_owner_is_shared_by_startup_processes(self):
        root = Path(__file__).resolve().parents[2]
        owner = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeUserSettingsOwner.cpp').read_text(
                encoding='utf-8', errors='replace'))
        headless = strip_comments(
            (root / 'Code/Launcher/Headless.cpp').read_text(
                encoding='utf-8', errors='replace'))
        shell = strip_comments(
            (root / 'Code/Launcher/NativeShell.cpp').read_text(
                encoding='utf-8', errors='replace'))
        startup = strip_comments(
            (root / 'Code/Libs/Src/Platform/StartupPreferences.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('LoadNativeUserSettingsAtPrefix', owner)
        self.assertIn('store.RemovePrefix(prefix)', owner)
        self.assertIn('StoreNativeUserSettingsAtPrefix', owner)
        self.assertIn('loaded.userSettings.Load(loaded.store', startup)
        self.assertIn('result = std::move(loaded)', startup)
        self.assertIn('LoadStartupPreferences(', headless)
        self.assertIn('LoadStartupPreferences(', shell)
        self.assertIn('preferences.userSettings.Source()', headless)
        self.assertIn('preferences.userSettings.Source()', shell)

    def test_native_user_defaults_implements_the_real_interface_without_raw_struct_storage(self):
        root = Path(__file__).resolve().parents[2]
        owner = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeUserDefaults.cpp').read_text(
                encoding='utf-8', errors='replace'))
        shell = strip_comments(
            (root / 'Code/Launcher/NativeShell.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('class NativeUserDefaults : public IUserDefaults', owner)
        self.assertIn('owner.Store(store', owner)
        self.assertIn('LegacyPreferenceValues values', owner)
        self.assertIn('CopyFromLegacy(user_defaults_, settings)', owner)
        self.assertIn('BOOL32 UbiEula() override { return FALSE; }', owner)
        self.assertNotIn('SetBinary("User Defaults", &user_defaults_', owner)
        self.assertIn('RegisterNativeUserDefaults(', shell)
        self.assertIn('DACOMDESC defaultsDescriptor("IUserDefaults")', shell)
        self.assertIn('userDefaults.Get()->StoreDefaults()', shell)

    def test_production_event_system_has_a_portable_owner_path(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/Libs/Src/DAHOTKEY/EventSys.cpp').read_text(
                encoding='utf-8', errors='replace'))
        heap = strip_comments(
            (root / 'Code/Libs/Include/da_heap_utility.h').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/event_system_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertNotIn('#include <windows.h>', source)
        self.assertIn('#include "SYSTEM.H"', source)
        self.assertIn('pFreeList(0)', source)
        self.assertEqual(source.count('DA_HEAP_FREE(HEAP,'), 3)
        self.assertIn('#define DA_HEAP_FREE( heap, ptr )', heap)
        self.assertIn('calloc( 1, (size) )', heap)
        self.assertIn('target_compile_definitions(frontierx_event_system PRIVATE '
                      'DA_HEAP_ENABLED=1)', cmake)
        self.assertIn('target_link_libraries(frontierx_event_system PUBLIC '
                      'frontierx_native_heap)', cmake)
        self.assertIn('HEAP_Acquire()', probe)
        self.assertIn('static production event owner did not return shared-heap allocations',
                      probe)

    def test_native_component_manager_hosts_the_production_event_owner(self):
        root = Path(__file__).resolve().parents[2]
        manager = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeComponentManager.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/event_system_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('priority >= registration.priority', manager)
        self.assertIn('registration.component->CreateInstance', manager)
        self.assertIn('GR_NOT_IMPLEMENTED', manager)
        self.assertIn('NativeComponentManager manager', probe)
        self.assertIn('RegisterTheEventSystem(&manager)', probe)
        self.assertNotIn('ProbeManager', probe)

    def test_production_hotkey_event_uses_portable_file_input_and_shared_heap(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/Libs/Src/DAHOTKEY/HKEvent.cpp').read_text(
                encoding='utf-8', errors='replace'))
        file_types = strip_comments(
            (root / 'Code/Libs/Include/FrontierX/LegacyFileTypes.h').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/hotkey_event_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertIn('#include "FrontierX/LegacyInput.h"', source)
        self.assertIn('#include "FrontierX/LegacyJoystick.h"', source)
        self.assertIn('dwSizeOfKey != sizeof(DAHOTKEY_STRUCT)', source)
        self.assertIn('std::numeric_limits<DWORD>::max()', source)
        self.assertIn('std::memchr(text', source)
        self.assertEqual(source.count('std::memmove(dst, src,'), 2)
        self.assertIn('DA_HEAP_FREE(HEAP, pKeyData)', source)
        self.assertNotIn('free(pKeyData)', source)
        self.assertIn('using HANDLE = void *', file_types)
        self.assertIn('using LONG = std::int32_t', file_types)
        self.assertIn('RegisterHotkeyEventServer(&manager)', probe)
        self.assertIn('invalid hotkey record stride was accepted', probe)
        self.assertIn('production hotkey aggregate did not return shared-heap allocations', probe)
        self.assertIn('target_compile_definitions(frontierx_hotkey_event PRIVATE '
                      'DA_HEAP_ENABLED=1)', cmake)
        self.assertIn('add_test(NAME portability.hotkey_event', cmake)

    def test_native_utf_file_system_hosts_checked_in_hotkey_tables(self):
        root = Path(__file__).resolve().parents[2]
        adapter = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeUtfFileSystem.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/hotkey_event_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))
        shell = strip_comments(
            (root / 'Code/Launcher/NativeShell.cpp').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertIn('class NativeUtfFileSystem final : public IFileSystem', adapter)
        self.assertIn('ReadUtfEntry(path_, descriptor->lpFileName', adapter)
        self.assertIn('TryDecodeOffsetHandleIndex(handle, FirstChildHandle', adapter)
        self.assertIn('descriptor->dwDesiredAccess != GENERIC_READ', adapter)
        self.assertIn('WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED)', adapter)
        self.assertIn('ValidateRealArchive(manager, argv[1])', probe)
        self.assertIn('ValidateRealArchive(manager, argv[2])', probe)
        self.assertIn('eventSystem->Send(keyMessage.message, &keyMessage)', probe)
        self.assertIn('eventOwner->Update()', probe)
        self.assertIn('RegisterHotkeyEventServer(manager)', shell)
        self.assertIn('OpenNativeUtfFileSystem(', shell)
        self.assertIn('rawEventConnection.Connect(rawEventPoint.Get(), hotkeyInput.Get())', shell)
        self.assertIn('hotkeyEventConnection.Connect(hotkeyEventPoint.Get()', shell)
        self.assertIn('~ConnectionOwner() { Disconnect(); }', shell)
        self.assertIn('eventSystemOwner.Get()->Update()', shell)
        self.assertIn('Code/App/Src/Hotkeys.utf', cmake)

    def test_native_dynamic_library_boundary_is_local_and_move_only(self):
        root = Path(__file__).resolve().parents[2]
        interface = strip_comments(
            (root / 'Code/Libs/Include/FrontierX/DynamicLibrary.h').read_text(
                encoding='utf-8', errors='replace'))
        implementation = strip_comments(
            (root / 'Code/Libs/Src/Platform/DynamicLibrary.cpp').read_text(
                encoding='utf-8', errors='replace'))
        manager = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeComponentManager.cpp').read_text(
                encoding='utf-8', errors='replace'))
        module_contract = strip_comments(
            (root / 'Code/Libs/Include/FrontierX/NativeComponentModule.h').read_text(
                encoding='utf-8', errors='replace'))
        event_owner = strip_comments(
            (root / 'Code/Libs/Src/DAHOTKEY/EventSys.cpp').read_text(
                encoding='utf-8', errors='replace'))
        event_module = strip_comments(
            (root / 'Code/Libs/Src/Platform/EventSystemModule.cpp').read_text(
                encoding='utf-8', errors='replace'))
        event_exports = (root / 'Code/Libs/Src/Platform/EventSystemModule.map').read_text(
            encoding='utf-8', errors='replace')
        probe = strip_comments(
            (root / 'Tools/portability/dynamic_library_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))
        heap_utility = strip_comments(
            (root / 'Code/Libs/Include/da_heap_utility.h').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertIn('DynamicLibrary(const DynamicLibrary &) = delete', interface)
        self.assertIn('DynamicLibrary(DynamicLibrary && other) noexcept', interface)
        self.assertIn('RTLD_NOW | RTLD_LOCAL', implementation)
        self.assertIn('std::exchange(other.handle_, nullptr)', implementation)
        self.assertIn('dlerror()', implementation)
        self.assertIn('FrontierXRegisterComponents', module_contract)
        self.assertIn('FrontierXCanUnloadComponents', module_contract)
        self.assertIn('RemoveRegistrations(position->id)', manager)
        self.assertIn('return loaded.canUnload() != FALSE', manager)
        self.assertIn('libraries_.back().library.Abandon()', manager)
        self.assertIn('document.Lines("Libraries")', manager)
        self.assertIn('baseDirectory / libraryPath', manager)
        self.assertIn('rollbackSucceeded', manager)
        self.assertIn('module_instance_count++', event_owner)
        self.assertIn('module_instance_count--', event_owner)
        self.assertIn('FrontierXRegisterComponents', event_module)
        self.assertIn('FrontierXEventSystemModuleCanUnload', event_module)
        self.assertIn('FrontierXRegisterComponents;', event_exports)
        self.assertIn('FrontierXCanUnloadComponents;', event_exports)
        self.assertIn('local:', event_exports)
        self.assertIn('--version-script=${frontierx_event_system_module_exports}', cmake)
        self.assertIn('#include "HeapObj.h"', heap_utility)
        self.assertIn('FRONTIERX_EVENT_SYSTEM_MODULE=1 DA_HEAP_ENABLED=1', cmake)
        self.assertIn('frontierx_dynamic_library_probe PROPERTY ENABLE_EXPORTS ON', cmake)
        self.assertIn('frontierx_native_shell PROPERTY ENABLE_EXPORTS ON', cmake)
        self.assertIn('InitializeDAHeap(1024 * 1024', probe)
        self.assertIn('availableBeforeEvent', probe)
        self.assertIn('FrontierXEventSystemModuleCanUnload', probe)
        self.assertIn('exposed an internal implementation symbol', probe)
        self.assertIn('did not return all shared-heap allocations', probe)
        self.assertLess(manager.index('RemoveRegistrations(position->id)'),
                        manager.index('position->library.Close(error)'))

    def test_sdl_compatibility_dispatch_reaches_the_production_event_owner(self):
        root = Path(__file__).resolve().parents[2]
        bridge = strip_comments(
            (root / 'Code/Libs/Include/FrontierX/LegacyEventDispatch.h').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/legacy_input_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('static_cast<IEventSystem *>(context)->Send', bridge)
        self.assertIn('NativeComponentManager manager', probe)
        self.assertIn('DispatchLegacyInputToEventSystem', probe)
        self.assertIn('eventPoint->Advise', probe)

    def test_sdl_event_pump_owns_batch_dispatch_to_the_event_system(self):
        root = Path(__file__).resolve().parents[2]
        implementation = strip_comments(
            (root / 'Code/Libs/Src/Platform/SdlEventPump.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/legacy_input_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertIn('while (window_.PollEvent(event))', implementation)
        self.assertIn('adapter_.Dispatch(event, DispatchLegacyInputToEventSystem', implementation)
        self.assertNotIn('event_system_.Post', implementation)
        self.assertIn('SdlEventPump pump(window, *eventSystem)', probe)
        self.assertIn('SDL_PushEvent', probe)
        self.assertIn('add_library(frontierx_sdl3_event_pump STATIC', cmake)

    def test_native_shell_composes_data_window_and_event_owners(self):
        root = Path(__file__).resolve().parents[2]
        shell = strip_comments(
            (root / 'Code/Launcher/NativeShell.cpp').read_text(
                encoding='utf-8', errors='replace'))
        headless = strip_comments(
            (root / 'Code/Launcher/Headless.cpp').read_text(
                encoding='utf-8', errors='replace'))
        startup = strip_comments(
            (root / 'Code/Libs/Src/Platform/StartupDataRoot.cpp').read_text(
                encoding='utf-8', errors='replace'))
        cmake = (root / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')

        self.assertIn('ICOManager * manager = DACOM_Acquire()', shell)
        self.assertIn('InitializeDAHeap(0x10000, 0x4000', shell)
        self.assertIn('manager->SetINIConfig(configName.c_str())', shell)
        self.assertIn('RegisterTheEventSystem(manager)', shell)
        self.assertIn('RegisterHotkeyEventServer(manager)', shell)
        self.assertLess(shell.index('InitializeDAHeap(0x10000, 0x4000'),
                        shell.index('RegisterTheEventSystem(manager)'))
        self.assertLess(shell.index('RegisterTheEventSystem(manager)'),
                        shell.index('manager->SetINIConfig(configName.c_str())'))
        self.assertIn('RegisterNativeWindowManager(manager)', shell)
        self.assertLess(shell.index('RegisterNativeWindowManager(manager)'),
                        shell.index('manager->SetINIConfig(configName.c_str())'))
        self.assertIn('AGGDESC windowDescriptor("IWindowManager", "WindowManager")', shell)
        self.assertIn('windowManager.Get()->SetCallback(ShellWindowCallback)', shell)
        self.assertIn('windowManager.Get()->ServeMessageQueue(WMF_BACKGROUND)', shell)
        self.assertIn('eventSystemOwner.Get()->Update()', shell)
        self.assertNotIn('SdlWindow window', shell)
        self.assertNotIn('SdlEventPump pump', shell)
        self.assertIn('SleepForMilliseconds(1)', shell)
        self.assertIn('LoadConfiguredDataRoot', shell)
        self.assertIn('ResolveStartupDataRoot', shell)
        self.assertIn('LoadConfiguredDataRoot', headless)
        self.assertIn('ResolveStartupDataRoot', headless)
        self.assertIn('ProfileDocument config', startup)
        self.assertIn('Platform::InspectDataRoot(result.path)', startup)
        self.assertIn('add_executable(frontierx_native_shell', cmake)

    def test_native_dacom_acquire_owns_one_manager_and_production_profiles(self):
        root = Path(__file__).resolve().parents[2]
        bootstrap = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeDacomBootstrap.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/native_dacom_bootstrap_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('extern "C" ICOManager * DACOM_CDECL DACOM_Acquire()', bootstrap)
        self.assertIn('static NativeDacomOwner owner', bootstrap)
        self.assertIn('RegisterProfileParsers(&manager)', bootstrap)
        self.assertIn('first == second', probe)
        self.assertIn('enumeration.profile == 1 && enumeration.profile2 == 1', probe)
        self.assertIn('first->CreateInstance(&descriptor', probe)

    def test_native_heap_owns_the_process_local_iheap_contract(self):
        root = Path(__file__).resolve().parents[2]
        header = (root / 'Code/Libs/Include/HeapObj.h').read_text(
            encoding='utf-8', errors='replace')
        implementation = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeHeap.cpp').read_text(
                encoding='utf-8', errors='replace'))
        bootstrap = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeDacomBootstrap.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/native_heap_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('#include "DACOM.H"', header)
        self.assertNotIn('__stdcall', header)
        self.assertNotIn('__cdecl', header)
        self.assertIn('class NativeHeap final : public IHeap', implementation)
        self.assertIn('std::map<void *, Block', implementation)
        self.assertIn('if (position == blocks_.end()) return FALSE', implementation)
        self.assertIn('RegisterNativeHeap', bootstrap)
        self.assertLess(bootstrap.index('RegisterNativeHeap(&manager)'),
                        bootstrap.index('RegisterProfileParsers(&manager)'))
        self.assertIn('InitializeDAHeap(long heap_size', bootstrap)
        self.assertIn('InitializeDAHeap(4096, 1024', probe)
        self.assertIn('native heap accepted a foreign pointer', probe)

    def test_native_window_manager_owns_the_sdl_window_contract(self):
        root = Path(__file__).resolve().parents[2]
        owner = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeWindowManager.cpp').read_text(
                encoding='utf-8', errors='replace'))
        probe = strip_comments(
            (root / 'Tools/portability/native_window_manager_probe.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('public IWindowManager', owner)
        self.assertIn('SdlWindow window_', owner)
        self.assertIn('void ServeMessageQueue(U32 flags) override', owner)
        self.assertIn('window_callback_(', owner)
        self.assertIn('CloseVetoed(close_callbacks_.pClientList)', owner)
        self.assertIn('RegisterNativeWindowManager', owner)
        self.assertIn('WMF_WAIT | WMF_BACKGROUND', probe)
        self.assertIn('prevent-multiple startup contract changed', probe)
        self.assertIn('close veto did not suppress', probe)


if __name__ == '__main__':
    unittest.main()
