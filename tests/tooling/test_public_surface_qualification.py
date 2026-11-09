import json
import os
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_toolchain import qualify_public_surfaces as qualification
from scripts.objc3c_toolchain import qualify_typed_keypath_packages as keypaths
from scripts.objc3c_toolchain.qualify_recursive_carriers import compilation_controls

ROOT = Path(__file__).resolve().parents[2]
RECURSIVE_COMPILER = Path(os.environ.get('OBJC3_TEST_RECURSIVE_COMPILER',
    str(ROOT / 'out/toolchains/objc3-development/stage-linux-x86_64/bin/clang')))


@pytest.mark.parametrize('language,standard', [('objective-c', 'c17'),
    ('objective-c++', 'c++20'), ('objective-c++', 'c++23')])
def test_typed_keypath_fixture_exports_its_actual_consumer_contract(tmp_path, language, standard):
    workspace = keypaths.prepare_workspace(tmp_path / 'sdk', tmp_path / 'sources',
        language, standard, 'arc', ROOT / 'tests/clang_foundation/typed_keypath_packages')
    package = json.loads((workspace.parent / 'provider/package.json').read_text())
    exports = package['exports']
    assert len(exports) == len(set(exports))
    assert {'package_checked_scalar_path', 'package_checked_record_path',
            'package_checked_read'} <= set(exports)
    cpp_roots = {name for name in exports if name.startswith('PackageDependentPaths::')}
    assert bool(cpp_roots) == ('++' in language)
    if cpp_roots:
        assert 'PackageDependentPaths::TypedHolder' in cpp_roots
        assert all('PackageDependentPaths::' + operation in cpp_roots
                   for operation in keypaths.CXX_OPERATIONS)


@pytest.mark.skipif(not RECURSIVE_COMPILER.is_file(), reason='requires the native ObjC3 fork')
@pytest.mark.parametrize('standard', ['c++20', 'c++23'])
@pytest.mark.parametrize('arc', [False, True])
def test_recursive_carrier_controls_require_current_admission_and_exact_rejection(
        tmp_path, standard, arc):
    fixtures = ROOT / 'tests/clang_foundation/recursive_carriers'
    for records in (False, True):
        for macro, diagnostic in compilation_controls(arc, records=records).items():
            object_file = tmp_path / (('records-' if records else '') + macro + '.o')
            args = [RECURSIVE_COMPILER, '--no-default-config', '--driver-mode=g++',
                '-std=' + standard, '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2',
                '-fexceptions', '-fcxx-exceptions', '-fobjc-exceptions', '-fobjc-arc-exceptions',
                '-fblocks', '-fno-crash-diagnostics', '-fobjc-arc' if arc else '-fno-objc-arc',
                '-DREJECT_' + ('RECORD_' if records else '') + macro, '-c',
                fixtures / ('records-rejections.mm' if records else 'rejections.mm'),
                '-o', object_file]
            result = subprocess.run(args, capture_output=True, text=True, timeout=30)
            assert result.returncode == (1 if diagnostic else 0), result.stderr
            if diagnostic:
                assert diagnostic in result.stderr and not object_file.exists()
            else:
                assert object_file.read_bytes().startswith(b'\x7fELF')


def row(language, standard, ownership, optimization):
    return {'language': language, 'standard': standard, 'ownership': ownership,
            'optimization': optimization, 'module': {'semantic_fingerprint': 'a' * 64},
            'source_executions': 4, 'binary_executions': 1, 'rejections': [{}] * 8}


def test_failure_joins_all_cases_and_replaces_stale_success(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    output = tmp_path / 'output'
    output.mkdir()
    receipt = output / 'qualification.json'
    receipt.write_text('{"status":"PASS"}')
    called = []

    def invoke(sdk, output, *mode):
        called.append(mode)
        if mode == ('objective-c', 'c17', 'arc', 'O0'):
            raise ValueError('forced public-surface failure')
        return row(*mode)

    monkeypatch.setattr(qualification, 'qualify_case', invoke)
    with pytest.raises(ValueError, match='forced public-surface failure'):
        qualification.qualify(sdk, output, jobs=2)
    assert len(called) == 16
    report = json.loads(receipt.read_text())
    assert report['status'] == 'FAIL' and len(report['cases']) == 15


def test_optimization_cannot_change_public_fingerprint(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()

    def invoke(sdk, output, *mode):
        result = row(*mode)
        result['module']['semantic_fingerprint'] = mode[-1]
        return result

    monkeypatch.setattr(qualification, 'qualify_case', invoke)
    with pytest.raises(ValueError, match='optimization'):
        qualification.qualify(sdk, tmp_path / 'output', jobs=2)
    assert json.loads((tmp_path / 'output/qualification.json').read_text())['status'] == 'FAIL'


@pytest.mark.parametrize('jobs', [0, 5, -1])
def test_qualification_preserves_compiler_budget(tmp_path, jobs):
    sdk = tmp_path / 'sdk'
    sdk.mkdir()
    with pytest.raises(ValueError, match='between 1 and 4'):
        qualification.qualify(sdk, tmp_path / 'output', jobs)
