from pathlib import Path
import os
import json
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.linux_sdk import copy_foundation, foundation_identity
from scripts.objc3c_toolchain.linux_host import native_linux_host, native_target

FOUNDATION_RECIPE = {'upstream_commit': 'upstream', 'patch_digest': 'patches',
                     'build_config_digest': 'configuration'}


@pytest.fixture(autouse=True)
def independent_runtime_dependency(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import compiler_runtimes
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(tmp_path / 'bootstrap'))
    monkeypatch.setattr(compiler_runtimes, 'discover', lambda *a: {'sha256': 'builtin-input'})
    monkeypatch.setattr(compiler_runtimes, 'materialize', lambda *a: {'files': 0})


def sdk(tmp_path: Path) -> Path:
    for name, content in {'bin/clang': 'compiler', 'lib/libclangAST.a': 'native AST',
                          'include/clang/AST/Type.h': 'native types',
                          'bin/objc3c': 'frontend', 'bin/clang-scan-deps': 'scanner',
                          'bin/FileCheck': 'qualification tool',
                          'lib/libobjc.so': 'runtime'}.items():
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
    return tmp_path


def test_project_updates_reuse_the_same_llvm_foundation(tmp_path):
    prefix = sdk(tmp_path)
    before = foundation_identity(prefix)
    for name in ('bin/objc3c', 'bin/clang-scan-deps', 'bin/FileCheck', 'lib/libobjc.so'):
        (prefix / name).write_text('updated project component')
    assert foundation_identity(prefix) == before


def test_native_header_bundle_is_reinstalled_without_invalidating_llvm_foundation(tmp_path):
    prefix = sdk(tmp_path / 'cached')
    before = foundation_identity(prefix)
    files = ('bin/clang.cfg', 'bin/clang++.cfg',
             'lib/clang/23/objc3-native-headers/roots/00/string',
             'lib/clang/23/objc3-native-headers/manifest.json')
    for name in files:
        path = prefix / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('current native headers')
    assert foundation_identity(prefix) == before
    destination = tmp_path / 'candidate'
    copy_foundation(prefix, destination)
    assert all(not (destination / name).exists() for name in files)
    assert (destination / 'bin/clang').read_text() == 'compiler'


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
    stale = ('bin/objc3c-clang', 'bin/objc3-managed-sample.o', 'bin/FileCheck',
             'lib/libobjc3c_removed.a', 'lib/cmake/objc3c/Removed.cmake',
             'include/objc/retired.h', 'include/objc3c/retired.h',
             'include/runtime/retired.h', 'include/objc3/retired.h', 'share/objc3c/examples/retired.m',
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
    for name in ('scripts/install_objc3c_linux.sh', 'stdlib/module_inventory.json', 'toolchains/linux-hosts.json',
                 f'out/toolchains/objc3-development/build-{native_linux_host()}/bin/llvm-tblgen',
                 f'out/toolchains/objc3-development/build-{native_linux_host()}/bin/FileCheck', fixture):
        path = repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('initial contents')
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {'configure': {'identity': 'foundation'}})
    monkeypatch.setattr(linux_sdk, 'facade_identity', lambda *a: {'sha256': 'frontend'})
    monkeypatch.setattr(linux_sdk, 'discover_native_headers', lambda *a: {'sha256': 'native-headers'})
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
    for name in ('scripts/install_objc3c_linux.sh', 'stdlib/module_inventory.json', 'toolchains/linux-hosts.json',
                 f'out/toolchains/objc3-development/build-{native_linux_host()}/bin/llvm-tblgen',
                 f'out/toolchains/objc3-development/build-{native_linux_host()}/bin/FileCheck'):
        path = repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('qualified input')
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {'configure': configure})
    monkeypatch.setattr(linux_sdk, 'facade_identity', lambda *a: {'sha256': 'frontend'})
    monkeypatch.setattr(linux_sdk, 'discover_native_headers', lambda *a: {'sha256': 'native-headers'})
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


def test_native_header_bytes_change_release_inputs_independently_of_llvm(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk, native_headers
    repo, prefix = identity_project(tmp_path, monkeypatch, {'identity': 'foundation'})
    header = tmp_path / 'native-header'
    header.write_bytes(b'first header')
    monkeypatch.setattr(linux_sdk, 'discover_native_headers',
                        lambda *a: {'sha256': native_headers.digest(header)})
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    timestamp = header.stat()
    header.write_bytes(b'other header')
    os.utime(header, ns=(timestamp.st_atime_ns, timestamp.st_mtime_ns))
    assert linux_sdk.input_identity(repo, prefix)['sha256'] != before


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
        **FOUNDATION_RECIPE,
        'source_build_id': 'llvm-build', 'seal_sha256': 'seal', 'target': native_target()})
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda *a: {**FOUNDATION_RECIPE, 'build_id': 'llvm-build'})
    changed = False
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *a: {
        'sha256': 'edited' if changed else 'qualified', 'foundation_contents': 'foundation',
        'native_test_inputs_sha256': 'test-inputs',
        'native_headers': {'sha256': 'native-headers'}, 'compiler_runtimes': {'sha256': 'builtin-input'}})
    monkeypatch.setattr(linux_sdk, 'discover_native_headers', lambda *a: {'sha256': 'native-headers'})
    monkeypatch.setattr(linux_sdk, 'materialize_native_headers', lambda *a: {'files': 0})
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

    def package(*args, checkpoint):
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
    stage = tmp_path / f'out/toolchains/objc3-development/stage-{native_linux_host()}'
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
    stage = tmp_path / f'out/toolchains/objc3-development/stage-{native_linux_host()}'
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


@pytest.mark.parametrize('name', [
    'stdlib/include/objc3/text.h', 'stdlib/module_inventory.json',
    f'out/toolchains/objc3-development/build-{native_linux_host()}/bin/FileCheck'])
def test_library_and_qualification_tool_edits_invalidate_sdk_reuse(tmp_path, monkeypatch, name):
    from scripts.objc3c_toolchain import linux_sdk
    repo, prefix = identity_project(tmp_path, monkeypatch, {'command': ['cmake']})
    path = repo / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text('before')
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    path.write_text('after')
    assert linux_sdk.input_identity(repo, prefix)['sha256'] != before


def test_packaged_filecheck_does_not_change_foundation_signature(tmp_path):
    prefix = sdk(tmp_path)
    (prefix / 'bin/FileCheck').unlink()
    before = foundation_identity(prefix)
    (prefix / 'bin/FileCheck').write_bytes(b'newly packaged qualification tool')
    assert foundation_identity(prefix) == before


@pytest.mark.parametrize('name', ['native/objc3c/tests/keypath_service_test.cpp',
                                 'native/objc3c/tests/support/new_oracle.hpp',
                                 'tests/clang_foundation/interface_import_test.cpp',
                                 'tests/tooling/fixtures/clang_foundation/support.hpp'])
def test_native_test_sources_invalidate_qualification_without_rebuilding_foundation(tmp_path, monkeypatch, name):
    from scripts.objc3c_toolchain import linux_sdk
    repo, prefix = identity_project(tmp_path, monkeypatch, {'source_build_id': 'same-foundation'})
    path = repo / name
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text('pass')
    before = linux_sdk.input_identity(repo, prefix)
    stamp = path.stat()
    path.write_text('fail')
    import os
    os.utime(path, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    after = linux_sdk.input_identity(repo, prefix)
    assert after['sha256'] != before['sha256']
    assert after['native_test_inputs_sha256'] != before['native_test_inputs_sha256']
    assert after['foundation_plan'] == before['foundation_plan']
    assert after['frontend'] == before['frontend']
    path.write_text('pass')
    os.utime(path, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert linux_sdk.input_identity(repo, prefix) == before
    path.unlink()
    assert linux_sdk.input_identity(repo, prefix)['native_test_inputs_sha256'] != before['native_test_inputs_sha256']


def test_native_test_edit_enters_fresh_qualification_instead_of_installing_old_release(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    repo, prefix = identity_project(tmp_path, monkeypatch, {'source_build_id': 'same-foundation'})
    source = repo / 'native/objc3c/tests/keypath_service_test.cpp'
    source.parent.mkdir(parents=True)
    source.write_text('return 0;')
    original = linux_sdk.input_identity(repo, prefix)
    stamp = source.stat()
    builds = repo / 'out/toolchains/linux-sdk-builds'
    original_receipt = builds / original['sha256'] / 'release.json'
    original_receipt.parent.mkdir(parents=True)
    original_receipt.write_text(json.dumps({'status': 'PASS', 'input_identity': original['sha256'],
        'archive': 'expected.tar.zst', 'archive_sha256': 'archive'}))
    monkeypatch.setattr(linux_sdk, 'verify_sdk', lambda *_: {
        **FOUNDATION_RECIPE,
        'source_build_id': 'llvm-build', 'seal_sha256': 'seal', 'target': native_target()})
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda *_: {**FOUNDATION_RECIPE, 'build_id': 'llvm-build'})
    installations, candidates = [], []
    monkeypatch.setattr(linux_sdk, 'install', lambda *args: installations.append(args) or {})

    def enter_fresh_qualification(foundation, candidate):
        candidates.append((foundation, candidate))
        raise ValueError('fresh candidate reached')

    monkeypatch.setattr(linux_sdk, 'copy_foundation', enter_fresh_qualification)
    assert linux_sdk.build_sdk(repo, foundation_sdk=prefix)['cache_hit']
    source.write_text('return 1;')
    os.utime(source, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    edited = linux_sdk.input_identity(repo, prefix)
    with pytest.raises(ValueError, match='fresh candidate reached'):
        linux_sdk.build_sdk(repo, foundation_sdk=prefix)
    assert len(installations) == 1
    assert candidates == [(prefix, builds / edited['sha256'] / 'prefix')]
    assert not (builds / edited['sha256'] / 'release.json').exists()
    source.write_text('return 0;')
    os.utime(source, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert linux_sdk.build_sdk(repo, foundation_sdk=prefix)['cache_hit']
    assert len(installations) == 2


def test_changed_recipe_routes_active_sdk_to_the_foundation_builder(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    from scripts.objc3c_toolchain.core import FoundationConfigurationChanged

    repo = tmp_path / 'repo'
    install_root = repo / 'installed'
    active = install_root / 'current'
    active.mkdir(parents=True)
    (active / 'objc3-sdk.json').write_text(json.dumps({'source_build_id': 'previous-configuration'}))
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(tmp_path))

    def changed(_repo):
        raise FoundationConfigurationChanged('development build configuration changed')

    def rebuild(root, **options):
        assert root == repo and options['stage'] is True
        raise RuntimeError('entered current foundation build')

    monkeypatch.setattr(linux_sdk, 'verify_development', changed)
    monkeypatch.setattr(linux_sdk, 'verify_sdk', lambda *_: pytest.fail('must not reuse old SDK'))
    monkeypatch.setattr(linux_sdk, 'build_foundation', rebuild)
    with pytest.raises(RuntimeError, match='entered current foundation build'):
        linux_sdk.build_sdk(repo, install_root=install_root)


@pytest.mark.parametrize('field,diagnostic', [('build_config_digest', 'build configuration differs'),
                                              ('patch_digest', 'source differs at patch_digest'),
                                              ('target', 'targets a different machine')])
def test_a_matching_display_id_cannot_override_full_foundation_identity(tmp_path, monkeypatch, field, diagnostic):
    from scripts.objc3c_toolchain import linux_sdk

    prefix = tmp_path / 'sdk'
    prefix.mkdir()
    candidate = {**FOUNDATION_RECIPE, 'source_build_id': 'llvm-build', 'seal_sha256': 'seal',
                 'target': native_target()}
    candidate[field] = 'different'
    monkeypatch.setattr(linux_sdk, 'verify_sdk', lambda *_: candidate)
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda *_: {**FOUNDATION_RECIPE, 'build_id': 'llvm-build'})
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *_: pytest.fail('ineligible foundation was admitted'))
    with pytest.raises(ValueError, match=diagnostic):
        linux_sdk.build_sdk(tmp_path / 'repo', foundation_sdk=prefix)


@pytest.mark.parametrize('source_directory', ['native/objc3c/tests', 'tests/clang_foundation'])
def test_registered_keypath_test_rebuilds_at_the_same_mtime_when_requalified(tmp_path, monkeypatch,
                                                                           source_directory):
    selected = os.environ.get('OBJC3_TEST_NATIVE_REUSE_SDK')
    if not selected:
        pytest.skip('requires an installed Linux SDK for the focused native reuse witness')
    from scripts.objc3c_toolchain import linux_sdk
    from scripts.objc3c_toolchain.frontend_tests import qualify

    installed = Path(selected).resolve(strict=True)
    checkout = Path(__file__).resolve().parents[2]
    repo, prefix = identity_project(tmp_path, monkeypatch, {'source_build_id': 'same-foundation'})
    native = repo / 'native/objc3c'
    for relative in ('tests/test_assertion_policy.h', 'cmake/FrontendTests.cmake'):
        destination = native / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(checkout / 'native/objc3c' / relative, destination)
    source = repo / source_directory / 'keypath_service_test.cpp'
    source.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(checkout / 'native/objc3c/tests/keypath_service_test.cpp', source)
    header = native / 'runtime/public/objc3_keypath.h'
    header.parent.mkdir(parents=True)
    shutil.copyfile(installed / 'include/objc3c/runtime/objc3_keypath.h', header)
    (native / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(keypath_reuse_witness LANGUAGES CXX)\n'
        'set(CMAKE_CXX_STANDARD 20)\nenable_testing()\n'
        'add_library(objc3c_build_defaults INTERFACE)\n'
        'target_include_directories(objc3c_build_defaults INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")\n'
        'include(cmake/FrontendTests.cmake)\n'
        f'add_executable(objc3_keypath_service_test "{source}")\n'
        f'target_link_libraries(objc3_keypath_service_test PRIVATE "{installed / "lib/libobjc3_runtime_services.a"}")\n'
        'objc3c_add_frontend_test(NAME objc3_keypath_service COMMAND objc3_keypath_service_test)\n'
        'objc3c_write_frontend_test_inventory()\n')
    original, timestamp = source.read_bytes(), source.stat()
    assertion = b'assert(objc3_keypath_validate_v2(&descriptor) == OBJC3_KEYPATH_OK);'
    assert original.count(assertion) == 1
    edited = original.replace(assertion, assertion.replace(b' == ', b' != '))
    assert len(edited) == len(original)
    build = tmp_path / 'warm-frontend'

    def execute(phase, name, argv):
        result = subprocess.run(argv, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=60)
        (tmp_path / (phase + '-' + name + '.log')).write_text(result.stdout)
        result.check_returncode()

    identities = []
    for phase, data in (('baseline', original), ('edited', edited), ('restored', original)):
        source.write_bytes(data)
        os.utime(source, ns=(timestamp.st_atime_ns, timestamp.st_mtime_ns))
        assert source.stat().st_mtime_ns == timestamp.st_mtime_ns
        identity = linux_sdk.input_identity(repo, prefix)
        identities.append({key: identity[key] for key in ('sha256', 'native_test_inputs_sha256', 'foundation_plan')})
        execute(phase, 'configure', ['cmake', '-S', native, '-B', build, '-G', 'Ninja',
            '-DCMAKE_BUILD_TYPE=Release', f'-DCMAKE_CXX_COMPILER={installed / "bin/clang++"}',
            f'-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld={installed / "bin/ld.lld"}',
            f'-DOBJC3C_NATIVE_TEST_INPUT_SHA256={identity["native_test_inputs_sha256"]}'])
        output = tmp_path / phase
        if phase == 'edited':
            with pytest.raises(ValueError, match='frontend test commands failed'):
                qualify(build, output, lambda name, argv: execute(phase, name, argv), jobs=1)
            failure = json.loads((output / 'qualification.json').read_text())
            assert failure['status'] == 'FAIL' and failure['failures'][0]['phase'] == 'ctest'
            diagnostic = (tmp_path / 'edited-frontend-objc3_keypath_service-ctest.log').read_text()
            assert 'objc3_keypath_validate_v2(&descriptor) != OBJC3_KEYPATH_OK' in diagnostic
        else:
            result = qualify(build, output, lambda name, argv: execute(phase, name, argv), jobs=1)
            assert result['status'] == 'PASS' and result['frontend_ctests'] == 1
            assert result['test_executables'] == 1
    assert identities[0] == identities[2]
    assert identities[0]['sha256'] != identities[1]['sha256']
    assert identities[0]['native_test_inputs_sha256'] != identities[1]['native_test_inputs_sha256']
    assert len({record['foundation_plan'] for record in identities}) == 1
    (tmp_path / 'input-identities.json').write_text(json.dumps(identities, indent=2))


@pytest.mark.parametrize('shape', ['file', 'directory', 'root', 'cycle', 'broken'])
def test_qualification_input_resolution_rejects_escaping_or_incomplete_trees(tmp_path, monkeypatch, shape):
    from scripts.objc3c_toolchain import linux_sdk
    repo, prefix = identity_project(tmp_path, monkeypatch, {'source_build_id': 'same-foundation'})
    native = repo / 'native/objc3c/tests'
    native.parent.mkdir(parents=True, exist_ok=True)
    outside = tmp_path / 'outside'
    outside.mkdir()
    (outside / 'native_case.cpp').write_text('outside oracle')
    if shape == 'root':
        native.symlink_to(outside, target_is_directory=True)
    else:
        native.mkdir()
        target = {'file': outside / 'native_case.cpp', 'directory': outside,
                  'cycle': native, 'broken': native / 'missing'}[shape]
        (native / 'linked').symlink_to(target)
    expected = {'cycle': 'directory cycle', 'broken': 'cannot be resolved'}.get(shape, 'escapes the checkout')
    with pytest.raises(ValueError, match=expected):
        linux_sdk.input_identity(repo, prefix)


def test_qualification_binds_in_checkout_linked_test_contents_and_target(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    repo, prefix = identity_project(tmp_path, monkeypatch, {'source_build_id': 'same-foundation'})
    native = repo / 'native/objc3c/tests'
    native.mkdir(parents=True, exist_ok=True)
    left, right = repo / 'left-fixtures', repo / 'right-fixtures'
    for directory in (left, right):
        directory.mkdir()
        (directory / 'native_case.cpp').write_text('same oracle')
    link = native / 'shared'
    link.symlink_to(left, target_is_directory=True)
    before = linux_sdk.input_identity(repo, prefix)
    key = 'native/objc3c/tests/shared/native_case.cpp'
    assert before['input_resolution'][key] == 'left-fixtures/native_case.cpp'
    link.unlink()
    link.symlink_to(right, target_is_directory=True)
    after = linux_sdk.input_identity(repo, prefix)
    assert after['input_resolution'][key] == 'right-fixtures/native_case.cpp'
    assert after['inputs'][key] == before['inputs'][key]
    assert after['sha256'] != before['sha256']
    assert after['foundation_plan'] == before['foundation_plan']


@pytest.mark.parametrize(('status', 'identity', 'reusable'), [
    ('PASS', 'qualified', True), ('PASS', 'other', False),
    ('FAIL', 'qualified', False), ('PASS', None, False),
])
def test_cache_hit_requires_a_pass_receipt_for_the_selected_identity(
        tmp_path, monkeypatch, status, identity, reusable):
    from scripts.objc3c_toolchain import linux_sdk
    repo = tmp_path / 'repo'
    directory = repo / 'out/toolchains/linux-sdk-builds/qualified'
    directory.mkdir(parents=True)
    prefix = sdk(tmp_path / 'foundation')
    monkeypatch.setattr(linux_sdk, 'verify_sdk', lambda *_: {
        **FOUNDATION_RECIPE,
        'source_build_id': 'llvm-build', 'seal_sha256': 'seal', 'target': native_target()})
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda *_: {**FOUNDATION_RECIPE, 'build_id': 'llvm-build'})
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *_: {'sha256': 'qualified'})
    installations = []
    monkeypatch.setattr(linux_sdk, 'install', lambda *args: installations.append(args) or {})
    receipt = {'status': status, 'archive': 'expected.tar.zst', 'archive_sha256': 'archive'}
    if identity is not None:
        receipt['input_identity'] = identity
    (directory / 'release.json').write_text(json.dumps(receipt))
    if reusable:
        assert linux_sdk.build_sdk(repo, foundation_sdk=prefix)['cache_hit']
        assert len(installations) == 1
    else:
        with pytest.raises(ValueError, match='cached release receipt does not match'):
            linux_sdk.build_sdk(repo, foundation_sdk=prefix)
        assert installations == []
