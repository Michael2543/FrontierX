"""Fail mandatory CI scopes on any failure, skip, or absent test execution."""
from pathlib import Path
import sys
import xml.etree.ElementTree as ET


def main(path):
    try:
        root = ET.parse(path).getroot()
        tests = root.findall('.//testcase')
        if not tests or any(t.find('skipped') is not None or
                            t.find('failure') is not None or
                            t.find('error') is not None for t in tests):
            print('FAIL: mandatory CTest scope absent, failed or skipped', file=sys.stderr)
            return 1
    except (OSError, ET.ParseError):
        print('FAIL: CTest execution report unavailable', file=sys.stderr)
        return 1
    print(f'CTest scope: {len(tests)} executed, zero failures/skips')
    return 0


if __name__ == '__main__':
    sys.exit(main(Path(sys.argv[1])))
