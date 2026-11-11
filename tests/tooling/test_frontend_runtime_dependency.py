"""Isolated compiler profiles must not borrow an ambient Objective-C runtime."""
import json
from pathlib import Path
import subprocess
from types import SimpleNamespace

import pytest

from scripts.objc3c_toolchain import test_runtime as owner

ROOT = Path(__file__).resolve().parents[2]


@pytest.fixture
def dependency(tmp_path, monkeypatch):
    repo = tmp_path / 'repo';bootstrap = tmp_path / 'bootstrap'
    (bootstrap / 'bin').mkdir(parents=True)
    (bootstrap / 'bin/clang').write_bytes(b'pinned bootstrap')
    pin = repo / 'scripts/objc3c_toolchain_runtime/runtime-profile.json'
    pin.parent.mkdir(parents=True)
    pin.write_text(json.dumps({'profiles': {'linux-x86_64': {
        'runtime_commit': 'a'*40, 'robin_map_commit': 'b'*40}}}))
    stage = repo / 'out/toolchains/libobjc2-v2.3/build-linux-x86_64/key/stage'
    (stage / 'lib').mkdir(parents=True)
    (stage / 'lib/libobjc.so').write_bytes(b'pinned runtime')
    value = {'status': 'passed', 'stage': str(stage), 'source_commit': 'a'*40,
             'robin_map_commit': 'b'*40,
             'configuration': {'compiler_sha256': owner.digest(bootstrap / 'bin/clang')}}
    def run(argv, **kwargs):
        assert kwargs['env']['LD_LIBRARY_PATH'] == str(bootstrap / 'lib')
        assert kwargs['env']['LLVM_PROFILE_FILE'] == '/dev/null'
        Path(argv[argv.index('--report-file')+1]).write_text(json.dumps(value))
        kwargs['stdout'].write('verified native runtime build\n')
        return SimpleNamespace(returncode=0)
    monkeypatch.setattr(owner.subprocess, 'run', run)
    return repo, bootstrap, stage, value


def test_profile_links_only_its_pinned_owned_runtime(tmp_path, dependency):
    repo, bootstrap, stage, _ = dependency
    result = owner.prepare(repo, bootstrap, tmp_path / 'evidence', fetch=True, jobs=2)
    assert result['root'] == str(stage)
    assert result['library_sha256'] == owner.digest(stage / 'lib/libobjc.so')
    assert result['receipt_sha256'] == owner.digest(Path(result['receipt']))
    assert '--fetch' in result['command']


@pytest.mark.parametrize('damage', ['source', 'robin-map', 'compiler', 'escaped-library'])
def test_profile_rejects_a_different_or_escaped_runtime(tmp_path, dependency, damage):
    repo, bootstrap, stage, value = dependency
    if damage == 'source': value['source_commit'] = 'c'*40
    elif damage == 'robin-map': value['robin_map_commit'] = 'c'*40
    elif damage == 'compiler': value['configuration']['compiler_sha256'] = '0'*64
    else:
        outside = tmp_path / 'foreign.so';outside.write_bytes(b'foreign runtime')
        (stage / 'lib/libobjc.so').unlink();(stage / 'lib/libobjc.so').symlink_to(outside)
    with pytest.raises(ValueError, match='pinned compiler/source inputs'):
        owner.prepare(repo, bootstrap, tmp_path / 'evidence', fetch=False, jobs=2)


@pytest.mark.parametrize('present', [True, False])
def test_real_cmake_requires_the_explicit_runtime_without_ambient_fallback(tmp_path, present):
    # Execute the production lookup with independent fake library files. This
    # checks configuration routing, not the validity of a compiled runtime.
    text = (ROOT / 'native/objc3c/src/clang_foundation/CMakeLists.txt').read_text()
    begin = text.index('  if(OBJC3_LIBOBJC2_BUILD_DIR)')
    end = text.index('  set(_c_inline_link_flags', begin)
    foundation = tmp_path / 'foundation';runtime = tmp_path / 'runtime'
    (foundation / 'lib').mkdir(parents=True);(runtime / 'lib').mkdir(parents=True)
    (foundation / 'lib/libobjc.so').write_bytes(b'ambient runtime')
    if present: (runtime / 'lib/libobjc.so').write_bytes(b'explicit runtime')
    source = tmp_path / 'source';source.mkdir()
    (source / 'CMakeLists.txt').write_text(f'''cmake_minimum_required(VERSION 3.25)
project(RuntimeLookup NONE)
set(OBJC3C_CLANG_FOUNDATION_ROOT "{foundation}")
set(OBJC3_LIBOBJC2_BUILD_DIR "{runtime}")
{text[begin:end]}
file(WRITE "${{CMAKE_BINARY_DIR}}/chosen.txt" "${{_c_inline_objc_runtime}}")
''')
    build = tmp_path / 'build'
    result = subprocess.run(['cmake', '-S', str(source), '-B', str(build)],
                            text=True, capture_output=True, timeout=30)
    if present:
        assert result.returncode == 0, result.stdout + result.stderr
        assert Path((build / 'chosen.txt').read_text()) == runtime / 'lib/libobjc.so'
    else:
        assert result.returncode != 0 and '_c_inline_objc_runtime' in result.stderr


@pytest.mark.parametrize('mode',['foundation','explicit','wrong-version','profile-override','missing'])
def test_frontend_generator_admission_executes_the_actual_cmake_guard(tmp_path,mode):
    text=(ROOT/'native/objc3c/src/clang_foundation/CMakeLists.txt').read_text()
    start=text.index('set(_tablegen ')
    end=text.index('add_custom_command(OUTPUT',start)
    repo=tmp_path/'repo';tool=repo/'out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen'
    tool.parent.mkdir(parents=True)
    version='23.1.4' if mode=='wrong-version' else '23.1.5'
    marker=tmp_path/'executed'
    tool.write_text(f'#!/bin/sh\nprintf ran > "{marker}"\nprintf "LLVM version {version}\\n"\n')
    tool.chmod(0o755)
    source=tmp_path/'source';source.mkdir()
    explicit=tool if mode!='missing' else tmp_path/'absent-generator'
    settings=(f'set(OBJC3C_TABLEGEN "{explicit}")\n' if mode!='foundation' else '')
    if mode=='profile-override':settings+='set(OBJC3C_COMPILER_INSTRUMENTATION_PROFILE compiler-asan-ubsan)\n'
    (source/'CMakeLists.txt').write_text(f'''cmake_minimum_required(VERSION 3.25)
project(FrontendGenerator NONE)
set(_objc3_repo "{repo}")
set(LLVM_PACKAGE_VERSION 23.1.5)
{settings}
{text[start:end]}
''')
    result=subprocess.run(['cmake','-S',str(source),'-B',str(tmp_path/'build')],
                          text=True,capture_output=True,timeout=30)
    if mode in ('foundation','explicit'):
        assert result.returncode==0,result.stdout+result.stderr
        assert owner.digest(tool) in result.stdout and marker.is_file()
    else:
        assert result.returncode!=0
        if mode=='wrong-version':assert 'must match LLVM 23.1.5' in result.stderr
        if mode in ('profile-override','missing'):assert not marker.exists()
