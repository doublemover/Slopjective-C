"""Native architecture, bootstrap pins and archive admission have one identity."""
import copy
import json
from pathlib import Path
import shutil

import pytest

from scripts.objc3c_toolchain import dependency_cache, install_sdk, linux_host, swift_toolchain

ROOT = Path(__file__).resolve().parents[2]


def architecture(monkeypatch, machine):
    monkeypatch.setattr(linux_host.platform, 'system', lambda: 'Linux')
    monkeypatch.setattr(linux_host.platform, 'machine', lambda: machine)


def elf(machine):
    header = bytearray(64)
    header[:6] = b'\x7fELF\x02\x01'
    header[18:20] = machine.to_bytes(2, 'little')
    return bytes(header)


@pytest.mark.parametrize('machine,host,code', [('x86_64', 'linux-x86_64', 62),
    ('amd64', 'linux-x86_64', 62), ('aarch64', 'linux-aarch64', 183), ('arm64', 'linux-aarch64', 183)])
def test_native_identity_rejects_opposite_architecture_before_execution(tmp_path, monkeypatch, machine, host, code):
    architecture(monkeypatch, machine)
    selected = linux_host.load(ROOT)
    assert selected['host'] == host and selected['elf_machine'] == code
    executable = tmp_path / 'tool';executable.write_bytes(elf(code))
    linux_host.require_elf(executable)
    executable.write_bytes(elf(183 if code == 62 else 62))
    with pytest.raises(ValueError, match='ELF architecture'):
        linux_host.require_elf(executable)
    with pytest.raises(ValueError, match='differs from the native host'):
        linux_host.host_identity('linux-aarch64' if code == 62 else 'linux-x86_64', require_native=True)


@pytest.mark.parametrize('data', [b'', b'#!/bin/sh\nexit 0\n', b'\x7fELF\x01\x01'+bytes(58),
                                  b'\x7fELF\x02\x02'+bytes(58)])
def test_bootstrap_executable_requires_real_64_bit_little_endian_elf(tmp_path, data):
    path = tmp_path / 'tool';path.write_bytes(data)
    with pytest.raises(ValueError, match='ELF architecture'):
        linux_host.require_elf(path)


@pytest.mark.parametrize('mutation', ['version', 'machine', 'inventory', 'hash', 'publisher', 'prefix'])
def test_host_pin_drift_is_rejected(tmp_path, mutation):
    for name in ('toolchains/llvm/manifest.json', 'toolchains/linux-hosts.json'):
        destination = tmp_path / name;destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / name, destination)
    path = tmp_path / 'toolchains/linux-hosts.json';value = json.loads(path.read_text())
    host = value['hosts']['linux-aarch64']
    if mutation == 'version': value['versions']['llvm'] = '0.0.0'
    elif mutation == 'machine': host['elf_machine'] = 62
    elif mutation == 'inventory': del host['assets']['node']
    elif mutation == 'hash': host['assets']['node']['sha256'] = 'unpinned'
    elif mutation == 'publisher': host['assets']['llvm']['url'] = 'https://example.invalid/llvm.tar.xz'
    else: host['prefixes']['llvm'] = '..'
    path.write_text(json.dumps(value))
    with pytest.raises(ValueError): linux_host.load(tmp_path)


def test_binary_caches_are_disjoint_and_pristine_source_objects_are_shared(tmp_path):
    x64 = dependency_cache.layout(ROOT, tmp_path, include_swift=True, host='linux-x86_64')
    arm = dependency_cache.layout(ROOT, tmp_path, include_swift=True, host='linux-aarch64')
    assert x64['sources'] == arm['sources']
    for name, entry in x64['entries'].items():
        assert (entry['key'] == arm['entries'][name]['key']) is (name == 'sources')
    assert x64['entries']['node']['paths'] != arm['entries']['node']['paths']
    assert dependency_cache.quick_layout(ROOT, tmp_path, host='linux-x86_64')['entries']['interpreter']['key'] != (
        dependency_cache.quick_layout(ROOT, tmp_path, host='linux-aarch64')['entries']['interpreter']['key'])


@pytest.mark.parametrize('machine,host', [('x86_64', 'linux-x86_64'), ('aarch64', 'linux-aarch64')])
def test_signed_swift_release_policy_and_host_admission_cover_both_architectures(monkeypatch, machine, host):
    architecture(monkeypatch, machine)
    monkeypatch.setattr(linux_host.platform, 'freedesktop_os_release', lambda: {'ID': 'debian', 'VERSION_ID': '13'})
    swift_toolchain.require_host()
    pin, _ = swift_toolchain.policy(ROOT)
    assert pin['host'] == host and pin['archive_root'] in pin['archive_url']
    other, _ = swift_toolchain.policy(ROOT, host='linux-aarch64' if host == 'linux-x86_64' else 'linux-x86_64')
    assert pin['archive_sha256'] != other['archive_sha256']
    assert pin['selected_manifest_sha256'] != other['selected_manifest_sha256']


@pytest.mark.parametrize('machine,code', [('x86_64', 62), ('aarch64', 183)])
def test_sdk_rejects_mismatched_manifest_and_any_mismatched_elf_member(tmp_path, monkeypatch, machine, code):
    architecture(monkeypatch, machine)
    binary = tmp_path / 'compiler';binary.write_bytes(elf(code))
    readme = tmp_path / 'README';readme.write_text('native SDK')
    data = {'target': linux_host.native_target(), 'files': {'compiler': {'kind': 'file'}, 'README': {'kind': 'file'}}}
    install_sdk.admit_native_sdk(tmp_path, data)
    other = copy.deepcopy(data);other['target'] = 'aarch64-unknown-linux-gnu' if code == 62 else 'x86_64-unknown-linux-gnu'
    with pytest.raises(ValueError, match='differs from the native Linux target'): install_sdk.admit_native_sdk(tmp_path, other)
    binary.write_bytes(elf(183 if code == 62 else 62))
    with pytest.raises(ValueError, match='ELF member differs'): install_sdk.admit_native_sdk(tmp_path, data)
