"""Matching profile trees may rebuild; stale/ordinary/foreign trees cannot enter."""
from copy import deepcopy
import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import compiler_instrumentation as compiler
from scripts.objc3c_toolchain import test_runtime


class Admitted(RuntimeError):
    pass


def fixture(tmp_path, monkeypatch):
    repo = tmp_path / 'repo'; repo.mkdir()
    paths = {name: repo / 'out/foundation' / name for name in ('build', 'stage', 'source')}
    for path in paths.values():
        path.mkdir(parents=True)
    libraries = paths['stage'] / 'lib'
    libraries.mkdir()
    for name in ('libLLVM.so.23.1', 'libclang-cpp.so.23.1'):
        (libraries / name).write_bytes(name.encode())
    bootstrap = repo / 'out/bootstrap'; bootstrap.mkdir()
    plan = {'paths': {key: str(value) for key, value in paths.items()}, 'source_build_id': 'profile-source',
            'build_config_digest': 'profile-config', 'configuration_inputs': {},
            'effective_configuration': {'LLVM_ENABLE_ASSERTIONS': 'ON'},
            'instrumentation': {'boundaries': {}},
            'host_environment': {'bootstrap_root': str(bootstrap), 'clang_sha256': 'matched-tool',
                                 'ccache_path': '/caller/optional-cache'}}
    (paths['build'] / 'CMakeCache.txt').write_text('LLVM_ENABLE_ASSERTIONS:BOOL=ON\n')
    ignorelist = paths['source'] / 'llvm/utils/sanitizers/ubsan_ignorelist.txt'
    ignorelist.parent.mkdir(parents=True)
    ignorelist.write_text('# controlled input\n')
    report = {'status': 'passed', 'source_identity': plan['source_build_id'],
              'phases': [{'phase': phase, 'exit_code': 0} for phase in ('configure', 'build', 'test', 'stage')]}
    (paths['build'] / 'foundation-build-report.json').write_text(json.dumps(report))
    embedding = repo / 'out/embedding'; embedding.mkdir()
    old = deepcopy(plan)
    old['host_environment']['ccache_path'] = None
    (embedding / 'compiler-instrumentation-plan.json').write_text(json.dumps(old))
    monkeypatch.setattr(compiler, 'build_plans', lambda *a, **k: {
        'configure': plan, **{phase: {} for phase in ('build', 'test', 'stage')}})
    def admitted(*a, **k):
        raise Admitted('passed reuse admission; no native execution is mocked as a pass')
    monkeypatch.setattr(test_runtime, 'prepare', admitted)
    return repo, paths, embedding, old


def test_matching_reuse_admits_only_the_optional_cache_path_difference(tmp_path, monkeypatch):
    repo, _, embedding, _ = fixture(tmp_path, monkeypatch)
    with pytest.raises(Admitted):
        compiler.qualify(repo, repo / 'out/new-evidence', embedding_build=embedding)


def test_compiler_command_owner_uses_runner_limits_after_admission(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import semantic_benchmarks

    repo, paths, _, _ = fixture(tmp_path, monkeypatch)
    plan = compiler.build_plans(repo)['configure']
    plan['host_environment'].update(cmake_path='/tools/cmake', ninja_path='/tools/ninja')
    dependency = {'root': str(paths['stage']), 'library': '/owned/libobjc.so',
                  'receipt': '/owned/runtime.json', 'library_sha256': 'library',
                  'receipt_sha256': 'receipt'}
    monkeypatch.setattr(test_runtime, 'prepare', lambda *a, **k: dependency)
    monkeypatch.setattr(compiler, 'SanitizerQualification', lambda *a, **k: SimpleNamespace(
        inputs={dependency['library']: 'library', dependency['receipt']: 'receipt'},
        environment={'PATH': '/tools'}, loader='/tools/loader', ensure_controls=lambda *a: None))
    monkeypatch.setattr(compiler, 'loaded_graph', lambda *a, run, **k:
                        run('runner-limit-probe', ['owned-command']))
    calls = []

    def sample(argv, **kwargs):
        calls.append((argv, kwargs['timeout']))
        raise Admitted('command delegated to runner; no native result claimed')

    monkeypatch.setattr(semantic_benchmarks, 'sample', sample)
    output = repo / 'out/new-evidence'
    with pytest.raises(Admitted):
        compiler.qualify(repo, output)
    assert calls == [(['owned-command'], None)]
    assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'


@pytest.mark.parametrize('fault', ['source', 'config', 'bootstrap', 'ordinary', 'outside',
                                  'overlaps-source', 'overlaps-output', 'failed-foundation'])
def test_incompatible_reuse_is_rejected_before_any_control_or_native_build(tmp_path, monkeypatch, fault):
    repo, paths, embedding, old = fixture(tmp_path, monkeypatch)
    output = repo / 'out/new-evidence'
    stamp = embedding / 'compiler-instrumentation-plan.json'
    if fault == 'source': old['source_build_id'] = 'other-source'
    elif fault == 'config': old['build_config_digest'] = 'ordinary'
    elif fault == 'bootstrap': old['host_environment']['clang_sha256'] = 'other-compiler'
    elif fault == 'ordinary': stamp.unlink()
    elif fault == 'outside': embedding = tmp_path / 'foreign'; embedding.mkdir()
    elif fault == 'overlaps-source': embedding = paths['source']
    elif fault == 'overlaps-output': output = embedding / 'new-evidence'
    else:
        report = paths['build'] / 'foundation-build-report.json'
        payload = json.loads(report.read_text()); payload['status'] = 'failed'
        report.write_text(json.dumps(payload))
    if fault in ('source', 'config', 'bootstrap'):
        stamp.write_text(json.dumps(old))
    with pytest.raises((ValueError, FileNotFoundError)):
        compiler.qualify(repo, output, embedding_build=embedding)
