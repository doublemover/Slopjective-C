from pathlib import Path
import os
import json

import pytest

from scripts.objc3c_toolchain.linux_sdk import copy_foundation, foundation_identity


def sdk(tmp_path: Path) -> Path:
    for name, content in {'bin/clang': 'compiler', 'lib/libclangAST.a': 'native AST',
                          'include/clang/AST/Type.h': 'native types',
                          'bin/objc3c': 'frontend', 'bin/clang-scan-deps': 'scanner',
                          'lib/libobjc.so': 'runtime'}.items():
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
    return tmp_path


def test_project_updates_reuse_the_same_llvm_foundation(tmp_path):
    prefix = sdk(tmp_path)
    before = foundation_identity(prefix)
    for name in ('bin/objc3c', 'bin/clang-scan-deps', 'lib/libobjc.so'):
        (prefix / name).write_text('updated project component')
    assert foundation_identity(prefix) == before


def test_llvm_library_change_invalidates_reuse_even_with_preserved_timestamp(tmp_path):
    prefix = sdk(tmp_path)
    before = foundation_identity(prefix)
    library = prefix / 'lib/libclangAST.a'
    prior = library.stat()
    library.write_bytes(b'X' * prior.st_size)
    os.utime(library, ns=(prior.st_atime_ns, prior.st_mtime_ns))
    assert foundation_identity(prefix) != before


def test_cached_foundation_cannot_restore_removed_project_components(tmp_path):
    prefix = sdk(tmp_path / 'cached')
    stale = ('bin/objc3c-clang', 'bin/objc3-managed-sample.o',
             'lib/libobjc3c_removed.a', 'lib/cmake/objc3c/Removed.cmake',
             'include/objc/retired.h', 'include/objc3c/retired.h',
             'include/runtime/retired.h', 'share/objc3c/examples/retired.m',
             'objc3-sdk.json')
    for name in stale:
        path = prefix / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('removed project component')
    (prefix / 'bin/clang++').symlink_to('clang')
    output = tmp_path / 'candidate'
    copy_foundation(prefix, output)
    for name in (*stale, 'bin/objc3c', 'bin/clang-scan-deps', 'lib/libobjc.so'):
        assert not (output / name).exists()
    assert (output / 'lib/libclangAST.a').read_text() == 'native AST'
    assert (output / 'bin/clang++').is_symlink()
    assert (output / 'bin/clang++').read_text() == 'compiler'


def test_editor_qualification_changes_invalidate_release_reuse(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    from scripts.objc3c_native_validation import plan
    repo = tmp_path / 'project'
    fixture = 'tests/tooling/fixtures/objc3_editor/qualification-cases.json'
    for name in ('scripts/install_objc3c_linux.sh',
                 'out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen', fixture):
        path = repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('initial contents')
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {'configure': {'identity': 'foundation'}})
    monkeypatch.setattr(linux_sdk, 'facade_identity', lambda *a: {'sha256': 'frontend'})
    monkeypatch.setattr(plan, 'build_plan', lambda *a, **kw: {
        'selected_nodes': [{'identity_inputs': [fixture]}]})
    prefix = sdk(tmp_path / 'sdk')
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    (repo / fixture).write_text('updated qualification expectations')
    assert linux_sdk.input_identity(repo, prefix)['sha256'] != before


def test_packaging_input_edit_prevents_installation(tmp_path, monkeypatch):
    """Edits during the installed source suite must not publish stale evidence."""
    from scripts.objc3c_toolchain import linux_sdk
    from types import SimpleNamespace
    repo = tmp_path / 'repo'
    repo.mkdir()
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(sdk))
    monkeypatch.setattr(linux_sdk, 'verify_sdk', lambda *a: {
        'source_build_id': 'llvm-build', 'seal_sha256': 'seal'})
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda *a: {'build_id': 'llvm-build'})
    changed = False
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *a: {
        'sha256': 'edited' if changed else 'qualified'})
    monkeypatch.setattr(linux_sdk, 'copy_foundation', lambda _, prefix: prefix.mkdir())
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {
        'configure': {'host_environment': {'resource_limits': {'compile_jobs': 1}}}})
    monkeypatch.setattr(linux_sdk, 'qualify_installed', lambda *a: {'status': 'PASS'})

    def execute(command, **kwargs):
        if 'scripts.objc3c_toolchain_runtime.build_linux' in command:
            json.dump({'stage': str(sdk), 'build_key': 'runtime', 'robin_map_source': str(sdk)},
                      kwargs['stdout'])
        return SimpleNamespace(returncode=0)

    def package(*args):
        nonlocal changed
        changed = True
        return {'archive': str(tmp_path / 'sdk.tar.zst'), 'archive_sha256': 'archive', 'seal_sha256': 'seal'}

    def install(*args):
        pytest.fail('a candidate with edited packaging inputs must not be installed')

    monkeypatch.setattr(linux_sdk.subprocess, 'run', execute)
    monkeypatch.setattr(linux_sdk, 'package_sdk', package)
    monkeypatch.setattr(linux_sdk, 'install', install)
    with pytest.raises(ValueError, match='inputs changed during packaging'):
        linux_sdk.build_sdk(repo, foundation_sdk=sdk)
    report = json.loads((repo / 'out/toolchains/linux-sdk-builds/qualified/build-report.json').read_text())
    assert report['status'] == 'FAIL'
    assert not (repo / 'out/toolchains/linux-sdk-builds/qualified/release.json').exists()
