from __future__ import annotations

import json
from pathlib import Path
from types import SimpleNamespace
import sys

import pytest

from scripts.objc3c_toolchain_runtime import build_linux


@pytest.mark.skipif(sys.platform != 'linux', reason='native Linux runtime producer')
def test_cold_checkout_noise_cannot_contaminate_the_structured_report(tmp_path, monkeypatch, capsys):
    profile = Path(__file__).resolve().parents[2] / 'scripts/objc3c_toolchain_runtime/runtime-profile.json'
    config = tmp_path / 'scripts/objc3c_toolchain_runtime/runtime-profile.json'
    config.parent.mkdir(parents=True)
    config.write_bytes(profile.read_bytes())
    compiler = tmp_path / 'compiler/bin/clang'
    compiler.parent.mkdir(parents=True)
    compiler.write_bytes(b'compiler identity')

    def cold_checkout(path, *args):
        path.mkdir(parents=True)
        print(f'Cloning into {path}...\nDetached HEAD advice {{not JSON}}', file=sys.stderr)

    def execute(command, **kwargs):
        if command[:2] == ['cmake', '--install']:
            stage = Path(command[command.index('--prefix') + 1])
            (stage / 'lib').mkdir(parents=True)
            (stage / 'lib/libobjc.so').write_bytes(b'runtime')
        return SimpleNamespace(returncode=0)

    monkeypatch.setattr(build_linux, 'checkout', cold_checkout)
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
