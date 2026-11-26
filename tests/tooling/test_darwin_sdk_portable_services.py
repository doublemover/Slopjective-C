"""Exercise the bounded archive owner and the installed public C++ surface."""
import os
from pathlib import Path
import shutil
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain import darwin_sdk_portable_services as portable
from scripts.objc3c_toolchain.darwin_bootstrap import verify_macho
from scripts.objc3c_toolchain.package import _file_hash, inventory, normalize_permissions


def pinned_tools(*names):
    root = Path(os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT', '/missing')) / 'bin'
    if not all((root / name).is_file() for name in names):
        pytest.skip('requires the pinned LLVM bootstrap tools')
    return root


def run(*args, cwd):
    return subprocess.run(list(map(str, args)), cwd=cwd, check=True, text=True, capture_output=True, timeout=60)


def test_optional_portable_scope_attests_the_two_compiled_macho_members(tmp_path):
    tools = pinned_tools('clang', 'llvm-ar')
    assert portable.manifest_files({}) == set()
    prefix = tmp_path / 'Relocated SDK'
    (prefix / 'bin').mkdir(parents=True)
    shutil.copy2(tools / 'llvm-ar', prefix / 'bin/llvm-ar')
    stages = tmp_path / 'build'
    stages.mkdir()
    layout = tmp_path / 'Install candidate'
    layout.mkdir()
    suites = {}
    for index, (name, suite) in enumerate(portable.MEMBERS.items()):
        source = stages / f'member{index}.c'
        source.write_text(f'int member{index}(void) {{ return {index + 1}; }}\n')
        obj = stages / name
        run(tools / 'clang', '--no-default-config', '--target=arm64-apple-macos15.0',
            '-c', source, '-o', obj, cwd=stages)
        suites[suite] = {'objects': {name: verify_macho(obj, 'object')}}
    sources = {name: _file_hash(portable.ROOT / name) for name in portable._witness_sources()}
    registration = {'status': 'registration-verified', 'support_truth': False, 'installation_root': str(layout),
                    'portable_services': {'status': 'PASS', 'inputs': sources, 'suites': suites}}
    descriptor = portable.stage_payload(prefix, registration)
    normalize_permissions(prefix)
    data = {portable.KEY: descriptor, 'files': inventory(prefix), 'compiler_host_libcxx': {
        'header_config_sha256': 'a'*64, 'library_stub_sha256': 'b'*64, 'reported_version': 'Xcode libc++ 190000'}}
    assert portable.manifest_files(data) == {portable.LIBRARY, portable.EXAMPLE, *portable.HEADERS}
    portable.validate_payload(prefix, data)
    with pytest.raises(ValueError, match=r'Xcode libc\+\+ identity'):
        portable.manifest_files({**data, 'compiler_host_libcxx': {}})
    claimed = {**descriptor, 'objects': {name: 'f'*64 for name in descriptor['objects']}}
    with pytest.raises(ValueError, match='qualified native objects'):
        portable.validate_payload(prefix, {**data, portable.KEY: claimed})
    claimed = {**descriptor, 'files': {**descriptor['files'], 'lib/unrelated.a': 'e'*64}}
    with pytest.raises(ValueError, match='reviewed scope'):
        portable.manifest_files({**data, portable.KEY: claimed})
    header = next(iter(portable.HEADERS))
    (prefix / header).write_text('/* changed installed header */\n')
    with pytest.raises(ValueError, match='differs from its producer receipt'):
        portable.validate_payload(prefix, data)


@pytest.mark.skipif(sys.platform != 'linux', reason='the portable installed-source check here uses a Linux C++ host')
def test_cold_cpp_example_uses_only_installed_headers_and_the_two_real_services(tmp_path):
    tools = pinned_tools('clang', 'llvm-ar', 'ld.lld')
    prefix = tmp_path / 'Relocated SDK'
    (prefix / 'lib').mkdir(parents=True)
    for target, source in {**portable.HEADERS, portable.EXAMPLE: portable.EXAMPLE_SOURCE}.items():
        path = prefix / target
        path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(portable.ROOT / source, path)
    compiler = [tools / 'clang', '--no-default-config', '--driver-mode=g++', '-std=c++20', '-O2',
                '-Wall', '-Wextra', '-Werror']
    objects = []
    for name in portable.MEMBERS:
        source = portable.NATIVE / 'src/runtime/stdlib' / Path(name).with_suffix('.cpp')
        obj = tmp_path / name
        run(*compiler, '-I', portable.NATIVE / 'src', '-c', source, '-o', obj, cwd=tmp_path)
        objects.append(obj)
    run(tools / 'llvm-ar', 'rcs', prefix / portable.LIBRARY, *objects, cwd=tmp_path)
    project = tmp_path / 'Fresh consumer'
    project.mkdir()
    source = project / 'example.cpp'
    shutil.copyfile(prefix / portable.EXAMPLE, source)
    run(*compiler, '-I', prefix / 'include', '-MMD', '-MF', 'example.d', '-c', source, '-o', 'example.o', cwd=project)
    portable._check_consumer_inputs(source, project / 'example.d', prefix, None)
    linker = [*compiler, '--ld-path=' + str(tools / 'ld.lld'), 'example.o', '-pthread']
    with pytest.raises(subprocess.CalledProcessError) as missing:
        run(*linker, '-o', 'missing', cwd=project)
    assert 'objc3_runtime_stdlib_map_i32_i32_v2_create' in missing.value.stderr
    assert 'objc3_runtime_stdlib_text_v2_create_utf8_checked' in missing.value.stderr
    run(*linker, prefix / portable.LIBRARY, '-o', 'example', cwd=project)
    execution = run(project / 'example', cwd=project)
    assert execution.stdout.strip() == portable.WITNESS and not execution.stderr
