"""A failing compiler fixture must not hide later independent regressions."""
from pathlib import Path
import shutil
import subprocess

import pytest

ROOT = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('failure', ['assertion', 'exception', 'false', 'none'])
def test_real_native_case_runner_collects_failures_and_continues(tmp_path, failure):
    compiler = shutil.which('g++') or shutil.which('clang++')
    if not compiler:
        pytest.skip('native C++ compiler unavailable')
    source = tmp_path / 'control.cpp'
    actions = {'assertion': 'assert(false);', 'exception': 'throw std::runtime_error("control");',
               'false': 'return false;', 'none': 'return true;'}
    source.write_text('''#include "isolated_test_cases.h"
#include <cassert>
#include <sys/resource.h>
int main() {
  rlimit core_limit{0,0};setrlimit(RLIMIT_CORE,&core_limit);
  objc3::clang_foundation::test::IsolatedCases cases;
  cases.Run("first", []{});
  cases.Run("control", [] {''' + actions[failure] + '''});
  cases.Run("last", []{});
  return cases.Finish();
}
''')
    binary = tmp_path / 'control'
    subprocess.run([compiler, '-std=c++20', '-UNDEBUG', '-I', str(ROOT / 'tests/clang_foundation'),
                    str(source), '-o', str(binary)], check=True, capture_output=True, timeout=30)
    result = subprocess.run([str(binary)], text=True, capture_output=True, timeout=10)
    assert result.returncode == (0 if failure == 'none' else 1), result.stderr
    assert 'PASS first' in result.stderr and 'PASS last' in result.stderr
    assert ('PASS control' if failure == 'none' else 'FAIL control') in result.stderr
    assert ('3 independent cases passed; 0 failed' if failure == 'none'
            else '2 independent cases passed; 1 failed') in result.stderr
