from __future__ import annotations

import json
from pathlib import Path
import struct
from types import SimpleNamespace
import sys

import pytest

from scripts.objc3c_toolchain_runtime import build_linux
from scripts.objc3c_toolchain.linux_host import host_identity, native_linux_host


@pytest.mark.skipif(sys.platform != 'linux', reason='native Linux runtime producer')
def test_cold_checkout_noise_cannot_contaminate_the_structured_report(tmp_path, monkeypatch, capsys):
    profile = Path(__file__).resolve().parents[2] / 'scripts/objc3c_toolchain_runtime/runtime-profile.json'
    config = tmp_path / 'scripts/objc3c_toolchain_runtime/runtime-profile.json'
    config.parent.mkdir(parents=True)
    config.write_bytes(profile.read_bytes())
    for row in json.loads(profile.read_text())['profiles'][native_linux_host()]['source_patches']:
        patch = tmp_path / row['file']
        patch.parent.mkdir(parents=True, exist_ok=True)
        patch.write_bytes((profile.parents[2] / row['file']).read_bytes())
    compiler = tmp_path / 'compiler/bin/clang'
    compiler.parent.mkdir(parents=True)
    elf = bytearray(64);elf[:6] = b'\x7fELF\x02\x01'
    elf[18:20] = host_identity()['elf_machine'].to_bytes(2, 'little')
    struct.pack_into('<Q', elf, 32, 64)
    struct.pack_into('<HH', elf, 54, 56, 1)
    note = struct.pack('<III', 4, 20, 3) + b'GNU\0' + b'\x01' * 20
    elf.extend(struct.pack('<IIQQQQQQ', 4, 0, 120, 0, 0, len(note), len(note), 4))
    elf.extend(note)
    for tool in ('clang', 'clang++', 'ld.lld'):
        (compiler.parent / tool).write_bytes(elf)

    def cold_checkout(path, *args, **kwargs):
        if not path.exists():
            path.mkdir(parents=True)
            print(f'Cloning into {path}...\nDetached HEAD advice {{not JSON}}', file=sys.stderr)

    commands = []

    def execute(command, **kwargs):
        commands.append(command)
        if command[:2] == ['cmake', '--install']:
            stage = Path(command[command.index('--prefix') + 1])
            (stage / 'lib').mkdir(parents=True)
            (stage / 'lib/libobjc.so').write_bytes(elf)
        return SimpleNamespace(returncode=0)

    monkeypatch.setattr(build_linux, 'checkout', cold_checkout)
    monkeypatch.setattr(build_linux, 'prepare_source', lambda *args: None)
    monkeypatch.setattr(build_linux, 'verify_source', lambda *args: True)
    monkeypatch.setattr(build_linux.subprocess, 'run', execute)
    report = tmp_path / 'logs/runtime-report.json'
    assert build_linux.main(['--repo', str(tmp_path), '--compiler-root', str(compiler.parents[1]),
                             '--fetch', '--report-file', str(report)]) == 0
    streams = capsys.readouterr()
    assert 'Detached HEAD advice' in streams.err
    data = json.loads(report.read_text())
    assert data == json.loads(streams.out)
    assert data['status'] == 'passed'
    assert Path(data['stage']).joinpath('lib/libobjc.so').is_file()
    assert data['runtime_build_id'] == '01' * 20
    configure = next(command for command in commands if command[:2] == ['cmake', '-S'])
    shared_flags = next(argument for argument in configure
                        if argument.startswith('-DCMAKE_SHARED_LINKER_FLAGS='))
    assert '-Wl,--disable-new-dtags' in shared_flags.split()
    assert '-Wl,--build-id=sha1' in shared_flags.split()
