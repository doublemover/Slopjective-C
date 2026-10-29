from __future__ import annotations

import json
from pathlib import Path
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import build
from scripts.objc3c_toolchain.core import ToolchainError


@pytest.fixture
def runner(tmp_path, monkeypatch):
    source = tmp_path / 'out/toolchains/objc3-development/src'
    source.mkdir(parents=True)
    directory = tmp_path / 'build'
    def plan(repo, *, phase, **kwargs):
        return {'paths': {'build': str(directory)}, 'identity': phase + '-identity',
                'source_build_id': 'qualified-source', 'source_tree': 'tree',
                'effective_configuration': {'LLVM_ENABLE_ASSERTIONS': 'ON'},
                'build_config_digest': 'config', 'host_environment': {'compiler': 'original'},
                'command': [phase], 'follow_up_commands': [[phase, 'follow-up']],
                'command_environment': {'QUALIFIED_BUILD': 'yes'}}
    monkeypatch.setattr(build, 'build_plans', lambda repo, **kwargs: {phase: plan(repo, phase=phase) for phase in ('configure', 'build', 'test', 'stage')})
    directory.mkdir()
    (directory / "CMakeCache.txt").write_text("LLVM_ENABLE_ASSERTIONS:BOOL=ON\n")
    calls = []
    def run(command, **kwargs):
        calls.append((command, kwargs['env']))
        return SimpleNamespace(returncode=0)
    monkeypatch.setattr(build.subprocess, 'run', run)
    return tmp_path, directory, calls


def test_build_runs_all_plans_with_isolated_logs_and_environment(runner):
    repo, directory, calls = runner
    report = build.build_foundation(repo, host='linux-x86_64', stage=True)
    assert report['status'] == 'passed'
    assert [call[0][0] for call in calls] == ['configure'] * 2 + ['build'] * 2 + ['test'] * 2 + ['stage'] * 2
    assert all(env['QUALIFIED_BUILD'] == 'yes' and env['CCACHE_COMPILERCHECK'] == 'content' for _, env in calls)
    assert len({phase['log'] for phase in report['phases']}) == 8
    assert json.loads((directory / 'foundation-build-report.json').read_text()) == report


def test_failed_phase_stops_before_tests_and_records_failure(runner, monkeypatch):
    repo, directory, calls = runner
    def run(command, **kwargs):
        calls.append((command, kwargs['env']))
        return SimpleNamespace(returncode=7 if command[0] == 'build' else 0)
    monkeypatch.setattr(build.subprocess, 'run', run)
    with pytest.raises(ToolchainError, match='build exited 7'):
        build.build_foundation(repo, stage=True)
    report = json.loads((directory / 'foundation-build-report.json').read_text())
    assert report['status'] == 'failed'
    assert report['phases'][-1]['exit_code'] == 7
    assert all(command[0] not in ('test', 'stage') for command, _ in calls)


@pytest.mark.parametrize("field", ["source_tree", "source_build_id", "build_config_digest", "host_environment"])
def test_source_or_compiler_change_during_build_invalidates_report(runner, monkeypatch, field):
    repo, directory, _ = runner
    original = build.build_plans
    calls = 0
    def plans(*args, **kwargs):
        nonlocal calls
        calls += 1
        result = original(*args, **kwargs)
        if calls == 2:
            result['configure'][field] = 'changed'
        return result
    monkeypatch.setattr(build, 'build_plans', plans)
    with pytest.raises(ToolchainError, match=field + ' changed'):
        build.build_foundation(repo)
    assert json.loads((directory / 'foundation-build-report.json').read_text())['status'] == 'failed'


def test_foundation_lock_preserves_an_active_build_and_releases_after_failure(tmp_path):
    with pytest.raises(RuntimeError, match='build failure'):
        with build.foundation_lock(tmp_path):
            with pytest.raises(ToolchainError, match='already holds'):
                with build.foundation_lock(tmp_path):
                    pytest.fail('concurrent builder acquired the same output directory')
            raise RuntimeError('build failure')
    with build.foundation_lock(tmp_path):
        pass


@pytest.mark.parametrize('value', ['OFF', '', 'on'])
def test_effective_cache_must_match_intended_configuration(tmp_path, value):
    cache = tmp_path / 'CMakeCache.txt'
    cache.write_text('LLVM_ENABLE_ASSERTIONS:BOOL=' + value + '\n')
    with pytest.raises(ToolchainError, match='effective configuration mismatch.*LLVM_ENABLE_ASSERTIONS'):
        build.verify_effective_configuration({'paths': {'build': str(tmp_path)},
            'effective_configuration': {'LLVM_ENABLE_ASSERTIONS': 'ON'}})


@pytest.mark.parametrize('outside', [False, True])
def test_cmake_executed_include_must_belong_to_configuration_inventory(tmp_path, outside):
    import subprocess
    root = tmp_path / 'repo'
    directory = root / 'toolchains/llvm/cmake'
    directory.mkdir(parents=True)
    primary = directory / 'ObjC3LLVMCache.cmake'
    fragment = (root if outside else directory) / 'profile.cmake'
    fragment.write_text('set(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "" FORCE)\n')
    primary.write_text('include("' + str(fragment) + '")\n')
    trace = tmp_path / 'trace.jsonl'
    subprocess.run(['cmake', '--trace-expand', '--trace-format=json-v1',
                    '--trace-redirect='+str(trace), '--trace-source='+str(primary),
                    '--trace-source='+str(fragment), '-P', str(primary)],
                   check=True, capture_output=True)
    plan = {'configuration_root': str(root), 'configuration_trace': str(trace),
            'configuration_inputs': {'toolchains/llvm/cmake/ObjC3LLVMCache.cmake': 'hash'}}
    if not outside:
        plan['configuration_inputs']['toolchains/llvm/cmake/profile.cmake'] = 'hash'
        assert len(build.verify_configuration_trace(plan)['executed_inputs']) == 2
    else:
        with pytest.raises(ToolchainError, match='outside its configuration inventory'):
            build.verify_configuration_trace(plan)
