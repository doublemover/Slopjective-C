"""Regression controls for qualification gates, including optimized Python."""
import ast
import json
import os
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace

import pytest

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
@pytest.mark.parametrize('status', [0, 1, -11, -6, 139, 134, 70, 2])
def test_expected_diagnostic_requires_normal_exit(tmp_path, owner, status):
    # Execute the actual runner closure with its real log/status checks. A
    # diagnostic followed by a crash must never satisfy an expected rejection.
    tree = ast.parse((OWNERS / (owner + '.py')).read_text())
    qualify = next(n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name == 'qualify')
    run = next(n for n in qualify.body if isinstance(n, ast.FunctionDef) and n.name == 'run')
    def invoke(argv, **kwargs):
        kwargs['stdout'].write('expected diagnostic at fixture.m:16:\n')
        return SimpleNamespace(returncode=status)
    namespace = {'output': tmp_path, 'env': {}, 'commands': [],
                 'subprocess': SimpleNamespace(run=invoke, STDOUT=subprocess.STDOUT),
                 'digest': lambda p: 'hash', 'packages': SimpleNamespace(digest=lambda p: 'hash')}
    exec(compile(ast.Module(body=[run], type_ignores=[]), str(OWNERS / owner), 'exec'), namespace)
    def check():
        if owner == 'qualify_stdlib':
            namespace['run']('negative', ['compiler'], tmp_path, failure='expected diagnostic')
        else:
            namespace['run']('negative', ['compiler'], cwd=tmp_path, expected=1)
    if status == 1:
        check()
    else:
        with pytest.raises(ValueError):
            check()
    assert namespace['commands'][0]['exit_code'] == status
