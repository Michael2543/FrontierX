"""Independent technical aggregate; absent host execution remains explicitly untested."""
import os
import sys


def main(env):
    required = ['CPU_RESULT', 'WINDOWS_RESULT']
    hosts = ['GPU_RESULT', 'MISSION_RESULT', 'MISSION_SANITIZER_RESULT']
    enabled = env.get('HOST_SMOKES') == 'true'
    if env.get('HOST_SMOKES') not in ('true', 'false'):
        print('FAIL: host execution policy missing')
        return 1
    if any(env.get(key) != 'success' for key in required):
        print('FAIL: required Linux build/test or native Windows/ABI job incomplete')
        return 1
    expected = 'success' if enabled else 'skipped'
    if any(env.get(key) != expected for key in hosts):
        print('FAIL: requested host checks failed/incomplete or disabled policy inconsistent')
        return 1
    message = ('Requested GPU and bounded mission checks executed.' if enabled else
               'GPU and bounded mission checks NOT RUN / UNTESTED; user host validation pending.')
    result = ('Technical preparation checks passed. ' + message +
              ' Full user playtest and M3 acceptance remain pending.')
    print(result)
    if env.get('GITHUB_STEP_SUMMARY'):
        with open(env['GITHUB_STEP_SUMMARY'], 'a') as stream:
            stream.write(result + '\n\nPrivate candidate.json and launch command: retained optional '
                         'build workspace on the controlled runner. No binary or runtime upload.\n')
    return 0


if __name__ == '__main__':
    sys.exit(main(os.environ))
