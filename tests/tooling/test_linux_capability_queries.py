from contextlib import nullcontext
import json
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import qualify_linux_capabilities as queries
from scripts.objc3c_toolchain.generate_features import AUTHORITY, generated


@pytest.mark.parametrize('target', queries.LINUX_TARGETS)
def test_catalog_controls_positive_and_negative_profile_assertions(target):
    _, rendered = generated(json.loads(AUTHORITY.read_text()))
    view = json.loads(rendered)
    compatibility, enabled = queries.consumer(view, target, 'compatibility')
    strict, restricted = queries.consumer(view, target, 'strict')
    assert 'defined(__OBJC3_FEATURE_OPTIONAL__) && __OBJC3_FEATURE_OPTIONAL__ == 1' in compatibility
    assert '__has_feature(objc3_optional) == 1' in compatibility
    assert '__has_extension(objc3_optional) == 1' in compatibility
    assert 'defined(__OBJC3_FEATURE_OPTIONAL__) && __OBJC3_FEATURE_OPTIONAL__ == 0' in strict
    assert '__has_feature(objc3_optional) == 0' in strict
    assert next(row for row in enabled if row['feature'] == 'optionals')['advertised']
    assert all(bool(row['witnesses']) == row['advertised'] for row in enabled)
    assert not next(row for row in restricted if row['feature'] == 'optionals')['advertised']


def test_real_clang_rejects_feature_advertised_without_opt_in():
    clang = shutil.which('clang')
    if clang is None:
        pytest.skip('requires stock Clang')
    _, rendered = generated(json.loads(AUTHORITY.read_text()))
    source, _ = queries.consumer(json.loads(rendered), queries.LINUX_TARGETS[0], None)
    cmd = [clang, '--no-default-config', '-x', 'objective-c', '-fsyntax-only', '-']
    passing = subprocess.run(cmd, input=source, text=True, capture_output=True)
    assert passing.returncode == 0, passing.stderr
    contaminated = subprocess.run([*cmd, '-D__OBJC3_FEATURE_OPTIONAL__=1'], input=source,
                                  text=True, capture_output=True)
    assert contaminated.returncode != 0
    assert '__OBJC3_FEATURE_OPTIONAL__ requires Objective-C 3' in contaminated.stderr


def test_failed_installed_check_removes_a_previous_pass(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    (sdk / 'bin').mkdir(parents=True)
    compiler = sdk / 'bin/clang'
    compiler.write_text('compiler')
    (sdk / 'objc3-sdk.json').write_text(json.dumps({
        'target': queries.LINUX_TARGETS[1], 'seal_sha256': 'sealed', 'source_build_id': 'source',
    }))
    report_path = tmp_path / 'report.json'
    monkeypatch.setattr(queries, 'sdk_verification', lambda _: nullcontext())

    def run(argv, **kwargs):
        assert argv[0] == str(compiler)
        assert '--target=' + queries.LINUX_TARGETS[1] in argv
        assert '-fobjc-runtime=gnustep-2.2' in argv
        assert kwargs['env']['LD_LIBRARY_PATH'] == str(sdk / 'lib')
        return subprocess.CompletedProcess(argv, 0, '', '')

    monkeypatch.setattr(queries.subprocess, 'run', run)
    report = queries.qualify(sdk, AUTHORITY, report_path)
    assert report['sdk_seal'] == 'sealed'
    assert len(report['commands']) == 10
    assert json.loads(report_path.read_text()) == report

    monkeypatch.setattr(queries.subprocess, 'run', lambda argv, **_: subprocess.CompletedProcess(
        argv, 1, '', 'missing feature macro'))
    with pytest.raises(ValueError, match='missing feature macro'):
        queries.qualify(sdk, AUTHORITY, report_path)
    assert not report_path.exists()
