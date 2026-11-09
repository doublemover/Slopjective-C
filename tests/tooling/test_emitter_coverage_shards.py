"""Sharded coverage must retain the whole native emitter case inventory."""
import xml.etree.ElementTree as ET

import pytest

from scripts.objc3c_assurance.emitter_shards import EMITTER_TESTS, SHARDS, emitter_partition, shard_count


@pytest.mark.parametrize('mutation', [None, 'missing', 'duplicate', 'case', 'pass', 'catalogue',
                                      'binary', 'partial', 'failure', 'index'])
def test_shard_completeness_is_checked_from_retained_native_junit(tmp_path, mutation):
    catalogue = [f'TestCase{index}' for index in range(2 * SHARDS + 3)]
    rows = []
    for index, name in enumerate(EMITTER_TESTS):
        names = list(catalogue)
        if index == 0 and mutation == 'catalogue':names[-1] = 'DifferentCase'
        text = ''.join(f'CATALOG {ordinal} {case}\n' for ordinal, case in enumerate(names))
        for case in names[index::SHARDS]:
            if not (index == 0 and mutation == 'case'):text += 'CASE '+case+'\n'
            if not (index == 0 and mutation == 'pass'):text += 'PASS '+case+'\n'
        text += f'SHARD {index if mutation != "index" else 1}/{SHARDS}\n'
        if index == 0 and mutation == 'partial':text += 'PARTIAL diagnostic selection\n'
        suite = ET.Element('testsuite', tests='1', failures='0')
        test = ET.SubElement(suite, 'testcase', name=name)
        ET.SubElement(test, 'system-out').text = text
        if index == 0 and mutation == 'failure':ET.SubElement(test, 'failure')
        path = tmp_path / 'suites' / ('frontend-'+name) / 'tests.xml'
        path.parent.mkdir(parents=True);ET.ElementTree(suite).write(path)
        rows.append({'name': name, 'component': 'frontend', 'status': 'PASS',
                     'binary_sha256': ('b' if index == 0 and mutation == 'binary' else 'a')*64})
    if mutation == 'missing':rows.pop()
    if mutation == 'duplicate':rows.append(rows[0])
    if mutation:
        with pytest.raises(ValueError):emitter_partition(tmp_path, rows)
    else:
        assert emitter_partition(tmp_path, rows)['cases'] == 2 * SHARDS + 3


@pytest.mark.parametrize('truncated', [False, True])
def test_actual_native_shard_output_survives_ctest_junit(tmp_path, truncated):
    import os
    from pathlib import Path
    import shutil
    import subprocess
    from scripts.objc3c_assurance.emitter_shards import CTEST_OUTPUT_LIMIT
    compiler = shutil.which('g++') or shutil.which('clang++')
    ctest = shutil.which('ctest')
    if not compiler or not ctest:pytest.skip('requires a native C++ compiler and CTest')
    source = tmp_path / 'catalogue.cpp';binary = tmp_path / 'catalogue'
    source.write_text('''#include "isolated_test_cases.h"
int main(int argc, char **argv) {
  objc3::clang_foundation::test::IsolatedCases cases(std::stoul(argv[1]), std::stoul(argv[2]));
  for(unsigned i=0;i<std::stoul(argv[3]);++i)cases.Run("TestCase"+std::to_string(i), [] {
    std::cerr << std::string(2048, 'x') << '\\n';
  });
  return cases.Finish();
}
''')
    include = Path(__file__).resolve().parents[2] / 'tests/clang_foundation'
    subprocess.run([compiler,'-std=c++20','-I',str(include),str(source),'-o',str(binary)],
                   capture_output=True,check=True,timeout=30)
    rows=[]
    environment={k:v for k,v in os.environ.items() if k!='OBJC3_TEST_CASE'}
    for index,name in enumerate(EMITTER_TESTS):
        work=tmp_path/'suites'/('frontend-'+name);work.mkdir(parents=True)
        (work/'CTestTestfile.cmake').write_text(f'add_test({name} "{binary}" {index} {SHARDS} {2*SHARDS+3})\n')
        command=[ctest,'--test-dir',str(work),'--output-junit',str(work/'tests.xml')]
        if not truncated:command += ['--test-output-size-passed',str(CTEST_OUTPUT_LIMIT)]
        subprocess.run(command,env=environment,capture_output=True,check=True,timeout=10)
        rows.append({'name':name,'component':'frontend','status':'PASS','binary_sha256':'a'*64})
    if truncated:
        with pytest.raises(ValueError, match='omitted'):emitter_partition(tmp_path,rows)
    else:
        assert emitter_partition(tmp_path,rows)['cases']==2 * SHARDS + 3


@pytest.mark.parametrize('mutation', [None, 'bool', 'small', 'large', 'missing', 'duplicate'])
def test_policy_count_and_complete_selected_inventory_are_one_contract(mutation):
    policy = {'emitter_shards': SHARDS, 'frontend_tests': list(EMITTER_TESTS)}
    if mutation == 'bool':policy['emitter_shards'] = True
    if mutation == 'small':policy['emitter_shards'] = 7
    if mutation == 'large':policy['emitter_shards'] = 65
    if mutation == 'missing':policy['frontend_tests'].pop()
    if mutation == 'duplicate':policy['frontend_tests'].append(EMITTER_TESTS[0])
    if mutation:
        with pytest.raises(ValueError):shard_count(policy)
    else:assert shard_count(policy) == SHARDS


def test_actual_cmake_registers_policy_count_and_explicit_binary_partition(tmp_path):
    import json
    from pathlib import Path
    import shutil
    import subprocess
    cmake, ctest = shutil.which('cmake'), shutil.which('ctest')
    if not cmake or not ctest:pytest.skip('requires CMake and CTest')
    root = Path(__file__).resolve().parents[2]
    text = (root / 'native/objc3c/src/clang_foundation/CMakeLists.txt').read_text()
    begin = text.index('  if(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE STREQUAL "compiler-source-coverage")')
    end = text.index('\n  objc3c_add_frontend_test(NAME objc3_standard_library_headers', begin)
    source = tmp_path / 'source';source.mkdir()
    (source / 'CMakeLists.txt').write_text('''cmake_minimum_required(VERSION 3.25)
project(CoverageRegistration NONE)
enable_testing()
add_executable(objc3_interface_emitter_test IMPORTED GLOBAL)
set_target_properties(objc3_interface_emitter_test PROPERTIES IMPORTED_LOCATION /bin/true)
function(objc3c_add_frontend_test)
  cmake_parse_arguments(T "" "NAME" "COMMAND" ${ARGN})
  add_test(NAME ${T_NAME} COMMAND ${T_COMMAND})
endfunction()
set(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE compiler-source-coverage)
''' + f'set(_objc3_repo "{root}")\n' + text[begin:end] + '\n')
    build = tmp_path / 'build'
    subprocess.run([cmake, '-S', str(source), '-B', str(build)],
                   check=True, capture_output=True, text=True, timeout=30)
    found = json.loads(subprocess.check_output([ctest, '--test-dir', str(build),
        '--show-only=json-v1'], text=True, timeout=10))['tests']
    assert [row['name'] for row in found] == list(EMITTER_TESTS)
    assert [row['command'][1:] for row in found] == [
        [f'coverage-shard-{index}-of-{SHARDS}'] for index in range(SHARDS)]
