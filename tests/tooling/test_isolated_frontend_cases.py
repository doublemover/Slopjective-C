"""A failing compiler fixture must not hide later independent regressions."""
from pathlib import Path
import shutil
import subprocess
import os

import pytest

ROOT = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('failure,selection', [
    ('assertion',None),('exception',None),('false',None),('none',None),
    ('none','first'),('none','control'),('none','absent'),('assertion','control')])
def test_real_native_case_runner_collects_failures_and_continues(tmp_path, failure, selection):
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
    environment={k:v for k,v in os.environ.items() if k!='OBJC3_TEST_CASE'}
    if selection is not None:environment['OBJC3_TEST_CASE']=selection
    result = subprocess.run([str(binary)], text=True, capture_output=True, timeout=10,env=environment)
    if selection is not None:
        if failure!='none' and selection=='control':
            assert result.returncode==1 and 'FAIL control' in result.stderr
            assert 'PASS last' not in result.stderr
            return
        assert result.returncode==2 and 'PARTIAL diagnostic case selection' in result.stderr
        assert 'PASS last' not in result.stderr
        if selection=='absent':assert '0 independent cases passed' in result.stderr
        else:assert f'PASS {selection}' in result.stderr
        return
    assert result.returncode == (0 if failure == 'none' else 1), result.stderr
    assert 'PASS first' in result.stderr and 'PASS last' in result.stderr
    assert ('PASS control' if failure == 'none' else 'FAIL control') in result.stderr
    assert ('3 independent cases passed; 0 failed' if failure == 'none'
            else '2 independent cases passed; 1 failed') in result.stderr


@pytest.mark.parametrize('failure', [False, True])
def test_native_shards_partition_cases_and_keep_draining_after_failure(tmp_path, failure):
    compiler = shutil.which('g++') or shutil.which('clang++')
    if not compiler:pytest.skip('native C++ compiler unavailable')
    source = tmp_path / 'shards.cpp'
    source.write_text('#include "isolated_test_cases.h"\nint main(int argc, char **argv) {\n  using objc3::clang_foundation::test::IsolatedCases;\n  IsolatedCases cases(std::stoul(argv[1]), 2);\n  cases.Run("first", []{});\n  cases.Run("second", []{});\n  cases.Run("control", [] { return CONTROL; });\n  cases.Run("fourth", []{});\n  cases.Run("last", []{});\n  return cases.Finish();\n}\n'.replace('CONTROL', 'false' if failure else 'true'))
    binary = tmp_path / 'shards'
    subprocess.run([compiler, '-std=c++20', '-I', str(ROOT / 'tests/clang_foundation'),
                    str(source), '-o', str(binary)], check=True, capture_output=True, timeout=30)
    environment={k:v for k,v in os.environ.items() if k!='OBJC3_TEST_CASE'}
    outputs=[]
    for index in range(2):
        result=subprocess.run([str(binary),str(index)],env=environment,text=True,capture_output=True,timeout=10)
        assert result.returncode == (1 if failure and index==0 else 0), result.stderr
        assert f'SHARD {index}/2' in result.stderr
        assert result.stderr.count('CATALOG ')==5
        outputs.append(result.stderr)
    assert 'PASS first' in outputs[0] and 'PASS last' in outputs[0]
    assert 'PASS second' in outputs[1] and 'PASS fourth' in outputs[1]
    assert 'PASS second' not in outputs[0] and 'PASS last' not in outputs[1]
    assert ('FAIL control' if failure else 'PASS control') in outputs[0]
    result=subprocess.run([str(binary),'0'],env={**environment,'OBJC3_TEST_CASE':'first'},
                          text=True,capture_output=True,timeout=10)
    assert result.returncode==2 and 'PARTIAL' in result.stderr
