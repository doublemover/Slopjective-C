"""Exercise strict policy and real paired detector controls, including weakened lanes."""
from pathlib import Path
import os
import subprocess

import pytest

from scripts.objc3c_toolchain import sanitizer_policy as policy

ROOT = Path(__file__).resolve().parents[2]
COMPILER = ROOT / 'out/toolchains/objc3-development/stage-linux-x86_64/bin/clang'
RESOURCE = Path(os.environ.get('OBJC3_TEST_SANITIZER_RESOURCE',
                               str(COMPILER.parent.parent / 'lib/clang/23')))
SYMBOLIZER = RESOURCE.parents[2] / 'bin/llvm-symbolizer'
NATIVE_READY = COMPILER.is_file() and SYMBOLIZER.is_file() and any(RESOURCE.rglob('libclang_rt.asan*.a'))


def test_ambient_options_and_suppressions_are_replaced():
    ambient = {'HOME': '/home/fixture', 'TMPDIR': '/tmp', 'PATH': '/untrusted',
               'ASAN_OPTIONS': 'detect_leaks=0:suppressions=/bad',
               'LSAN_OPTIONS': 'exitcode=0:suppressions=/bad',
               'UBSAN_OPTIONS': 'halt_on_error=0', 'TSAN_OPTIONS': 'exitcode=0',
               'ASAN_SYMBOLIZER_PATH': '/bad', 'LLVM_SYMBOLIZER_PATH': '/bad',
               'LD_PRELOAD': '/bad', 'LD_AUDIT': '/bad', 'LD_LIBRARY_PATH': '/bad',
               'CPATH': '/bad', 'SECRET_CREDENTIAL': 'must-not-propagate'}
    env = policy.strict_environment(Path('/compiler/bin/clang'), Path('/tools/llvm-symbolizer'),
                                    library_dirs=[Path('/selected/lib')], base=ambient)
    assert env['HOME'] == ambient['HOME'] and env['TMPDIR'] == '/tmp'
    assert env['LD_LIBRARY_PATH'] == '/selected/lib'
    assert env['LLVM_SYMBOLIZER_PATH'] == env['ASAN_SYMBOLIZER_PATH'] == '/tools/llvm-symbolizer'
    assert '/untrusted' not in env['PATH']
    assert not any(name in env for name in ('LD_PRELOAD', 'LD_AUDIT', 'CPATH', 'SECRET_CREDENTIAL'))
    for key, value in policy.OPTIONS.items():
        assert env[key] == value and 'suppressions=' not in value


@pytest.mark.parametrize('detector', list(policy.DETECTORS))
def test_detection_requires_its_class_site_and_exact_failure_exit(detector):
    source = Path('/controlled') / (detector+'.cpp')
    marker, site, code, _ = policy.DETECTORS[detector]
    diagnostic = f'{marker}\n{site} at {source}:7:3\n'
    policy.validate_detection(detector, source, code, diagnostic, bad=True)
    policy.validate_detection(detector, source, 0, '', bad=False)
    for bad_code, bad_diagnostic in [(0, diagnostic), (-11, diagnostic), (124, diagnostic),
            (127, 'loader error'), (1, 'Assertion failed'), (code, 'unrelated finding'),
            (code, diagnostic.replace(site, 'other_function')),
            (code, diagnostic.replace(str(source), '/other/source.cpp'))]:
        with pytest.raises(ValueError):
            policy.validate_detection(detector, source, bad_code, bad_diagnostic, bad=True)
    with pytest.raises(ValueError):
        policy.validate_detection(detector, source, 0, diagnostic, bad=False)
    with pytest.raises(ValueError):
        policy.require_clean_diagnostic(diagnostic)


@pytest.fixture
def unexecuted_policy(tmp_path, monkeypatch):
    compiler = tmp_path / 'compiler'; compiler.write_bytes(b'compiler')
    (tmp_path / 'ld.lld').write_bytes(b'linker')
    symbolizer = tmp_path / 'symbolizer'; symbolizer.write_bytes(b'symbolizer'); symbolizer.chmod(0o700)
    resource = tmp_path / 'resources'; (resource / 'include').mkdir(parents=True)
    (resource / 'include/stddef.h').write_bytes(b'header')
    runtime = resource / 'libclang_rt.asan.a'; runtime.write_bytes(b'runtime')
    monkeypatch.setattr(policy.subprocess, 'check_output', lambda *args, **kwargs: 'x86_64-unknown-linux-gnu\n')
    return policy.SanitizerQualification(compiler, resource, tmp_path / 'output', 'address', symbolizer=symbolizer)


def test_configuration_and_runtime_mutation_invalidate_controls(unexecuted_policy):
    gate = unexecuted_policy
    with pytest.raises(ValueError, match='no detector controls'):
        gate.finalize()
    gate.environment['ASAN_OPTIONS'] = 'detect_leaks=0'
    with pytest.raises(ValueError, match='configuration changed'):
        gate.ensure_controls('O2')
    gate.environment = dict(gate._expected_environment)
    Path(next(path for path in gate.inputs if path.endswith('.a'))).write_bytes(b'changed')
    with pytest.raises(ValueError, match='inputs changed'):
        gate.ensure_controls('O2')


@pytest.mark.skipif(not NATIVE_READY, reason='requires native Clang and explicit sanitizer resources/symbolizer')
@pytest.mark.parametrize('sanitizer', ['address,undefined', 'thread'])
@pytest.mark.parametrize('optimization', ['O0', 'O2'])
def test_real_detectors_are_paired_and_cached_only_for_unchanged_inputs(tmp_path, sanitizer, optimization):
    gate = policy.SanitizerQualification(COMPILER, RESOURCE, tmp_path, sanitizer)
    receipt = gate.ensure_controls(optimization)
    commands = (Path(receipt['path']).read_bytes())
    assert gate.ensure_controls(optimization) == receipt
    assert Path(receipt['path']).read_bytes() == commands
    assert gate.finalize()['controls'][optimization] == receipt
    Path(receipt['path']).write_bytes(commands + b' ')
    with pytest.raises(ValueError, match='receipt changed'):
        gate.ensure_controls(optimization)
    with pytest.raises(ValueError, match='receipt changed'):
        gate.finalize()


@pytest.mark.skipif(not NATIVE_READY, reason='requires native detector execution')
@pytest.mark.parametrize('mutant', ['missing-instrumentation', 'leaks-disabled', 'recoverable-ub',
                                   'race-zero-exit', 'suppressed-race', 'wrong-resource'])
def test_weakened_detector_routes_cannot_publish_pass(tmp_path, monkeypatch, mutant):
    original_environment, original_run = policy.strict_environment, subprocess.run
    sanitizer = 'thread' if mutant in ('race-zero-exit', 'suppressed-race') else 'address,undefined'
    suppression = tmp_path / 'unapproved.supp'
    suppression.write_text('race:objc3_tsan_detector\n')

    def environment(*args, **kwargs):
        env = original_environment(*args, **kwargs)
        if mutant == 'leaks-disabled':
            env['ASAN_OPTIONS'] = env['ASAN_OPTIONS'].replace('detect_leaks=1', 'detect_leaks=0')
            env['LSAN_OPTIONS'] = env['LSAN_OPTIONS'].replace('detect_leaks=1', 'detect_leaks=0')
        if mutant == 'recoverable-ub':
            env['UBSAN_OPTIONS'] = env['UBSAN_OPTIONS'].replace('halt_on_error=1', 'halt_on_error=0')
        if mutant == 'race-zero-exit':
            env['TSAN_OPTIONS'] = 'halt_on_error=0:exitcode=0:symbolize=1'
        if mutant == 'suppressed-race':
            env['TSAN_OPTIONS'] += ':suppressions='+str(suppression)
        return env

    def run(argv, **kwargs):
        if str(argv[0]) == str(COMPILER.resolve()):
            if mutant == 'missing-instrumentation':
                argv = [arg for arg in argv if not str(arg).startswith('-fsanitize=')]
            if mutant == 'recoverable-ub':
                argv = [arg for arg in argv if arg != '-fno-sanitize-recover=undefined']
        return original_run(argv, **kwargs)

    monkeypatch.setattr(policy, 'strict_environment', environment)
    monkeypatch.setattr(policy.subprocess, 'run', run)
    resource = RESOURCE
    if mutant == 'wrong-resource':
        resource = tmp_path / 'wrong-resource'; (resource / 'include').mkdir(parents=True)
        (resource / 'include/stddef.h').write_text('/* wrong resource */')
        (resource / 'libclang_rt.asan.a').write_text('not a runtime')
    with pytest.raises(ValueError):
        gate = policy.SanitizerQualification(COMPILER, resource, tmp_path / 'controls',
                                              sanitizer, symbolizer=SYMBOLIZER)
        gate.ensure_controls('O2')
