"""Sharded coverage must retain the whole native emitter case inventory."""
import xml.etree.ElementTree as ET

import pytest

from scripts.objc3c_assurance.emitter_shards import EMITTER_TESTS, SHARDS, emitter_partition


@pytest.mark.parametrize('mutation', [None, 'missing', 'duplicate', 'case', 'pass', 'catalogue',
                                      'binary', 'partial', 'failure', 'index'])
def test_shard_completeness_is_checked_from_retained_native_junit(tmp_path, mutation):
    catalogue = [f'TestCase{index}' for index in range(19)]
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
        assert emitter_partition(tmp_path, rows)['cases'] == 19


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
  objc3::clang_foundation::test::IsolatedCases cases(std::stoul(argv[1]), 8);
  for(unsigned i=0;i<19;++i)cases.Run("TestCase"+std::to_string(i), [] {
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
        (work/'CTestTestfile.cmake').write_text(f'add_test({name} "{binary}" {index})\n')
        command=[ctest,'--test-dir',str(work),'--output-junit',str(work/'tests.xml')]
        if not truncated:command += ['--test-output-size-passed',str(CTEST_OUTPUT_LIMIT)]
        subprocess.run(command,env=environment,capture_output=True,check=True,timeout=10)
        rows.append({'name':name,'component':'frontend','status':'PASS','binary_sha256':'a'*64})
    if truncated:
        with pytest.raises(ValueError, match='omitted'):emitter_partition(tmp_path,rows)
    else:
        assert emitter_partition(tmp_path,rows)['cases']==19
