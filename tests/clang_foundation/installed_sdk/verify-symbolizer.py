"""Resolve an installed SDK sample's main function to its original source."""

from pathlib import Path
import re
import subprocess
import sys


def verify(prefix: Path, executable: Path) -> None:
    tools = prefix.resolve() / 'bin'
    executable = executable.resolve(strict=True)
    table = subprocess.check_output(
        [tools / 'llvm-readobj', '--symbols', '--elf-output-style=GNU', executable],
        text=True,
    )
    main_addresses = set()
    for line in table.splitlines():
        fields = line.split()
        if (len(fields) == 8 and fields[3] == 'FUNC' and fields[6] != 'UND'
                and fields[7] == 'main'):
            main_addresses.add(int(fields[1], 16))
    if len(main_addresses) != 1:
        raise ValueError('expected one defined main address in the SDK sample')

    address = next(iter(main_addresses))
    transcript = subprocess.check_output(
        [tools / 'llvm-symbolizer', f'--obj={executable}', hex(address)],
        text=True,
    )
    print(transcript, end='')
    lines = transcript.splitlines()
    if (not lines or lines[0] != 'main' or len(lines) < 2
            or not re.search(r'(?:^|/)main\.objc3:[1-9][0-9]*:[0-9]+$', lines[1])):
        raise ValueError('installed llvm-symbolizer did not identify the sample source line')


if __name__ == '__main__':
    verify(Path(sys.argv[1]), Path(sys.argv[2]))
