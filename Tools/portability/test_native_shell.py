"""Process tests for the native SDL shell using synthetic empty data directories."""
import os
import re
import ctypes
import ctypes.util
from pathlib import Path
import subprocess
import struct
import shutil
import sys
import tempfile
import unittest
import wave

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())
EVENT_MODULE = str(Path(sys.argv.pop(1)).resolve()) if len(sys.argv) > 1 else ''
HOTKEY_TABLE = str(Path(sys.argv.pop(1)).resolve()) if len(sys.argv) > 1 else ''
GPU_ENABLED = sys.argv.pop(1) == '1' if len(sys.argv) > 1 else False
FONT_FILE = sys.argv.pop(1) if len(sys.argv) > 1 else ''
VIDEO_ENABLED = sys.argv.pop(1) == '1' if len(sys.argv) > 1 else False


def describe_external_face(filename):
    # FreeType is already required by the optional target. Read its documented
    # face prefix; no font copying, substitute face or additional Python package.
    class FacePrefix(ctypes.Structure):
        _fields_ = [(name, ctypes.c_long) for name in
                    ('num_faces', 'face_index', 'face_flags', 'style_flags', 'num_glyphs')] + [
                    ('family_name', ctypes.c_char_p), ('style_name', ctypes.c_char_p)]
    Face = ctypes.POINTER(FacePrefix)
    library = ctypes.CDLL(ctypes.util.find_library('freetype'))
    library.FT_Init_FreeType.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    library.FT_New_Face.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_long,
                                  ctypes.POINTER(Face)]
    library.FT_Done_Face.argtypes = [Face]
    library.FT_Done_FreeType.argtypes = [ctypes.c_void_p]
    context, face = ctypes.c_void_p(), Face()
    if library.FT_Init_FreeType(ctypes.byref(context)):
        raise RuntimeError('external fixture FreeType initialization failed')
    try:
        if library.FT_New_Face(context, os.fsencode(filename), 0, ctypes.byref(face)):
            raise RuntimeError('external fixture face cannot be read')
        try:
            return face.contents.family_name.decode('utf-8'), bool(face.contents.style_flags & 2)
        finally:
            library.FT_Done_Face(face)
    finally:
        library.FT_Done_FreeType(context)


class NativeShellTests(unittest.TestCase):
    def test_jointed_turret_model_frame_source_contract(self):
        # TM5 Battleship barrels pointed down world Z. The numerical material
        # check covers the joint/muzzle basis; pin its three real consumers.
        source = Path(__file__).resolve().parents[2] / 'Code/App/Src/Turret.cpp'
        text = re.sub(r'//[^\n]*|/\*.*?\*/', '', source.read_text(), flags=re.DOTALL)
        node = text.split('TRANSFORM Turret::getModelNodeTransform(', 1)[1].split(
            'void Turret::getSighting', 1)[0]
        self.assertIn('NativeShipHardpointTransform', node)
        self.assertIn('IsNativePersistentGunboat(owner.Ptr())', node)
        self.assertNotIn('turretIndex == -1', node)
        sight = text.split('void Turret::getSighting', 1)[1].split('void Turret::swivelToTarget', 1)[0]
        swivel = text.split('void Turret::swivelToTarget (', 1)[1].split('void Turret::swivelToTargetRT', 1)[0]
        leading = text.split('void Turret::recalculateOffsetVectorForDistance', 1)[1].split(
            'void Turret::recalculateOffsetVectorForAngle', 1)[0]
        self.assertIn('getModelNodeTransform(barrelIndex)', sight)
        self.assertIn('getModelNodeTransform(turretIndex)', swivel)
        self.assertIn('getModelNodeTransform(barrelIndex)', leading)

    def test_blackhole_required_resources_fail_closed_source_contract(self):
        # Regression: TM4's real ring import returned INVALID_ARCHETYPE_INDEX,
        # yet the factory published a BlackHole and instance init asserted later.
        # This source check pins the production factory's boundary/rollback;
        # actual retail resource failure and playback need an external run.
        source = Path(__file__).resolve().parents[2] / 'Code/App/Src/blackhole.cpp'
        text = re.sub(r'//[^\n]*|/\*.*?\*/', '', source.read_text(), flags=re.DOTALL)
        create = text.split('HANDLE BlackHoleManager::CreateArchetype(', 1)[1].split(
            'BOOL32 BlackHoleManager::DestroyArchetype', 1)[0]
        self.assertIn('std::unique_ptr<BLACKHOLE_INIT> pending(new BLACKHOLE_INIT)', create)
        self.assertIn('BLACKHOLE_INIT *newguy = pending.get()', create)
        self.assertRegex(create, r'if\s*\(!valid\)\s*throw std::runtime_error')
        for contract in (
            'bool(billboard.mesh_name[0]) == bool(billboard.tex_name[0])',
            'newguy->bb_mesh_archID[i] != INVALID_ARCHETYPE_INDEX || newguy->mm_arch[i]',
            'animation && animation->frames && animation->frame_cnt',
            'animation->frames[frame].texture != 0',
            'newguy->archIndex != INVALID_ARCHETYPE_INDEX',
            'newguy->sysMapIconID != U32(-1)',
        ):
            self.assertIn(contract, create)
        self.assertGreater(create.index('return pending.release()'), create.rindex('requireResource('))
        cleanup = text.split('~BLACKHOLE_INIT (void)', 1)[1].split('struct SortVertex', 1)[0]
        for owned in ('if (archIndex != INVALID_ARCHETYPE_INDEX)',
                      'if (bb_mesh_archID[i] != INVALID_ARCHETYPE_INDEX)',
                      'TMANAGER->ReleaseTextureRef(bb_txm[i])',
                      'delete bb_anim_arch[i]', 'DestroyMorphMeshArchetype(mm_arch[i])'):
            self.assertIn(owned, cleanup)

    def test_hawkes_rescue_movie_source_binding(self):
        # Protect the missing original caller. This source check does not run a
        # victory, manufacture progress, or stand in for MovieScreen playback.
        source = Path(__file__).resolve().parents[2] / 'Code/App/Src'

        def uncomment(filename):
            return re.sub(r'//[^\n]*|/\*.*?\*/', '',
                          (source / filename).read_text(), flags=re.DOTALL)

        outcome = uncomment('Menu_EndGame.cpp').split(
            'void MenuEndGame::checkForMovie (void)', 1)[1].split(
            'void MenuEndGame::setStateInfo (void)', 1)[0]
        self.assertRegex(outcome, r'if\s*\(bWon\)')
        rescue = outcome.split('case 2:', 1)[1].split('case 8:', 1)[0]
        native = rescue.split('#elif defined(FRONTIERX_HAVE_VIDEO_DECODER)', 1)[1].split('#endif', 1)[0]
        self.assertRegex(native, r'if\s*\(!CQFLAGS\.bSkipMovies\)\s*\{')
        self.assertRegex(native, r'MovieScreen\(this, "cq_1\.mpg"\);')
        self.assertLess(native.index('MovieScreen('), native.index('SetMovieSeen(1)'))
        self.assertNotIn('GetMoviesSeen', native)

        replay = uncomment('Menu_mission.cpp').split('case MOVIE_BUTTON_1:', 1)[1].split(
            'case MOVIE_BUTTON_2:', 1)[0]
        self.assertIn('#if !defined(_WIN32) && defined(FRONTIERX_HAVE_VIDEO_DECODER)', replay)
        self.assertRegex(replay, r'if\s*\(!CQFLAGS\.bSkipMovies\)\s*MovieScreen\(this, "cq_1\.mpg"\);')
        self.assertNotIn('SetMissionCompleted', replay)
        self.assertNotIn('SetMovieSeen', replay)

    def test_solarian_effects_reach_live_mesh_and_original_build_owners(self):
        # Runtime evidence is separate: these guard the original caller wires
        # behind the real MassDisruptor core and missing construction beams.
        source = Path(__file__).resolve().parents[2] / 'Code/App/Src'
        mass = (source / 'MassDisruptor.cpp').read_text().split(
            'void MassDisruptor::Render (void)', 1)[1].split('void MassDisruptor::InitWeapon', 1)[0]
        self.assertIn('VOLPTR(IExtent) extent = target.Ptr()', mass)
        self.assertIn('MeshChain &chain = extent->GetMeshChain()', mass)
        self.assertIn('TreeRenderNativePuffy(chain.mi, chain.numChildren, scale, 0.4,', mass)
        build = (source / 'TObjBuild.h').read_text()
        setup = build.split('void setupNativeConstructionBuild', 1)[1].split('SINGLE emitterPause', 1)[0]
        self.assertIn('this->race != M_TERRAN && this->race != M_SOLARIAN', setup)
        self.assertIn('buildEffectObj->SetupMesh(', setup)
        solar = (source / 'SolarianBuild.cpp').read_text()
        self.assertIn('ResetNativeLegacyBuildCorners(fgr, fg_dork->nativeSource)', solar)
        self.assertIn('UpdateNativeLegacyBuildFace(fgr, fg_dork->nativeSource, face, f)', solar)
        probe = (source / 'DumbReconProbe.cpp').read_text().split('void DumbReconProbe::Render', 1)[1].split('void DumbReconProbe::InitRecon', 1)[0]
        self.assertIn('NativeProbeVisualTransform(transform, targetPos)', probe)
        self.assertIn('TreeRender(mc.mi, mc.numChildren, transform.get_inverse() * visual)', probe)

    def test_later_campaign_movies_use_original_result_and_replay_owners(self):
        # Missing original TM9/TM11 movie calls silently marked unseen media as
        # seen. Protect the caller; actual decoder/EOF is checked separately.
        source = Path(__file__).resolve().parents[2] / 'Code/App/Src'
        def uncomment(filename):
            return re.sub(r'//[^\n]*|/\*.*?\*/', '',
                          (source / filename).read_text(), flags=re.DOTALL)
        outcome = uncomment('Menu_EndGame.cpp').split(
            'void MenuEndGame::checkForMovie (void)', 1)[1].split(
            'void MenuEndGame::setStateInfo (void)', 1)[0]
        for mission_id, movie, next_case in ((8, 2, 10), (10, 3, 15)):
            branch = outcome.split(f'case {mission_id}:', 1)[1].split(f'case {next_case}:', 1)[0]
            native = branch.split('#elif defined(FRONTIERX_HAVE_VIDEO_DECODER)', 1)[1].split('#endif', 1)[0]
            self.assertRegex(native, r'if\s*\(!CQFLAGS\.bSkipMovies\)\s*\{')
            self.assertIn(f'MovieScreen(this, "cq_{movie}.mpg");', native)
            self.assertLess(native.index('MovieScreen('), native.index(f'SetMovieSeen({movie})'))
            replay = uncomment('Menu_mission.cpp').split(f'case MOVIE_BUTTON_{movie}:', 1)[1].split('break;', 1)[0]
            self.assertIn('#if !defined(_WIN32) && defined(FRONTIERX_HAVE_VIDEO_DECODER)', replay)
            self.assertIn(f'MovieScreen(this, "cq_{movie}.mpg");', replay)
            self.assertNotIn('SetMovieSeen', replay)
            self.assertNotIn('SetMissionCompleted', replay)

    def test_tm4_script_startup_briefing_resources_and_map_source_binding(self):
        # Source regression: TM4's compiled owner initially passed schema tests
        # but its original briefing was rejected by a separate manual allowlist.
        # TM5 is included only to preserve the TM4 Continue-to-next-briefing contract.
        # This verifies binding coverage, not a mission entry or save/load run.
        source = Path(__file__).resolve().parents[2]
        def read(name):
            return re.sub(r'//[^\n]*|/\*.*?\*/', '',
                          (source / name).read_text(), flags=re.DOTALL)
        script = read('Code/App/Src/MScript.cpp')
        load = script.split('void MScript::Load (IFileSystem * inFile)', 1)[1].split(
            'void MScript::LoadScript (void)', 1)[0]
        briefing = script.split('void MScript::LoadScript (void)', 1)[1].split('#else', 1)[0]
        for number in (3, 4, 5, 6, 7, 8, 9, 10, 11, 12):
            token = f'{number:02}'
            self.assertIn(f'head = &nativeTerran{token}Programs', script)
            self.assertIn(f'FrontierXCheckScript{token}Reset()', script)
            self.assertIn(f'GetNativeScript{token}Schema(', script)
            for alias in (f'Script{token}T.dll', f'script{token}.dll', f'Script{token}.dll'):
                self.assertIn(f'"{alias}"', load)
            self.assertIn(f'FrontierXInitializeScript{token}()', load)
            self.assertIn(f'!findScript("TM{number}_Briefing")', briefing)
            self.assertRegex(script, rf'GetNativeScript{token}ResourceHandle\(\)\)\s*'
                             rf'counted = FrontierX::GetNativeScript{token}CountedString\(stringID\)')
        # The actual TM5 retail header uses this uppercase module spelling.
        self.assertIn('strcmp(scriptName, "SCRIPT05.dll") == 0', load)
        self.assertIn('terran04 ? nativeTerran04Programs : terran05 ? nativeTerran05Programs : terran07 ? nativeTerran07Programs : terran08 ? nativeTerran08Programs : terran09 ? nativeTerran09Programs : terran10 ? nativeTerran10Programs : terran11 ? nativeTerran11Programs : terran12 ? nativeTerran12Programs : terran06 ? nativeTerran06Programs : nullptr', load)
        self.assertIn('"TM4_MissionStart"', load)
        self.assertIn('runStartProgram(true)', briefing)
        shell = read('Code/Launcher/NativeShell.cpp')
        for ordinal, number in (('fourth', 4), ('fifth', 5), ('sixth', 6), ('seventh', 7), ('eighth', 8), ('ninth', 9)):
            selected = shell.split(f'const bool {ordinal}TerranMission', 1)[1].split(
                'Native App selected source script:', 1)[0]
            for alias in (f'Script{number:02}T.dll', f'script{number:02}.dll', f'Script{number:02}.dll'):
                self.assertIn(f'"{alias}"', selected)
            self.assertIn(f'!{ordinal}TerranMission', selected)
            if number == 5:
                self.assertIn('std::strcmp(selectedScript, "SCRIPT05.dll") == 0', selected)
        mission = read('Code/App/Src/Mission.cpp').split(
            'static bool NativeLoadMissionSystemMap', 1)[1].split('#endif', 1)[0]
        for alias in ('script03t.dll', 'script03.dll', 'script04t.dll', 'script04.dll', 'script05t.dll', 'script05.dll', 'script06t.dll', 'script06.dll', 'script07t.dll', 'script07.dll', 'script08t.dll', 'script08.dll', 'script09t.dll', 'script09.dll', 'script10t.dll', 'script10.dll', 'script11t.dll', 'script11.dll', 'script12t.dll', 'script12.dll'):
            self.assertIn(f'foldedScript == "{alias}"', mission)
        self.assertIn('regularTerran && foldedExtension == ".mission"', mission)

    def test_tm4_direct_source_retains_full_terran_effects_and_materials(self):
        # The original selector starts from TM1; a direct TM4 diagnostic must
        # still use the same full presentation owners, without reduced flags.
        # TM5 is prepared here solely as the required next-entry contract.
        source = (Path(__file__).resolve().parents[2] / 'Code/Launcher/NativeShell.cpp').read_text()
        effects = source.split('if (options.missionStart)', 1)[1].split(
            'InitializeNativeEffectsOptions();', 1)[0]
        materials = source.split('NATIVE_USE_ORIGINAL_LEGACY_MATERIALS =', 1)[1].split(';', 1)[0]
        for number in range(1, 13):
            name = f'"T_Mission{number}.dmission"'
            self.assertIn(name, effects)
            self.assertIn(name, materials)
        self.assertIn('!options.skipLegacyLighting', materials)

    def test_briefing_start_rejects_failed_native_mission_reload_source_contract(self):
        # Concrete source regression: a failed terrain/object load closes the
        # partial mission. IDS_START previously ignored Reload's false result
        # and reported success, allowing scripts to run against missing actors.
        # This checks the caller's control flow, not a real mission load failure.
        path = Path(__file__).resolve().parents[2] / 'Code/App/Src/Menu_Briefing.cpp'
        source = re.sub(r'//[^\n]*|/\*.*?\*/', '', path.read_text(), flags=re.DOTALL)
        start = source.split('case IDS_START:', 1)[1].split('case IDS_REPLAY:', 1)[0]
        native = start.split('#else', 1)[1].split('#endif', 1)[0]
        self.assertRegex(native, r'if\s*\(!MISSION->Reload\(\)\)\s*\{'
                         r'[^{}]*endDialog\(0\);\s*break;\s*\}')
        self.assertNotIn('endDialog(1)', native)
        self.assertNotIn('endDialog(1)', start.split('#else', 1)[0])
        self.assertIn('endDialog(1)', start.split('#endif', 1)[1])
        windows = start.split('#if defined(_WIN32)', 1)[1].split('#else', 1)[0]
        self.assertRegex(windows, r'MISSION->Reload\(\);')

    def test_unit_catalog_requires_exclusive_diagnostic(self):
        result = self.run_shell('--unit-catalog-check')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('requires exclusive --mission-start', result.stderr)
        result = self.run_shell('--unit-catalog-check', '--unit-catalog-check')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('duplicate option: --unit-catalog-check', result.stderr)
        for mode in ('--mission-campaign', '--mission-briefing'):
            result = self.run_shell('--unit-catalog-check', '--mission-start', mode)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('requires exclusive --mission-start', result.stderr)

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.data = self.root / 'Data'
        for directory in [
                'Objects', 'Interface', 'Textures', 'Mat', 'Profiles',
                'Maps/Campaign', 'Maps/Multiplayer', 'Music', 'SFX']:
            (self.data / directory).mkdir(parents=True)
        with wave.open(str(self.data / 'SFX/interface1.wav'), 'wb') as sound:
            sound.setnchannels(1)
            sound.setsampwidth(2)
            sound.setframerate(22050)
            sound.writeframes(b'\0\0' * 220)
        shutil.copyfile(self.data / 'SFX/interface1.wav', self.data / 'SFX/specialdenied.wav')
        # Original LFParser framing: header, numeric value, previous-frame marker.
        (self.data / 'SFX/interface1.txt').write_bytes(
            b'Frame, phoneme\r\n0, 3\r\n1, <\r\n2, 12\r\n')
        (self.data / 'Profiles/native-smoke.ini').write_text(
            '[NativeSmoke]\nValue = ready\n')

    def run_shell(self, *arguments, data_root='', video_driver='dummy'):
        environment = os.environ.copy()
        environment['SDL_VIDEODRIVER'] = video_driver
        environment['SDL_AUDIODRIVER'] = 'dummy'
        environment['FRONTIERX_DATA_ROOT'] = data_root
        environment['XDG_CONFIG_HOME'] = str(self.root / 'xdg')
        environment['HOME'] = str(self.root / 'home')
        return subprocess.run(
            [EXECUTABLE, *map(str, arguments)], cwd=self.root, env=environment,
            text=True, capture_output=True, timeout=10)

    def test_packed_campaign_mounts_and_required_materials(self):
        shutil.rmtree(self.data / 'Maps')
        names = b'/\0Campaign\0Multiplayer\0'
        name_offset = 56 + 3 * 44
        header = struct.pack('<14I', 0x20465455, 0x101, 56, 3 * 44, 0, 44,
                             name_offset, len(names), len(names),
                             name_offset + len(names), 0, 0, 0, 0)
        def entry(next_entry, name, child):
            return struct.pack('<11I', next_entry, name, 0x10, 0, child, 0, 0, 0, 0, 0, 0)
        (self.data / 'maps').write_bytes(
            header + entry(0, 0, 44) + entry(88, 2, 0) + entry(0, 11, 0) + names)
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native data archives mounted: 2', result.stdout)
        (self.data / 'Mat').rmdir()
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Mat: missing', result.stderr)
        self.assertNotIn('Maps/Campaign:', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)

        material_root = self.root / 'material-source'
        material_root.mkdir()
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Native material source lacks directory: shaders', result.stderr)
        for directory in ('shaders', 'materials', 'textures'):
            (material_root / directory).mkdir()
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native material source: explicit directory', result.stdout)
        self.assertIn('Native data archives mounted: 2', result.stdout)
        self.assertNotIn(str(material_root), result.stdout + result.stderr)
        (material_root / 'textures').rmdir()
        (material_root / 'textures').write_text('not a directory')
        result = self.run_shell('--data-root', self.data, '--material-root', material_root,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Native material source lacks directory: textures', result.stderr)

    def test_hidden_smoke_lifecycle(self):
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '5', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line; required data sources passed', result.stdout)
        self.assertIn('Preferences source: xdg-config-home; status: missing; entries: 0; '
                      'user-defaults: absent; settings-source: defaults', result.stdout)
        defaults_ready = result.stdout.find(
            'Native user defaults initialized before GameSystem startup')
        self.assertGreaterEqual(defaults_ready, 0)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertIn('retail game startup and rendering were not attempted', result.stdout)
        if GPU_ENABLED:
            game_ready = result.stdout.find(
                'Native GameSystem startup completed with synthetic profile')
            self.assertGreater(game_ready, defaults_ready)
        mission = self.data / 'Maps/Campaign/Sol_Train.dmission'
        (mission / 'ParseData').mkdir(parents=True)
        (mission / 'ParseData/Data.h').write_text('struct MT_GlobalData { int id; };')
        result = self.run_shell('--data-root', self.data,
                                '--mission-source', 'Sol_Train.dmission',
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native App parser schema initialization failed', result.stderr)
            profile = self.root / 'mission.ini'
            profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--mission-engine', '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission Engine components or IRenderer unavailable',
                          result.stderr)
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission global data unavailable', result.stderr)
            (mission / 'ParseData/Data.h').write_text('static_assert(1);')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App mission ParseData invalid', result.stderr)
            (mission / 'ParseData/Data.h').write_text(
                'struct MT_GlobalData { int id; };')
            (mission / 'MT_GlobalData').mkdir()
            (mission / 'MT_GlobalData/Globals').write_bytes(b'\x01\x00\x00\x00')
            save_root = self.root / 'SavedGame'
            save_root.mkdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', save_root,
                                    '--skip-notification-cue',
                                    '--sfx-smoke',
                                    *(['--font-smoke', FONT_FILE] if FONT_FILE else []),
                                    '--profile-smoke', 'native-smoke.ini',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            if FONT_FILE:
                family, bold = describe_external_face(FONT_FILE)
                weight = 700 if bold else 400
                good = f'{family},{weight}={FONT_FILE}\n'
                # A face can expose Arial as typographic family and Arial Narrow
                # as its legitimate legacy SFNT family. Neither is a negative
                # fixture for the other; use an unrelated synthetic family.
                wrong_family = 'FrontierX Unrelated Synthetic Family'
                mapping = self.root / 'font-map.txt'
                invalid_mappings = [
                    ('Arial,400 font.ttf\n', 'Expected font mapping Family,weight=filename'),
                    ('Arial,0=font.ttf\n', 'Invalid font mapping family or weight'),
                    (good + good, 'Duplicate font family/weight mapping'),
                    (f'{wrong_family},{weight}={FONT_FILE}\n',
                     'Mapped face does not match the original font family'),
                ]
                if ''.join(family.split()).casefold() != 'bauhausmditctt':
                    wrong_weight = 400 if bold else 700
                    invalid_mappings.append((f'{family},{wrong_weight}={FONT_FILE}\n',
                        'Mapped face does not match the requested font weight'))
                for contents, error in invalid_mappings:
                    mapping.write_text(contents)
                    rejected = self.run_shell('--data-root', self.data,
                        '--system-profile', profile, '--mission-source', 'Sol_Train.dmission',
                        '--save-root', save_root, '--skip-notification-cue', '--sfx-smoke',
                        '--font-smoke', FONT_FILE, '--ui-font-map', mapping,
                        '--smoke-ms', '1', '--hidden')
                    self.assertEqual(rejected.returncode, 6, rejected.stdout + rejected.stderr)
                    self.assertIn('Native external font mapping failed: ' + error, rejected.stderr)
                self.assertIn('Native source teletype counted string, pause, expiry and flush checked', result.stdout)
                self.assertIn('subtitle wrapping and release checked', result.stdout)
                self.assertIn('status/hint/scrolling lifecycle checked', result.stdout)
                self.assertIn('Native source status Rally Point CR/LF metrics and draw geometry checked', result.stdout)
                self.assertIn('hot-rectangle input/post/lifecycle checked', result.stdout)
                self.assertIn('original VFX image reader checked', result.stdout)
                self.assertIn('unit alert lifecycle checked', result.stdout)
            if GPU_ENABLED:
                self.assertIn('Native effect-animation randomization and RNG consumption checked', result.stdout)
            self.assertIn('Native App parser schema loaded from embedded data.i',
                          result.stdout)
            self.assertIn('Native App mission source opened through SPMAPDIR', result.stdout)
            if GPU_ENABLED:
                self.assertIn('Native production player AI save lifecycle checked', result.stdout)
                self.assertIn('Native source stateless Ping restart validated against real launcher owner',
                              result.stdout)
                self.assertIn('Native source Gunboat failed launcher load propagated', result.stdout)
                self.assertIn('Native source system-map mission animations saved and restored',
                              result.stdout)
                self.assertIn('Native source mission records correlated and invalid records rejected',
                              result.stdout)
                self.assertIn('Native source mission ability records saved and invalid records rejected',
                              result.stdout)
            self.assertIn('Native App mission IStreamer initialized with Music and SFX sources',
                          result.stdout)
            self.assertIn('Native App source string resources initialized', result.stdout)
            self.assertIn('Native App writable save directory initialized outside retail data',
                          result.stdout)
            self.assertIn('Native App text resources initialized', result.stdout)
            self.assertIn('Native App status: Disabled', result.stderr)
            self.assertIn('Native App hint: Disabled', result.stderr)
            self.assertIn('Native App message: Disabled', result.stderr)
            self.assertIn('Native App message: Native message', result.stderr)
            self.assertIn('scrolling text initialized; notification cue explicitly disabled',
                          result.stdout)
            self.assertIn('Native App mission progress owner initialized', result.stdout)
            self.assertIn('Native mission loading: 5%', result.stderr)
            self.assertIn('Native App MusicManager initialized', result.stdout)
            self.assertIn('Native ISFX BUTTONPRESS1 preload/open/play/stop checked', result.stdout)
            self.assertIn('Native source SFX pool/cache ownership and RNG consumption checked',
                          result.stdout)
            self.assertIn('Native source builder-drone arrays released by their allocating owners',
                          result.stdout)
            self.assertIn('Native mission sound retains EOF until actual completion and owner cleanup checked',
                          result.stdout)
            self.assertIn('Native unit communication unavailable: frontierx-missing-communication.wav; failed opens 1; playback ID 0 (failure)',
                          result.stderr)
            self.assertIn('Native mission audio unavailable: frontierx-missing-mission-audio.wav; failed opens 1; playback ID 0 (failure)',
                          result.stderr)
            self.assertIn('Native mission beacon resolved from SFX: specialDenied.wav', result.stderr)
            if GPU_ENABLED:
                self.assertIn('Native operation storage alignment/copy/load checked', result.stdout)
                self.assertIn('Native objective storage compaction/state checked', result.stdout)
            self.assertIn('Native App PROFILEDIR CreateProfileParser checked', result.stdout)
            self.assertIn('mission not started', result.stdout)
            (self.data / 'SFX/interface1.wav').unlink()
            (self.data / 'SFX/interface1.txt').unlink()
            (self.data / 'SFX/specialdenied.wav').unlink()
            (self.data / 'SFX').rmdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', save_root,
                                    '--skip-notification-cue',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('mission audio source or IStreamer unavailable', result.stderr)
            (self.data / 'SFX').mkdir()
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--save-root', self.data,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App save-directory binding failed', result.stderr)
            profile.write_text('[System]\nIEventSystem\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('mission audio source or IStreamer unavailable', result.stderr)
            profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                               'IWindowManager = WindowManager\n'
                               'IViewConstructor\nIParticleManager\n[Engine]\n')
            invalid_gen_data = self.root / 'invalid-gendata.db'
            invalid_gen_data.write_bytes(b'not a UTF archive')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--gen-data', invalid_gen_data,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App general data archive invalid', result.stderr)
            invalid_game_types = self.root / 'invalid-gametypes.db'
            invalid_game_types.write_bytes(b'not a UTF archive')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--mission-source', 'Sol_Train.dmission',
                                    '--game-types', invalid_game_types,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
            self.assertIn('Native App game-types archive invalid', result.stderr)
        self.assertNotIn(str(self.root), result.stdout)
        stored = self.root / 'xdg' / 'frontierx' / 'preferences.dat'
        self.assertTrue(stored.is_file())
        self.assertTrue(stored.read_bytes().startswith(b'FXSET001'))

    def test_environment_root_and_startup_failures(self):
        result = self.run_shell('--smoke-ms', '1', '--hidden', data_root=str(self.data))
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: environment', result.stdout)
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden',
                                video_driver='frontierx_missing_driver')
        self.assertEqual(result.returncode, 3 if GPU_ENABLED else 4)
        self.assertIn('Native game-system startup failed' if GPU_ENABLED else
                      'Native window-manager startup failed', result.stderr)
        self.assertEqual(
            self.run_shell('--data-root', 'missing', '--smoke-ms', '1', '--hidden').returncode,
            2)
        (self.data / 'Objects').rmdir()
        result = self.run_shell('--data-root', self.data, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 2)
        self.assertIn('Objects: missing', result.stderr)

    def test_configuration_root_and_precedence(self):
        config = self.root / 'settings' / 'native.ini'
        config.parent.mkdir()
        config.write_text(
            '[FrontierX]\nDataRoot=../Data\n[Libraries]\n; no dynamic modules\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: configuration', result.stdout)
        self.assertIn('Native component library configuration applied', result.stdout)

        result = self.run_shell('--config', config, '--data-root', self.data,
                                '--smoke-ms', '1', '--hidden', data_root='missing')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Data root source: command-line', result.stdout)

        config.write_text('[FrontierX]\n')
        self.assertEqual(
            self.run_shell('--config', config, '--smoke-ms', '1', '--hidden').returncode,
            2)

        config.write_text(
            '[FrontierX]\nDataRoot=../Data\n[Libraries]\nmissing-module.so\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native component configuration failed', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

    def test_usage_contract(self):
        result = self.run_shell('--help')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('render smoke draws a repository fixture', result.stdout)
        self.assertIn('game-scene renders one supplied UTF mesh', result.stdout)
        for arguments in [
                ('--hidden',), ('--smoke-ms', '0'), ('--smoke-ms', '60001'),
                ('--smoke-ms', 'bad'), ('--data-root',), ('--preferences',),
                ('--hotkeys',), ('--system-profile',), ('--render-smoke',),
                ('--game-types',), ('--game-types', 'types.db'),
                ('--animation-object',), ('--animation-object', 'Forger'),
                ('--mesh-object',), ('--mesh-object', '../Forger'),
                ('--movie-source',), ('--movie-source', 'movie.mpg'),
                ('--movie-source', 'movie.mpg', '--skip-movies'),
                ('--movie-source', 'movie.mpg', '--mission-start', '--mission-campaign'),
                ('--movie-source', 'movie.mpg', '--mission-start', '--mission-briefing'),
                ('--movie-source', 'movie.mpg', '--mission-start', '--smoke-ms', '1'),
                ('--mission-source',), ('--mission-source', '../Sol_Train.dmission'),
                ('--mission-source', 'Sol_Train.mission'),
                ('--mission-start',), ('--no-frame-limit',),
                ('--no-frame-limit', '--no-frame-limit'), ('--frame-stats',),
                ('--frame-stats', '--frame-stats'),
                ('--mission-briefing',),
                ('--mission-briefing', '--mission-briefing'),
                ('--mission-briefing', '--mission-start', '--hidden'),
                ('--skip-sector-map',),
                ('--skip-space-background',),
                ('--skip-unit-labels',),
                ('--mission-alert-smoke',),
                ('--mission-hint-check',),
                ('--base-textures-only',),
                ('--skip-legacy-lighting',),
                ('--skip-training-speech',),
                ('--skip-mission-music',),
                ('--mission-start', '--mission-source', 'Sol_Train.dmission'),
                ('--render-smoke', 'vertex.spv'),
                ('--render-smoke', 'vertex.spv', '--hidden'), ('--unknown',)]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)
        mesh_profile = self.root / 'mesh-path.ini'
        mesh_profile.write_text('[System]\nIEventSystem\nIWindowManager = WindowManager\n'
                                '[Engine]\nIAnimation\n')
        for path in ['/Forger', 'planets/../Forger', 'planets//Forger',
                     'planets/./Forger', 'planets/Forger/', 'C:Forger', r'planets\Forger']:
            result = self.run_shell('--data-root', self.data, '--system-profile', mesh_profile,
                                    '--mesh-object', path, '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 1, (path, result.stdout, result.stderr))
        for arguments in [
                ('--game-scene',),
                ('--game-scene', 'materials.utf', 'mesh.utf', 'v.spv',
                 'f.spv', 't4.spv', '--hidden'),
                ('--render-smoke', 'v.spv', 'f.spv', '--game-scene',
                 'materials.utf', 'mesh.utf', 'v.spv', 'f.spv', 't4.spv', 'mt.spv')]:
            self.assertEqual(self.run_shell(*arguments).returncode, 1, arguments)

    def test_production_animation_channel_from_object_directory(self):
        # The real Anim::LoadChannels caller must create IChannel2 and reject
        # corrupt frame data; a registered factory alone would miss this gate.
        object_dir = self.data / 'Objects' / 'Forger'
        channel_dir = object_dir / 'Animation' / 'Chnl' / 'Track'
        script_dir = object_dir / 'Animation' / 'Script'
        channel_dir.mkdir(parents=True)
        script_dir.mkdir()
        (channel_dir / 'Header').write_bytes(struct.pack('<IfI', 2, 1.0, 1))
        (channel_dir / 'Frames').write_bytes(struct.pack('<ff', 0.0, 10.0))
        (script_dir / 'Move').write_bytes(
            b'\0' * 256 + b'Track\0'.ljust(128, b'\0'))
        profile = self.root / 'animation.ini'
        profile.write_text('[System]\nIEventSystem\nIWindowManager = WindowManager\n'
                           '[Engine]\nIAnimation\n')
        arguments = ('--data-root', self.data, '--system-profile', profile,
                     '--animation-object', 'Forger', 'Move',
                     '--smoke-ms', '1', '--hidden')
        result = self.run_shell(*arguments)
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 6,
                         result.stdout + result.stderr)
        if not GPU_ENABLED:
            return
        self.assertIn('Native animation script loaded; scripts: 1; duration: 1',
                      result.stdout)
        self.assertIn('Native source animation node kinds released with allocated types',
                      result.stdout)
        self.assertIn('Native source animation channel stream types released polymorphically',
                      result.stdout)
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--mesh-object', 'Forger', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native mesh archetype load failed', result.stderr)
        (self.data / 'Objects/planets/Forger').mkdir(parents=True)
        nested = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--mesh-object', 'planets/Forger', '--smoke-ms', '1', '--hidden')
        self.assertEqual(nested.returncode, 6, nested.stdout + nested.stderr)
        self.assertIn('Native mesh archetype load failed', nested.stderr)
        # Accepted relative components still reach the genuine owner and reject
        # this animation-only fixture as a mesh; no model-load success is faked.
        project_root = Path(__file__).resolve().parents[2]
        result = self.run_shell(
            '--data-root', self.data, '--material-root', project_root / 'Data/mat',
            '--system-profile', project_root / 'Code/App/Src/Conquest.ini',
            '--skip-movies', '--mesh-object', 'Forger', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native GameSystem startup completed with supplied profile',
                      result.stdout)
        self.assertIn('IMeshManager returned [OK]', result.stdout + result.stderr)
        self.assertIn('Native mesh archetype load failed', result.stderr)
        (channel_dir / 'Frames').write_bytes(struct.pack('<f', 0.0))
        result = self.run_shell(*arguments)
        self.assertEqual(result.returncode, 6, result.stdout + result.stderr)
        self.assertIn('Native animation script-set load failed', result.stderr)

    def test_render_smoke_rejects_missing_shaders_before_gpu_startup(self):
        result = self.run_shell(
            '--data-root', self.data,
            '--render-smoke', 'missing.vert.spv', 'missing.frag.spv',
            '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 7)
        if GPU_ENABLED:
            self.assertIn('Native render initialization failed: could not open shader',
                          result.stderr)
        else:
            self.assertIn('Native render smoke is unavailable in this build',
                          result.stderr)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

    @unittest.skipUnless(HOTKEY_TABLE, 'production hotkey table path not supplied')
    def test_production_hotkey_pipeline(self):
        result = self.run_shell('--data-root', self.data, '--hotkeys', HOTKEY_TABLE,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native hotkey table loaded and event pipeline connected', result.stdout)
        self.assertNotIn(HOTKEY_TABLE, result.stdout + result.stderr)

        missing = self.root / 'missing-hotkeys.utf'
        result = self.run_shell('--data-root', self.data, '--hotkeys', missing,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 6)
        self.assertIn('Native hotkey table error:', result.stderr)
        self.assertNotIn(str(missing), result.stdout + result.stderr)

    def test_preferences_owner_and_failures(self):
        preferences = self.root / 'preferences.dat'
        preferences.write_bytes(b'FXSET001' + struct.pack('<I', 0))
        result = self.run_shell('--data-root', self.data, '--preferences', preferences,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Preferences source: command-line; status: loaded; entries: 0; '
                      'user-defaults: absent; settings-source: defaults', result.stdout)
        self.assertNotIn(str(self.root), result.stdout)
        self.assertGreater(len(preferences.read_bytes()), 12)

        result = self.run_shell('--data-root', self.data, '--preferences', 'missing.dat',
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 5)
        self.assertIn('Preferences error: explicit preference file is missing', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

        preferences.write_bytes(b'invalid')
        result = self.run_shell('--data-root', self.data, '--preferences', preferences,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 5)
        self.assertIn('Preferences error:', result.stderr)
        self.assertNotIn(str(self.root), result.stderr)

    def test_supplied_system_profile_and_loader_failures(self):
        profile = self.root / 'system.ini'
        profile.write_text(
            '[System]\nIEventSystem\nIWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\nIStringTable\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        else:
            self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

        profile.write_text(
            '[System]\nIEventSystem\nIRenderPrimitive\nIRenderPipeline = RendSft\n'
            'IWindowManager = WindowManager\nIViewConstructor\nTextureLibrary\n'
            'LightManager\nVertexBufferManager\nIStringTable\nIMaterialManager\nITManager\n'
            '[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3, result.stderr)
        if GPU_ENABLED:
            self.assertIn('IMaterialManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('ITManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('Native shell opened; video driver: dummy', result.stdout)

        profile.write_text('[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)

        profile.write_text(f'[System]\n{self.root}/MissingSystemComponent\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native game-system startup failed', result.stderr)
        self.assertNotIn('Native shell opened', result.stdout)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

        profile.write_text('[System]\nIEventSystem\nIStreamer\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('IStreamer returned [OK]', result.stdout + result.stderr)

        for component in ('IVideoSystem',):
            profile.write_text(
                '[System]\nIEventSystem\n' + component + '\nIWindowManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            if VIDEO_ENABLED:
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertIn(component + ' returned [OK]', result.stdout + result.stderr)
                self.assertIn('Native shell opened; video driver: dummy', result.stdout)
                continue
            self.assertEqual(result.returncode, 3)
            self.assertIn(component + ' returned [FAILED]', result.stdout + result.stderr)
            self.assertNotIn('IWindowManager returned [OK]', result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        if GPU_ENABLED:
            profile.write_text('[System]\nIEventSystem\nIParticleManager\n'
                               'IWindowManager = WindowManager\n[Engine]\n')
            result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn('IParticleManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('point-emitter program created', result.stdout)

        # The existing App movie flag excludes only full-screen movie service creation.
        profile.write_text('[System]\nIEventSystem\nIStreamer\nIVideoSystem\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('IVideoSystem [DISABLED by --skip-movies]', result.stdout + result.stderr)
        self.assertIn('IStreamer returned [OK]', result.stdout + result.stderr)
        self.assertNotIn('IVideoSystem returned [OK]', result.stdout + result.stderr)
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 1)
        self.assertIn('duplicate option: --skip-movies', result.stderr)
        profile.write_text('[System]\nIEventSystem\nIVideoSystem\nIParticleManager\nIMeshManager\n'
                           'IWindowManager = WindowManager\n[Engine]\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--skip-movies', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0 if GPU_ENABLED else 3)
        if GPU_ENABLED:
            self.assertIn('IParticleManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('IMeshManager returned [OK]', result.stdout + result.stderr)
            self.assertIn('Native shell opened', result.stdout)
        else:
            self.assertIn('IParticleManager returned [FAILED]', result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        # Engine aggregates from the production animation and hardpoint owners
        # must initialize and tear down through GameSystem, even without a scene.
        profile.write_text('[System]\nIEventSystem\nIWindowManager = WindowManager\n'
                           '[Engine]\nIAnimation\nIHardpoint\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        if GPU_ENABLED:
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)
            self.assertIn('Native shell closed cleanly', result.stdout)

            render_profile = (
                '[System]\nIEventSystem\nIRenderPrimitive\n'
                'IRenderPipeline = RendSft\nTextureLibrary\n'
                'IWindowManager = WindowManager\n[Engine]\nIRenderer\n'
                '[RenderManager]\n')
            profile.write_text(render_profile + 'Optics\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn('Native GameSystem startup completed with supplied profile',
                          result.stdout)

            profile.write_text(render_profile + 'MissingRenderComponent\n')
            result = self.run_shell('--data-root', self.data,
                                    '--system-profile', profile,
                                    '--smoke-ms', '1', '--hidden')
            self.assertEqual(result.returncode, 3, result.stdout + result.stderr)
            self.assertIn('required render component is not available',
                          result.stdout + result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)

        profile.write_text(
            '[System]\nIEventSystem\nIWindowManager = WindowManager\n'
            f'[Engine]\n{self.root}/MissingEngineComponent\n')
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        if GPU_ENABLED:
            self.assertEqual(result.returncode, 3)
            self.assertIn('Native game-system startup failed', result.stderr)
            self.assertNotIn('Native shell opened', result.stdout)
            self.assertNotIn(str(self.root), result.stdout + result.stderr)

        result = self.run_shell('--data-root', self.data, '--system-profile',
                                self.root / 'missing.ini', '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native system profile read failed', result.stderr)
        self.assertNotIn(str(self.root), result.stdout + result.stderr)

        profile.write_bytes(b'x' * (1024 * 1024 + 1))
        result = self.run_shell('--data-root', self.data, '--system-profile', profile,
                                '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 3)
        self.assertIn('Native system profile size is invalid', result.stderr)
        self.assertNotIn(str(profile), result.stdout + result.stderr)

    @unittest.skipUnless(EVENT_MODULE, 'production event module path not supplied')
    def test_configured_production_event_module(self):
        config = self.root / 'module.ini'
        config.write_text(
            f'[FrontierX]\nDataRoot={self.data}\n'
            f'[Libraries]\n"{EVENT_MODULE}"\n')
        result = self.run_shell('--config', config, '--smoke-ms', '1', '--hidden')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('Native component library configuration applied', result.stdout)
        self.assertIn('Native shell opened; video driver: dummy', result.stdout)
        self.assertNotIn(EVENT_MODULE, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
