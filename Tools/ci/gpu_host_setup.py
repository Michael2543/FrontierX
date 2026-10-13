"""Explicit Hyprland/X11 preparation of only this CTest process's GPU windows.

No persistent desktop rules, preload library, renderer override or skip. Raw
desktop inventory and command output never leave the private runner log.
"""
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import time


def descendant(pid, ancestor):
    seen = set()
    while pid > 1 and pid not in seen:
        if pid == ancestor:
            return True
        seen.add(pid)
        try:
            # comm can contain spaces or parentheses; fields after its last )
            # start with state and PPID.
            fields = Path(f'/proc/{pid}/stat').read_text().rsplit(')', 1)[1].split()
            pid = int(fields[1])
        except (OSError, ValueError, IndexError):
            return False
    return False


def run(command, root, log, env, private):
    if env.get('FRONTIERX_M3_GPU_SETUP') != 'hyprland-x11':
        raise RuntimeError('explicit supported GPU desktop setup missing')
    directory = private / 'gpu-host-setup'
    directory.mkdir(mode=0o700)
    env = dict(env, SDL_VIDEODRIVER='x11', FRONTIERX_GPU_HOST_SETUP_DIR=str(directory))
    with log.open('ab') as stream:
        subprocess.run(['hyprctl', 'version'], stdout=stream, stderr=subprocess.STDOUT,
                       env=env, check=True, timeout=5)
        process = subprocess.Popen(command, cwd=root, env=env, stdout=stream,
                                   stderr=subprocess.STDOUT, start_new_session=True)
        dispatched = set()
        reported = set()
        try:
            while process.poll() is None:
                requests = [p for p in directory.glob('*.requested')
                            if p.stem.isdecimal() and descendant(int(p.stem), process.pid)]
                if requests:
                    inventory = subprocess.check_output(['hyprctl', 'clients', '-j'],
                                                        env=env, timeout=5)
                    clients = json.loads(inventory)
                    for request in requests:
                        matches = [c for c in clients if c.get('pid') == int(request.stem)]
                        status = (request.stem, tuple((c.get('mapped'), c.get('xwayland'),
                                                      c.get('floating')) for c in matches))
                        if status not in reported:
                            stream.write(('GPU setup owned-window status: ' + repr(status) + '\n').encode())
                            stream.flush()
                            reported.add(status)
                        for client in clients:
                            if (client.get('pid') != int(request.stem) or
                                    client.get('title') not in ('FrontierX SDL3 GPU probe',
                                                                'FrontierX game mesh probe') or
                                    not client.get('mapped') or not client.get('xwayland')):
                                continue
                            address = client.get('address', '')
                            if not re.fullmatch(r'0x[0-9a-fA-F]+', address):
                                raise RuntimeError('invalid test window address')
                            if client.get('floating'):
                                (directory / (request.stem + '.ready')).touch(mode=0o600)
                            elif request.stem not in dispatched:
                                expression = ('hl.dispatch(hl.dsp.window.float({action="toggle",'
                                              'window=hl.get_window("address:' + address + '")}))')
                                subprocess.run(['hyprctl', 'eval', expression], env=env,
                                               stdout=stream, stderr=subprocess.STDOUT,
                                               check=True, timeout=5)
                                dispatched.add(request.stem)
                time.sleep(0.02)
        finally:
            if process.poll() is None:
                os.killpg(process.pid, signal.SIGTERM)
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
        if process.returncode:
            raise RuntimeError('required GPU CTest failed; private diagnostics retained')
