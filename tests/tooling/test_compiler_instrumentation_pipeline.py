"""A cached ordinary SDK cannot stand in for compiler-process qualification."""
from pathlib import Path

import pytest

from scripts.objc3c_toolchain import compiler_instrumentation as compiler, linux_sdk


def test_optional_sdk_profile_precedes_even_cached_ordinary_delivery(tmp_path, monkeypatch):
    order = []
    def instrument(repo, **options):
        order.append(('compiler', options))
        return {'status': 'PASS', 'subject': 'compiler-implementation', 'profile': compiler.PROFILE}
    ordinary = {'status': 'PASS', 'cache_hit': True}
    def deliver(*args):
        order.append(('sdk', None))
        return ordinary
    monkeypatch.setattr(compiler, 'build_and_qualify', instrument)
    monkeypatch.setattr(linux_sdk, '_build_sdk', deliver)
    result = linux_sdk.build_sdk(tmp_path, compiler_instrumentation_profile=compiler.PROFILE)
    assert [item[0] for item in order] == ['compiler', 'sdk']
    assert result['compiler_implementation_qualification']['subject'] == 'compiler-implementation'
    assert 'compiler_implementation_qualification' not in ordinary


def test_failed_compiler_profile_stops_sdk_delivery(tmp_path, monkeypatch):
    def fail(*args, **kwargs):
        raise ValueError('host compiler instrumentation failed')
    def forbidden(*args):
        pytest.fail('SDK delivery ran after compiler-process failure')
    monkeypatch.setattr(compiler, 'build_and_qualify', fail)
    monkeypatch.setattr(linux_sdk, '_build_sdk', forbidden)
    with pytest.raises(ValueError, match='instrumentation failed'):
        linux_sdk.build_sdk(tmp_path, compiler_instrumentation_profile=compiler.PROFILE)


def test_ordinary_sdk_does_not_select_a_compiler_process_profile(tmp_path, monkeypatch):
    def forbidden(*args, **kwargs):
        pytest.fail('ordinary SDK unexpectedly selected compiler instrumentation')
    monkeypatch.setattr(compiler, 'build_and_qualify', forbidden)
    monkeypatch.setattr(linux_sdk, '_build_sdk', lambda *args: {'status': 'PASS'})
    assert linux_sdk.build_sdk(tmp_path) == {'status': 'PASS'}


def test_unsupported_compiler_profile_cannot_start_a_build(tmp_path):
    with pytest.raises(ValueError, match='unknown compiler implementation'):
        compiler.build_and_qualify(tmp_path, profile='compiler-msan')
