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


def identity_project(tmp_path, monkeypatch, configure):
    from scripts.objc3c_toolchain import linux_sdk
    from scripts.objc3c_native_validation import plan
    repo = tmp_path / 'project'
    for name in ('scripts/install_objc3c_linux.sh',
                 'out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen'):
        path = repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('qualified input')
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {'configure': configure})
    monkeypatch.setattr(linux_sdk, 'facade_identity', lambda *a: {'sha256': 'frontend'})
    monkeypatch.setattr(plan, 'build_plan', lambda *a, **kw: {'selected_nodes': []})
    return repo, sdk(tmp_path / 'sdk')


def test_reclaiming_foundation_outputs_preserves_release_reuse(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    configure = {'identity': 'with-stage', 'command': ['cmake', '-DASSERTIONS=ON'],
                 'source_tree': 'source', 'build_config_digest': 'config',
                 'sdk_consumers': {'resource_dir': {'path': '/stage/resources', 'sha256': 'headers'}}}
    repo, prefix = identity_project(tmp_path, monkeypatch, configure)
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    configure['identity'] = 'without-stage'
    configure['sdk_consumers']['resource_dir']['sha256'] = None
    assert linux_sdk.input_identity(repo, prefix)['sha256'] == before
    # A mutable build-tree inventory is also an output, not a release input.
    configure['sdk_consumers']['build_tree_resource_dir'] = {'path': '/build/resources', 'sha256': 'new-output'}
    assert linux_sdk.input_identity(repo, prefix)['sha256'] == before


@pytest.mark.parametrize('field', ['command', 'source_tree', 'build_config_digest', 'host_environment'])
def test_foundation_configuration_changes_invalidate_release_reuse(tmp_path, monkeypatch, field):
    from scripts.objc3c_toolchain import linux_sdk
    configure = {'identity': 'plan', 'command': ['cmake', '-DASSERTIONS=ON'],
                 'source_tree': 'source', 'build_config_digest': 'config',
                 'host_environment': {'clang_sha256': 'bootstrap'}}
    repo, prefix = identity_project(tmp_path, monkeypatch, configure)
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    configure[field] = {'changed': configure[field]}
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
        'sha256': 'edited' if changed else 'qualified', 'foundation_contents': 'foundation'})
    monkeypatch.setattr(linux_sdk, 'copy_foundation', lambda _, prefix: prefix.mkdir())
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {
        'configure': {'host_environment': {'resource_limits': {'compile_jobs': 1}}}})
    monkeypatch.setattr(linux_sdk, 'qualify_installed', lambda *a: {'status': 'PASS'})
    monkeypatch.setattr(linux_sdk, 'qualify_frontend_tests', lambda *a, **kw: {'test_executables': 0, 'bytes': 0, 'tests': []})
    monkeypatch.setattr(linux_sdk, 'adopt_candidate_foundation', lambda *a, **kw: {'duplicate_stage_reclaimed': False})

    def execute(command, **kwargs):
        if 'scripts.objc3c_toolchain_runtime.build_linux' in command:
            result = {'stage': str(sdk), 'build_key': 'runtime', 'robin_map_source': str(sdk)}
            destination = Path(command[command.index('--report-file') + 1])
            destination.write_text(json.dumps(result))
            # Cold dependency acquisition may emit diagnostics to the combined
            # command log. Only the explicit report is a machine-readable input.
            kwargs['stdout'].write('Cloning runtime dependencies...\n')
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


def test_candidate_foundation_reclaims_only_the_verified_stage(tmp_path):
    from scripts.objc3c_toolchain import linux_sdk
    stage = tmp_path / 'out/toolchains/objc3-development/stage-linux-x86_64'
    (stage / 'bin').mkdir(parents=True)
    (stage / 'bin/clang').write_bytes(b'qualified foundation')
    prefix = tmp_path / 'candidate'
    linux_sdk.copy_foundation(stage, prefix)
    expected = linux_sdk.foundation_identity(stage)
    result = linux_sdk.adopt_candidate_foundation(tmp_path, stage, prefix, expected_contents=expected)
    assert result['duplicate_stage_reclaimed'] and not stage.exists()
    assert linux_sdk.foundation_identity(prefix) == expected == result['foundation_contents']


def test_damaged_candidate_preserves_its_source_stage(tmp_path):
    from scripts.objc3c_toolchain import linux_sdk
    stage = tmp_path / 'out/toolchains/objc3-development/stage-linux-x86_64'
    (stage / 'bin').mkdir(parents=True)
    (stage / 'bin/clang').write_bytes(b'qualified foundation')
    prefix = tmp_path / 'candidate'
    linux_sdk.copy_foundation(stage, prefix)
    (prefix / 'bin/clang').write_bytes(b'wrong compiler')
    with pytest.raises(ValueError, match='preserving staging'):
        linux_sdk.adopt_candidate_foundation(tmp_path, stage, prefix, expected_contents=linux_sdk.foundation_identity(stage))
    assert (stage / 'bin/clang').read_bytes() == b'qualified foundation'


def test_candidate_never_reclaims_an_installed_foundation(tmp_path):
    from scripts.objc3c_toolchain import linux_sdk
    installed = tmp_path / 'installed-sdk'
    (installed / 'bin').mkdir(parents=True)
    (installed / 'bin/clang').write_bytes(b'qualified foundation')
    prefix = tmp_path / 'candidate'
    linux_sdk.copy_foundation(installed, prefix)
    result = linux_sdk.adopt_candidate_foundation(tmp_path, installed, prefix, expected_contents=linux_sdk.foundation_identity(installed))
    assert not result['duplicate_stage_reclaimed'] and installed.is_dir()
