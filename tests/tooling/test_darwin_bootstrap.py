from __future__ import annotations

import struct

import pytest

from scripts.objc3c_toolchain import darwin_bootstrap


@pytest.mark.parametrize(('system', 'machine'), [('Linux', 'aarch64'), ('Darwin', 'x86_64')])
def test_bootstrap_rejects_non_native_hosts_before_discovery(tmp_path, monkeypatch, system, machine):
    monkeypatch.setattr(darwin_bootstrap.platform, 'system', lambda: system)
    monkeypatch.setattr(darwin_bootstrap.platform, 'machine', lambda: machine)
    output = tmp_path / 'evidence'
    with pytest.raises(ValueError, match='Apple Silicon Mac running native arm64 Python'):
        darwin_bootstrap.probe(tmp_path / 'missing-toolchain', output)
    assert not output.exists()


def test_macho_artifact_header_checks_architecture_and_kind(tmp_path):
    artifact = tmp_path / 'fixture.o'

    def write(magic=0xfeedfacf, cpu=0x0100000c, kind=1):
        artifact.write_bytes(struct.pack('<8I', magic, cpu, 0, kind, 1, 8, 0, 0) + bytes(8))

    write()
    assert darwin_bootstrap.verify_macho(artifact, 'object')['format'] == 'Mach-O arm64'
    for identity in ({'cpu': 0x01000007}, {'kind': 6}, {'magic': 0x464c457f}):
        write(**identity)
        with pytest.raises(ValueError, match='expected an arm64 Mach-O object'):
            darwin_bootstrap.verify_macho(artifact, 'object')
    artifact.write_bytes(artifact.read_bytes()[:16])
    with pytest.raises(ValueError, match='missing Mach-O header'):
        darwin_bootstrap.verify_macho(artifact, 'object')
