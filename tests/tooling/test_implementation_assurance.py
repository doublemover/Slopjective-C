"""Exact selection, retained evidence and cleanup controls for implementation analysis."""
import json
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_implementation_assurance import evidence
from scripts.objc3c_implementation_assurance.plan import select, validate
from scripts.objc3c_toolchain.analysis_checkpoint import analysis_argv
from scripts.objc3c_toolchain.implementation_analysis import checker_configuration

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


def test_selection_is_scoped_dependency_aware_and_periodically_complete():
    assert not select(['README.md'], **REV)['required']
    selected = select(['native/objc3c/src/runtime/service.cpp', 'README.md'], **REV)
    assert selected['required'] and selected['analysis_paths'] == [
        'native/objc3c/src/runtime/service.cpp']
    assert selected['selection'] == 'dependency-aware-owned-changes'
    periodic = select(['README.md'], **REV, periodic=True)
    assert periodic['required'] and periodic['analysis_paths'] == []
    assert periodic['selection'] == 'complete-owned-tu-inventory'
    assert select([], **REV)['required'] and select(None, **REV)['required']


@pytest.mark.parametrize('mutation', ['required', 'path', 'digest'])
def test_modified_analysis_plan_is_rejected(mutation):
    plan = select(['native/analysis/CMakeLists.txt'], **REV)
    if mutation == 'required': plan['required'] = False
    elif mutation == 'path': plan['analysis_paths'] = []
    else: plan['sha256'] = '0'*64
    with pytest.raises(ValueError):
        validate(plan)


def passing_bundle(tmp_path, *, periodic=False, partition=None):
    from scripts.objc3c_toolchain.analysis_partition import partition_spec, partition_rows
    tmp_path.mkdir(parents=True, exist_ok=True)
    if periodic and partition is None:
        for index in range(4):
            plan, _ = passing_bundle(tmp_path / ('part-'+str(index)), periodic=True,
                                     partition=partition_spec(index,4))
        return plan, None
    plan = select(['native/objc3c/src/runtime/service.cpp'], **REV, periodic=periodic)
    analysis = tmp_path / 'analysis';analysis.mkdir()
    # Producer paths intentionally do not exist on the receiving machine.
    producer_repo = Path('/producer/checkout')
    producer = producer_repo / 'out/implementation-assurance/results/analysis'
    config = producer / 'checks.json'
    tool = '/producer/tool/objc3-clang-tidy'
    policy = json.loads((Path(__file__).resolve().parents[2] /
                         'toolchains/llvm/implementation-analysis.json').read_text())
    (analysis / 'checks.json').write_text(json.dumps(checker_configuration(producer_repo,
        producer_repo / 'out/toolchains/objc3-development/src', policy)))
    inventory = []
    for index, owner in enumerate(('frontend', 'standalone-runtime', 'maintained-clang')):
        source = (producer_repo / 'native/objc3c/src/runtime/service.cpp' if index < 2 else
                  producer_repo / 'out/toolchains/objc3-development/src/clang/lib/Sema/Sema.cpp')
        inventory.append({'owner': owner, 'identity': str(index+1)*64, 'file': str(source),
            'directory': '/producer/build/'+owner, 'output': '/producer/build/'+owner+'/source.o',
            'analysis_arguments': ['clang++', '-DREAL_FLAG=1', '-c', str(source)],
            'dependencies': [str(source)], 'selected': periodic or index < 2,
            'analysis_timeout_seconds': (policy['maintained_translation_unit_timeout_seconds']
                                         if owner=='maintained-clang' else policy['translation_unit_timeout_seconds']),
            'dependency_origin': 'actual-ninja-compilation'})
    if partition is not None:
        for row in inventory:row['selected'] &= bool(partition_rows([row], partition))
    results = [{'identity': row['identity'], 'status': 'PASS', 'diagnostics': [],
                'execution_reused': False, 'timeout_seconds': row['analysis_timeout_seconds']}
               for row in inventory if row['selected']]
    controls = [
        {'name': 'control-corrected', 'status': 'PASS', 'exit_code': 0,
         'expected_exit_code': 0, 'timed_out': False},
        {'name': 'control-defective', 'status': 'PASS', 'exit_code': 1,
         'expected_exit_code': 1, 'timed_out': False},
    ]
    commands = list(controls)
    for row in inventory:
        if not row['selected']:
            continue
        name = row['owner']+'-'+row['identity'][:16]
        directory = analysis / name;directory.mkdir()
        (directory / 'compile_commands.json').write_text(json.dumps([{
            'file': row['file'], 'directory': row['directory'], 'arguments': row['analysis_arguments']}]))
        commands.append({'name': name, 'status': 'PASS', 'exit_code': 0,
            'expected_exit_code': 0, 'timed_out': False,
            'argv': analysis_argv(tool, producer / name, row['file'], config, '/producer/resources')})
    logs = analysis / 'logs';logs.mkdir()
    for command in commands:
        name = command['name']
        path = logs / (name+'.log')
        path.write_text('/producer/control.cpp:2:1: error: null access '
                        '[clang-analyzer-core.NullDereference,-warnings-as-errors]\n'
                        if name == 'control-defective' else '')
        command.update(log=str(producer / 'logs' / path.name), log_sha256=evidence.digest(path))
    inner = {'schema': 'objc3c.implementation-analysis.v1', 'status': 'PASS',
        'subject': 'native-and-maintained-clang-implementation', 'policy': policy,
        'source_build_id': 'fork-build', 'build_config_digest': 'config',
        'tool': {'path': tool, 'sha256': 'a'*64}, 'tool_graph': {'libraries': {'libclang.so': 'b'*64}},
        'tool_objects': [{'object': 'tool.o'}],
        'inputs': {str(config): evidence.digest(analysis / 'checks.json'),
                   **{path:'c'*64 for row in inventory for path in row['dependencies']}},
        'configuration': str(config), 'repository_output': str(producer_repo / 'out'),
        'resource_directory': '/producer/resources',
        'changed_paths': [str(producer_repo / path) for path in plan['analysis_paths']],
        'results': results, 'inventory': inventory, 'commands': commands,
        'exclusions': [{'owner': row['owner'], 'selected_variants': 1,
                        'analyzed_variants': int(row['selected'])} for row in inventory],
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'fresh_native_executions': len(results), 'verified_checkpoint_actions': 0}
    if partition is not None:inner['partition']=partition
    (analysis / 'qualification.json').write_text(json.dumps(inner))
    report = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
        'plan_sha256': plan['sha256'], 'analysis_paths': plan['analysis_paths'],
        'temporary_builds_reclaimed': True, 'commands': [], 'evidence_root': str(producer.parent),
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'source_build_id': 'fork-build', 'build_config_digest': 'config'}
    if partition is not None:report['partition']=partition
    build_logs = tmp_path / 'build-logs';build_logs.mkdir()
    for name in ('frontend-configure', 'frontend-build', 'runtime-configure', 'runtime-build'):
        path = build_logs / (name+'.log');path.write_text('successful build')
        report['commands'].append({'name': name, 'exit_code': 0, 'timed_out': False,
            'log': str(producer.parent / 'build-logs' / path.name), 'log_sha256': evidence.digest(path),
            'argv': ['cmake', '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan',
                     '-DOBJC3C_LINK_CLANG_CPP=ON']})
    foundation = tmp_path / 'foundation';foundation.mkdir()
    (foundation / 'foundation-0.json').write_text(json.dumps({'tests': [{'name': 'compiler', 'code': 'PASS'}]}))
    (foundation / 'foundation.json').write_text(json.dumps({'status': 'passed',
        'source_identity': 'fork-build', 'phases': [{'phase': phase, 'exit_code': 0,
            **({'test_results': '/producer/foundation-0.json'} if phase == 'test' else {})}
            for phase in ('configure', 'build', 'test', 'stage')]}))
    report['artifacts'] = [evidence.artifact(tmp_path, path)
                           for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    return plan, report


@pytest.mark.parametrize('periodic', [False, True])
def test_complete_analysis_bundle_is_admitted_after_relocation(tmp_path, periodic):
    plan, _ = passing_bundle(tmp_path, periodic=periodic)
    result = evidence.aggregate(plan, tmp_path,
        outcomes={'plan': 'success', 'analysis': 'success'}, tested=REV['tested'])
    assert result['status'] == 'PASS' and result['analyzed_variants'] == (3 if periodic else 2)


@pytest.mark.parametrize('fault', ['cleanup', 'artifact', 'control', 'finding', 'outcome'])
def test_incomplete_analysis_bundle_fails_closed(tmp_path, fault):
    plan, report = passing_bundle(tmp_path)
    inner_path = tmp_path / 'analysis/qualification.json'
    inner = json.loads(inner_path.read_text())
    outcomes = {'plan': 'success', 'analysis': 'success'}
    if fault == 'cleanup': report['temporary_builds_reclaimed'] = False
    elif fault == 'artifact': report['artifacts'][0]['sha256'] = '0'*64
    elif fault == 'control': inner['commands'][1]['exit_code'] = 0
    elif fault == 'finding': inner['results'][0]['diagnostics'] = [{'check': 'blocker'}]
    else: outcomes['analysis'] = 'failure'
    inner_path.write_text(json.dumps(inner))
    if fault in ('control', 'finding'):
        report['artifacts'] = [evidence.artifact(tmp_path, path)
                               for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    with pytest.raises(ValueError):
        evidence.aggregate(plan, tmp_path, outcomes=outcomes, tested=REV['tested'])


def test_workflow_has_periodic_full_gate_and_automatic_cleanup():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/implementation-analysis.yml').read_text()
    assert 'schedule:' in workflow and 'workflow_dispatch:' in workflow
    assert 'workflow_call:' in workflow
    assert 'Linux implementation analysis gate' in workflow
    assert '--retain-builds' not in workflow
    release = (root / '.github/workflows/platform-host-evidence.yml').read_text()
    assert 'uses: ./.github/workflows/implementation-analysis.yml' in release
    assert 'needs: [build, implementation-analysis]' in release
    assert 'uses: ./.github/workflows/native-assurance.yml' in release
    assert 'build-sdk --fetch' not in release
    native = (root / '.github/workflows/native-assurance.yml').read_text()
    assert 'workflow_call:' in native
    assert "if: success() && matrix.shard == 'core'" in native
    assert 'name: objc3c-linux-sdk' in native and 'name: objc3c-linux-sdk' in release
    assert 'out/toolchains/sdk-cache/*.tar.zst' in native
    runner = (root / 'scripts/objc3c_implementation_assurance/runner.py').read_text()
    assert "PROFILE = 'compiler-asan-ubsan'" in runner
    assert 'with foundation_lock(repo)' in runner


@pytest.mark.parametrize('fault', ['missing-action', 'extra-action', 'wrong-selection', 'wrong-argv',
    'wrong-database', 'hidden-finding', 'missing-log', 'wrong-log-hash', 'reused-action',
    'missing-control-diagnostic', 'missing-build', 'wrong-profile', 'wrong-foundation', 'symlink',
    'wrong-resource-bound'])
def test_rehashed_outer_bundle_cannot_hide_missing_native_proof(tmp_path, fault):
    plan, report = passing_bundle(tmp_path)
    path = tmp_path / 'analysis/qualification.json'
    inner = json.loads(path.read_text())
    if fault == 'missing-action':
        inner['results'].pop();inner['analyzed_variants'] -= 1;inner['fresh_native_executions'] -= 1
        inner['exclusions'][1]['analyzed_variants'] = 0
    elif fault == 'extra-action':
        inner['results'].append({'identity': inner['inventory'][2]['identity'], 'status': 'PASS'})
        inner['analyzed_variants'] += 1;inner['fresh_native_executions'] += 1
        inner['exclusions'][2]['analyzed_variants'] = 1
    elif fault == 'wrong-selection': inner['inventory'][1]['selected'] = False
    elif fault == 'wrong-argv': inner['commands'][-1]['argv'].append('--checks=-*')
    elif fault == 'wrong-database':
        next((tmp_path / 'analysis').glob('frontend-*/compile_commands.json')).write_text('[]')
    elif fault == 'wrong-resource-bound':
        inner['inventory'][0]['analysis_timeout_seconds'] = 3600
        inner['results'][0]['timeout_seconds'] = 3600
    elif fault == 'hidden-finding':
        command = inner['commands'][-1]
        log = tmp_path / 'analysis/logs' / Path(command['log']).name
        log.write_text('/source/service.cpp:9:2: warning: moved value [bugprone-use-after-move]\n')
        command['log_sha256'] = evidence.digest(log)
    elif fault == 'missing-log':
        (tmp_path / 'analysis/logs' / Path(inner['commands'][-1]['log']).name).unlink()
    elif fault == 'wrong-log-hash': inner['commands'][-1]['log_sha256'] = '0'*64
    elif fault == 'reused-action':
        inner['fresh_native_executions'] -= 1;inner['verified_checkpoint_actions'] += 1
    elif fault == 'missing-control-diagnostic':
        log = tmp_path / 'analysis/logs/control-defective.log';log.write_text('some unrelated failure')
        inner['commands'][1]['log_sha256'] = evidence.digest(log)
    elif fault == 'missing-build': report['commands'].pop()
    elif fault == 'wrong-profile': report['commands'][0]['argv'] = ['cmake', '-S', 'frontend']
    elif fault == 'wrong-foundation':
        foundation = tmp_path / 'foundation/foundation.json'
        value = json.loads(foundation.read_text());value['source_identity'] = 'wrong-fork'
        foundation.write_text(json.dumps(value))
    else:
        log = tmp_path / 'analysis/logs/control-defective.log'
        alternate = tmp_path / 'original-control.txt';alternate.write_bytes(log.read_bytes())
        log.unlink();log.symlink_to(alternate)
    path.write_text(json.dumps(inner))
    report['analyzed_variants'] = inner['analyzed_variants']
    if fault != 'symlink':
        report['artifacts'] = [evidence.artifact(tmp_path, value)
                               for value in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    with pytest.raises(ValueError):
        evidence.aggregate(plan, tmp_path, outcomes={'plan': 'success', 'analysis': 'success'},
                           tested=REV['tested'])


@pytest.fixture
def lifecycle(tmp_path, monkeypatch):
    from scripts.objc3c_implementation_assurance import runner
    from scripts.objc3c_implementation_assurance.plan import from_checkout
    from scripts.objc3c_toolchain.build import foundation_lock
    from scripts.objc3c_toolchain.core import ToolchainError
    repo = tmp_path / 'repo';repo.mkdir()
    def git(*args):
        return subprocess.check_output(['git', '-C', str(repo), *args], text=True).strip()
    git('init', '-q');git('config', 'user.name', 'Analysis Test')
    git('config', 'user.email', 'analysis@example.invalid')
    (repo / '.gitignore').write_text('/out/\n')
    source = repo / 'native.cpp';source.write_text('int original;\n')
    git('add', '.');git('commit', '-qm', 'initial implementation')
    sha = git('rev-parse', 'HEAD')
    plan = from_checkout(repo, head=sha, base=sha, tested=sha)
    base = repo / 'out/toolchains/objc3-development'
    bootstrap = tmp_path / 'bootstrap';bootstrap.mkdir()
    build = base / ('build-linux-x86_64-'+runner.PROFILE)
    stage = base / ('stage-linux-x86_64-'+runner.PROFILE)
    def foundation(*args, **kwargs):
        with pytest.raises(ToolchainError, match='already holds'):
            with foundation_lock(repo):
                pytest.fail('runner released its shared build lock')
        for root in (base / 'src', build, stage): root.mkdir(parents=True)
        log = build / 'compile.log';log.write_text('native build diagnostic')
        (build / 'foundation-build-report.json').write_text(json.dumps({
            'status': 'passed', 'phases': [{'phase': 'build', 'log': str(log)}]}))
        return {'status': 'passed'}
    configuration = {'paths': {'build': str(build), 'stage': str(stage), 'source': str(base / 'src')},
        'host_environment': {'bootstrap_root': str(bootstrap), 'cmake_path': '/tools/cmake',
            'ninja_path': '/tools/ninja', 'resource_limits': {'compile_jobs': 2}}}
    monkeypatch.setattr(runner, '_build_foundation', foundation)
    monkeypatch.setattr(runner, 'build_plans', lambda *a, **kw: {'configure': configuration})
    def command(argv, *, log, **kwargs):
        if '-B' in argv: Path(argv[argv.index('-B')+1]).mkdir(parents=True, exist_ok=True)
        log.write_text('compiled successfully')
        return {'argv': list(map(str, argv)), 'exit_code': 0, 'timed_out': False,
                'log': str(log), 'log_sha256': evidence.digest(log)}
    monkeypatch.setattr(runner, 'execute_command', command)
    monkeypatch.setattr(runner, 'qualify', lambda *a, **kw: {
        'status': 'PASS', 'inventory_variants': 1, 'source_build_id': 'fork', 'build_config_digest': 'config'})
    monkeypatch.setattr(runner, 'analysis_results', lambda output: [{'status': 'PASS'}])
    return runner, repo, plan, base, source


def test_analysis_runner_holds_the_lock_and_cleans_only_its_created_roots(lifecycle):
    runner, repo, plan, base, _ = lifecycle
    output = repo / 'out/evidence'
    result = runner.run(repo, plan, output)
    assert result['status'] == 'PASS' and result['temporary_builds_reclaimed']
    assert (output / 'foundation/phase-0.log').read_text() == 'native build diagnostic'
    assert not (base / 'src').exists() and not (repo / 'out/implementation-assurance-builds').exists()
    assert '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan' in result['commands'][0]['argv']


@pytest.mark.parametrize('fault', ['busy', 'preexisting', 'linked-parent', 'linked-output', 'dirty'])
def test_analysis_runner_preserves_unowned_work_before_start(lifecycle, tmp_path, fault):
    runner, repo, plan, base, source = lifecycle
    output = repo / 'out/evidence'
    protected = base / 'src';protected.mkdir(parents=True)
    marker = protected / 'do-not-delete';marker.write_text('existing development work')
    if fault == 'busy':
        from scripts.objc3c_toolchain.build import foundation_lock
        from scripts.objc3c_toolchain.core import ToolchainError
        with foundation_lock(repo), pytest.raises(ToolchainError, match='already holds'):
            runner.run(repo, plan, output)
    else:
        if fault == 'linked-parent':
            (repo / 'out/implementation-assurance-builds').symlink_to(tmp_path, target_is_directory=True)
        elif fault == 'linked-output': output.symlink_to(tmp_path, target_is_directory=True)
        elif fault == 'dirty': source.write_text('int changed;\n')
        with pytest.raises(ValueError): runner.run(repo, plan, output)
    assert marker.read_text() == 'existing development work'
    assert not (output / 'result.json').exists()


@pytest.mark.parametrize('fault', ['foundation-failure', 'analysis-failure', 'source-mutated', 'diagnostic-failure'])
def test_partial_attempt_always_retains_failure_and_reclaims_created_builds(lifecycle, monkeypatch, fault):
    runner, repo, plan, base, source = lifecycle
    original = runner._build_foundation
    if fault in ('foundation-failure', 'diagnostic-failure'):
        def foundation(*args, **kwargs):
            original(*args, **kwargs)
            if fault == 'diagnostic-failure':
                path = base / ('build-linux-x86_64-'+runner.PROFILE) / 'foundation-build-report.json'
                path.write_text('malformed diagnostic receipt')
            raise ValueError('forced build failure')
        monkeypatch.setattr(runner, '_build_foundation', foundation)
    else:
        def qualify(*args, **kwargs):
            if fault == 'analysis-failure': raise ValueError('forced analysis failure')
            source.write_text('int mutated_while_analyzing;\n')
            return {'status': 'PASS', 'inventory_variants': 1,
                    'source_build_id': 'fork', 'build_config_digest': 'config'}
        monkeypatch.setattr(runner, 'qualify', qualify)
    output = repo / 'out/evidence'
    with pytest.raises(ValueError): runner.run(repo, plan, output)
    report = json.loads((output / 'result.json').read_text())
    assert report['status'] == 'FAIL' and report['temporary_builds_reclaimed']
    assert not (base / 'src').exists() and not (repo / 'out/implementation-assurance-builds').exists()
    if fault == 'diagnostic-failure': assert 'diagnostic_error' in report
