"""Check effective native test flags in all supported CMake configurations."""
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess

import pytest

ROOT = Path(__file__).resolve().parents[2]
STAGE = ROOT / 'out/toolchains/objc3-development/stage-linux-x86_64'
pytestmark = pytest.mark.skipif(not (STAGE / 'bin/clang').is_file(),
                                reason='requires a qualified native foundation')


@pytest.mark.parametrize('configuration', ['Debug', 'Release', 'RelWithDebInfo'])
def test_every_registered_test_retains_assertions(tmp_path, configuration):
    build = tmp_path / configuration
    subprocess.run(['cmake', '-S', str(ROOT / 'native/objc3c'), '-B', str(build), '-G', 'Ninja',
                    '-DCMAKE_BUILD_TYPE='+configuration,
                    '-DOBJC3C_CLANG_FOUNDATION_ROOT='+str(STAGE),
                    '-DCMAKE_CXX_COMPILER='+str(STAGE / 'bin/clang++')],
                   check=True, capture_output=True, text=True, timeout=120)
    manifest = json.loads((build / 'frontend-test-targets.json').read_text())
    entries = json.loads((build / 'compile_commands.json').read_text())
    covered = set()
    for entry in entries:
        matches = [row for row in manifest['tests'] if '/'+row['target']+'.dir/' in entry['command']]
        if not matches:
            continue
        args = [arg for arg in shlex.split(entry['command']) if not arg.startswith('@')]
        # Probe final macro state through the actual compiler, forced includes
        # and ordered flags, using a tiny independent TU to avoid generated
        # product-header dependencies in an otherwise unbuilt configuration.
        source = args.index('-c')
        args[source:source+2] = []
        output = args.index('-o')
        args[output:output+2] = []
        result = subprocess.run([*args, '-E', '-dM', '-x', 'c++', '-'], input='#include <cassert>\n',
                                check=True, capture_output=True, text=True, timeout=30)
        assert '#define NDEBUG' not in result.stdout
        assert any('test_assertion_policy.h' in arg for arg in args)
        covered.update(row['target'] for row in matches)
    custom = {'objc3_task_coroutines_test', 'objc3_task_group_await_test', 'objc3_runtime_lifetime_test'}
    assert covered == {row['target'] for row in manifest['tests']} - custom
    for target in custom:
        commands = subprocess.check_output(['ninja', '-C', str(build), '-t', 'commands', target], text=True)
        marker = {'objc3_task_coroutines_test': 'task-coroutines-',
                  'objc3_task_group_await_test': 'task-group-await.o',
                  'objc3_runtime_lifetime_test': 'runtime-lifetime.o'}[target]
        object_commands = [line for line in commands.splitlines() if ' -c ' in line and marker in line]
        assert object_commands
        for command in object_commands:
            args = shlex.split(command)
            assert '-UNDEBUG' in args and '-DNDEBUG' not in args
            assert str(ROOT / 'native/objc3c/tests/test_assertion_policy.h') in args
    subprocess.run(['cmake', '--build', str(build), '--target', 'objc3_assertion_control_test'],
                   check=True, capture_output=True, timeout=60)
    result = subprocess.run(['ctest', '--test-dir', str(build), '--no-tests=error',
                             '--output-on-failure', '-R', '^objc3_assertion_control$'],
                            capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stdout + result.stderr
    # Losing the final -U protection is diagnosed at compilation, independently
    # of the negative CTest control and its WILL_FAIL registration.
    result = subprocess.run([str(STAGE / 'bin/clang++'), '-DNDEBUG', '-include',
                             str(ROOT / 'native/objc3c/tests/test_assertion_policy.h'),
                             '-fsyntax-only', '-x', 'c++', '-'], input='int main() {}',
                            capture_output=True, text=True, timeout=30)
    assert result.returncode == 1
    assert 'Native regression tests require active assertions' in result.stderr
