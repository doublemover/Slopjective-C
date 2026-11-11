"""Exercise lit prerequisite materialization with a real CMake/Ninja build."""
from pathlib import Path
import subprocess
import pytest
from scripts.objc3c_assurance.upstream import UPSTREAM_TOOLS, prepare_upstream_tools

@pytest.mark.parametrize('mode', ['complete', 'unknown-target', 'unproduced'])
def test_actual_upstream_tool_build_admits_every_executable(tmp_path, mode):
    repo=tmp_path/'repo';source=tmp_path/'source';source.mkdir()
    build=repo/'out/toolchains/objc3-development/build-linux-x86_64'
    (source/'probe.c').write_text('int main(void){return 37;}\n')
    commands=['cmake_minimum_required(VERSION 3.25)', 'project(UpstreamTools C)',
              'set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")']
    for tool in UPSTREAM_TOOLS:
        if tool==UPSTREAM_TOOLS[0] and mode=='unknown-target':continue
        commands.append(f'add_custom_target({tool})' if tool==UPSTREAM_TOOLS[0] and mode=='unproduced'
                        else f'add_executable({tool} probe.c)')
    (source/'CMakeLists.txt').write_text('\n'.join(commands)+'\n')
    subprocess.run(['cmake','-S',str(source),'-B',str(build),'-G','Ninja'],
                   check=True,capture_output=True,text=True,timeout=30)
    recorded=[]
    def execute(label,argv):
        recorded.append(label)
        subprocess.run(list(map(str,argv)),check=True,capture_output=True,text=True,timeout=30)
    if mode=='unknown-target':
        with pytest.raises(subprocess.CalledProcessError):prepare_upstream_tools(repo,execute)
    elif mode=='unproduced':
        with pytest.raises(ValueError,match='missing after build'):prepare_upstream_tools(repo,execute)
    else:
        tools=prepare_upstream_tools(repo,execute)
        assert set(tools)=={'split-file','opt','llvm-profdata','llvm-lto','llvm-lto2','yaml2obj','clang-scan-deps'}
        assert all(len(value)==64 for value in tools.values())
        for tool in tools:
            assert subprocess.run([str(build/'bin'/tool)],timeout=5).returncode==37
    assert recorded==['upstream-tools-build']
