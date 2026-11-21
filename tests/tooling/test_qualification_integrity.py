"""Regression controls for qualification gates, including optimized Python."""
import ast
import json
import os
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain.diagnostic_termination import require_diagnostic_exit
from scripts.objc3c_toolchain.qualify_source_graph import require_private_visibility_exit

ROOT = Path(__file__).resolve().parents[2]
OWNERS = ROOT / 'scripts/objc3c_toolchain'


@pytest.mark.parametrize('optimization', ['', '-O', '-OO'])
@pytest.mark.parametrize('checker', ['check_factory_metadata', 'check_cpp_metadata'])
def test_missing_semantic_contract_rejected_by_optimized_python(optimization, checker):
    code = ("from scripts.objc3c_toolchain.qualify_native_async_packages import " + checker +
            "\n" + checker + "({'required_semantics': [], 'declarations': []})")
    env = {**os.environ, 'PYTHONPATH': str(ROOT)}
    result = subprocess.run([sys.executable, *([optimization] if optimization else []), '-c', code],
                            env=env, cwd=ROOT, capture_output=True, text=True, timeout=30)
    assert result.returncode == 1
    assert 'ValueError' in result.stderr
    assert ('required semantic check failed' if checker == 'check_factory_metadata' else
            'native async interface erased the callable effect') in result.stderr


def test_qualification_gates_cannot_introduce_optimization_removable_checks():
    installed = ROOT / 'tests/clang_foundation/installed_sdk'
    for path in [*OWNERS.glob('*.py'), *installed.glob('qualify*.py')]:
        assert not any(isinstance(node, ast.Assert) for node in ast.walk(ast.parse(path.read_text()))), path


def installed_guards():
    cases = []
    for path in sorted((ROOT / 'tests/clang_foundation/installed_sdk').glob('qualify*.py')):
        for node in ast.walk(ast.parse(path.read_text())):
            if (isinstance(node, ast.If) and len(node.body) == 1 and
                    isinstance(node.body[0], ast.Raise) and
                    'installed qualification contract failed:' in ast.unparse(node.body[0])):
                cases.append(pytest.param(path, node, id=path.stem+'-'+str(node.lineno)))
    return cases


@pytest.mark.parametrize('optimization', [0, 1, 2])
@pytest.mark.parametrize('path,guard', installed_guards())
def test_every_installed_contract_rejects_corrupt_evidence_under_python_optimization(tmp_path, path, guard, optimization):
    (tmp_path / 'control.log').write_text('unrelated diagnostic\n')
    rows = [{'semantic_fingerprint': str(index), 'runtime_abi': 'wrong-runtime',
             'identity': {'semantic_fingerprint': str(index)},
             'language': 'unselected', 'standard': 'unselected'} for index in range(3)]
    namespace = {'report': {'executions': [], 'forced_failures': [], 'rejections': [],
                           'commands': [], 'interfaces': rows},
                 'identity': {'language_profile': 'wrong-profile'}, 'sidecar': {'required_semantics': []},
                 'output': tmp_path, 'name': 'control', 'target': Path(sys.executable),
                 'language': 'c++', 'standard': 'c++20'}
    code = compile(ast.Module(body=[guard], type_ignores=[]), str(path), 'exec', optimize=optimization)
    with pytest.raises(ValueError, match='installed qualification contract failed:'):
        exec(code, namespace)


@pytest.mark.parametrize('owner', ['run_linux_compiler_witnesses', 'qualify_stdlib'])
@pytest.mark.parametrize('status,trailer', [(code, '') for code in (0, 1, -11, -6, 139, 134, 70, 2)] + [
    (1, 'clang: error: clang frontend command failed due to signal (use -v to see invocation)'),
    (1, 'clang: error: unable to execute command: Segmentation fault (core dumped)'),
    (1, 'clang: error: clang frontend command failed with exit code 139'),
    (1, 'PLEASE submit a bug report'),
    (1, 'LLVM ERROR: recovered fatal error'),
    (1, "tool: module.cpp:19: Assertion `valid' failed."),
    (1, '==51==ERROR: AddressSanitizer: SEGV'),
])
def test_expected_diagnostic_requires_normal_exit(tmp_path, owner, status, trailer):
    # Execute the actual runner closure with its real log/status checks. A
    # diagnostic followed by a crash must never satisfy an expected rejection.
    tree = ast.parse((OWNERS / (owner + '.py')).read_text())
    qualify = next(n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name == 'qualify')
    run = next(n for n in qualify.body if isinstance(n, ast.FunctionDef) and n.name == 'run')
    def invoke(argv, **kwargs):
        kwargs['stdout'].write('expected diagnostic at fixture.m:16:\n' + trailer + '\n')
        return SimpleNamespace(returncode=status)
    namespace = {'output': tmp_path, 'env': {}, 'commands': [],
                 'subprocess': SimpleNamespace(run=invoke, STDOUT=subprocess.STDOUT),
                 'digest': lambda p: 'hash', 'packages': SimpleNamespace(digest=lambda p: 'hash'),
                 'require_diagnostic_exit': require_diagnostic_exit}
    exec(compile(ast.Module(body=[run], type_ignores=[]), str(OWNERS / owner), 'exec'), namespace)
    def check():
        if owner == 'qualify_stdlib':
            namespace['run']('negative', ['compiler'], tmp_path, failure='expected diagnostic')
        else:
            namespace['run']('negative', ['compiler'], cwd=tmp_path, diagnostics=('expected diagnostic',))
    if status == 1 and not trailer:
        check()
    else:
        with pytest.raises(ValueError):
            check()
    assert namespace['commands'][0]['exit_code'] == status
    assert namespace['commands'][0]['expected_termination'] == 'diagnostic-exit'


@pytest.mark.parametrize('body,expected_code,accepted', [
    ("print('error: fixture.Right is private'); sys.exit(1)", 1, True),
    ("print('error: unrelated failure'); sys.exit(1)", 1, False),
    ("print('error: fixture.Right is private'); sys.exit(0)", 0, False),
    ("print('error: fixture.Right is private'); sys.exit(139)", 139, False),
    ("print('error: fixture.Right is private'); os.kill(os.getpid(), signal.SIGABRT)", -6, False),
    ("print('error: fixture.Right is private'); os.kill(os.getpid(), signal.SIGSEGV)", -11, False),
    ("print('error: fixture.Right is private'); print('LLVM ERROR: fatal error'); sys.exit(1)", 1, False),
])
def test_package_wrapper_checks_real_inner_termination(tmp_path, body, expected_code, accepted):
    log = tmp_path / 'compile.log'
    outcome = tmp_path / 'compile-result.json'
    child = 'import os, resource, signal, sys; resource.setrlimit(resource.RLIMIT_CORE, (0, 0)); ' + body
    wrapper = '''
import json, pathlib, subprocess, sys
with pathlib.Path(sys.argv[1]).open('w') as stream:
    result = subprocess.run([sys.executable, '-u', '-c', sys.argv[3]], stdout=stream, stderr=subprocess.STDOUT)
pathlib.Path(sys.argv[2]).write_text(json.dumps({'exit_code': result.returncode}))
sys.exit('package build failed: compiler invocation failed')
'''
    result = subprocess.run([sys.executable, '-c', wrapper, str(log), str(outcome), child],
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=10)
    assert result.returncode == 1 and json.loads(outcome.read_text())['exit_code'] == expected_code
    if accepted:
        assert require_private_visibility_exit(result, outcome, log, 'fixture.Right') == 1
    else:
        with pytest.raises(ValueError, match='private visibility rejection failed'):
            require_private_visibility_exit(result, outcome, log, 'fixture.Right')


def test_timeout_after_expected_diagnostic_is_not_a_rejection(tmp_path):
    code = "import time; print('error: expected diagnostic', flush=True); time.sleep(5)"
    with pytest.raises(subprocess.TimeoutExpired) as caught:
        subprocess.run([sys.executable, '-c', code], capture_output=True, cwd=tmp_path, timeout=1)
    assert b'expected diagnostic' in caught.value.stdout
