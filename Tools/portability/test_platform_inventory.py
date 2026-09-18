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

    def test_native_user_settings_owner_is_connected_to_headless_startup(self):
        root = Path(__file__).resolve().parents[2]
        owner = strip_comments(
            (root / 'Code/Libs/Src/Platform/NativeUserSettingsOwner.cpp').read_text(
                encoding='utf-8', errors='replace'))
        headless = strip_comments(
            (root / 'Code/Launcher/Headless.cpp').read_text(
                encoding='utf-8', errors='replace'))

        self.assertIn('LoadNativeUserSettingsAtPrefix', owner)
        self.assertIn('store.RemovePrefix(prefix)', owner)
        self.assertIn('StoreNativeUserSettingsAtPrefix', owner)
        self.assertIn('NativeUserSettingsOwner userSettings', headless)
        self.assertIn('NativeUserSettingsSourceName(userSettings.Source())', headless)

    def test_production_event_system_has_a_portable_owner_path(self):
        root = Path(__file__).resolve().parents[2]
        source = strip_comments(
            (root / 'Code/Libs/Src/DAHOTKEY/EventSys.cpp').read_text(
                encoding='utf-8', errors='replace'))
        heap = strip_comments(
            (root / 'Code/Libs/Include/da_heap_utility.h').read_text(
                encoding='utf-8', errors='replace'))

        self.assertNotIn('#include <windows.h>', source)
        self.assertIn('#include "SYSTEM.H"', source)
        self.assertIn('pFreeList(0)', source)
        self.assertEqual(source.count('DA_HEAP_FREE(HEAP,'), 3)
        self.assertIn('#define DA_HEAP_FREE( heap, ptr )', heap)
        self.assertIn('calloc( 1, (size) )', heap)

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

        self.assertIn('DynamicLibrary(const DynamicLibrary &) = delete', interface)
        self.assertIn('DynamicLibrary(DynamicLibrary && other) noexcept', interface)
        self.assertIn('RTLD_NOW | RTLD_LOCAL', implementation)
        self.assertIn('std::exchange(other.handle_, nullptr)', implementation)
        self.assertIn('dlerror()', implementation)
        self.assertIn('FrontierXRegisterComponents', module_contract)
        self.assertIn('FrontierXCanUnloadComponents', module_contract)
        self.assertIn('RemoveRegistrations(position->id)', manager)
        self.assertIn('position->canUnload() == FALSE', manager)
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

        self.assertIn('NativeComponentManager manager', shell)
        self.assertIn('RegisterTheEventSystem(&manager)', shell)
        self.assertIn('SdlWindow window', shell)
        self.assertIn('SdlEventPump pump(window, *eventSystem.Get())', shell)
        self.assertIn('SleepForMilliseconds(1)', shell)
        self.assertIn('LoadConfiguredDataRoot', shell)
        self.assertIn('ResolveStartupDataRoot', shell)
        self.assertIn('LoadConfiguredDataRoot', headless)
        self.assertIn('ResolveStartupDataRoot', headless)
        self.assertIn('ProfileDocument config', startup)
        self.assertIn('Platform::InspectDataRoot(result.path)', startup)
        self.assertIn('add_executable(frontierx_native_shell', cmake)


if __name__ == '__main__':
    unittest.main()
