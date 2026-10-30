"""Real object controls defend compiler implementation instrumentation evidence."""
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.compiler_instrumentation import graph_records, verify_symbols


@pytest.fixture
def native_graph(tmp_path):
    root = os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT')
    if not root:
        pytest.skip('requires the explicitly selected native bootstrap LLVM')
    compiler, nm = (Path(root) / 'bin' / name for name in ('clang++', 'llvm-nm'))
    ninja = Path(shutil.which('ninja')).resolve()
    source, build = tmp_path / 'source', tmp_path / 'build'
    source.mkdir()
    (source / 'unit.cpp').write_text('int checked(int value) { return value + 1; }\n')
    (source / 'CMakeLists.txt').write_text('''cmake_minimum_required(VERSION 3.20)
project(control LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(control STATIC unit.cpp)
target_compile_options(control PRIVATE -O1 -gline-tables-only -fno-omit-frame-pointer
  -fsanitize=address,undefined -fno-sanitize-recover=undefined)
''')
    def run(name, argv):
        return subprocess.check_output(list(map(str, argv)), stderr=subprocess.STDOUT,
                                       text=True, timeout=60)
    run('configure', ['cmake', '-S', source, '-B', build, '-G', 'Ninja',
                      '-DCMAKE_CXX_COMPILER='+str(compiler)])
    run('build', ['cmake', '--build', build])
    def collect():
        return graph_records(build, source, ['control'], ninja, dict(os.environ),
                             foundation=False, run=run)
    records = collect()
    verify_symbols(records, nm, run=run)
    assert len(records) == 1
    assert any('__ubsan_handle_add_overflow' in item
               for item in records[0]['instrumentation_symbols'])
    return source, build, compiler, nm, run, collect


@pytest.mark.parametrize('mutation', ['disabled-tu', 'recovery', 'omitted-command', 'ordinary-object'])
def test_actual_compiler_graph_rejects_weakened_evidence(native_graph, mutation):
    source, build, compiler, nm, run, collect = native_graph
    path = build / 'compile_commands.json'
    commands = json.loads(path.read_text())
    if mutation == 'ordinary-object':
        record, = collect()
        run('replace', [compiler, '-O1', '-c', source / 'unit.cpp', '-o', record['object']])
        # Requested flags still look correct, but the replacement bytes are not.
        with pytest.raises(ValueError, match='ASan symbol evidence'):
            verify_symbols(collect(), nm, run=run)
        return
    if mutation == 'omitted-command':
        commands.clear()
    elif mutation == 'recovery':
        commands[0]['command'] += ' -fsanitize-recover=undefined'
    else:
        commands[0]['command'] = commands[0]['command'].replace('-fsanitize=address,undefined', '')
    path.write_text(json.dumps(commands))
    with pytest.raises(ValueError, match='instrumentation|missing compilation|compilation ownership'):
        collect()


def test_compilation_database_cannot_lie_about_the_ninja_command(native_graph):
    source, build, compiler, nm, run, collect = native_graph
    path = build / 'compile_commands.json'
    commands = json.loads(path.read_text())
    commands[0]['command'] += ' -DOBJC3_UNEXECUTED_FLAG=1'
    path.write_text(json.dumps(commands))
    with pytest.raises(ValueError, match='differs from compilation ownership'):
        collect()


def test_custom_product_objects_have_canonical_sources_and_real_instrumentation(native_graph):
    source, build, compiler, nm, run, collect = native_graph
    extra = source / 'extra.cpp'
    extra.write_text('int extra(int value) { return value * 7; }\n')
    output = build / 'extra.o'
    manifest = json.dumps([{'file': str(extra), 'output': str(output)}])
    with (source / 'CMakeLists.txt').open('a') as stream:
        stream.write(f'''
add_custom_command(OUTPUT "{output}"
  COMMAND "{compiler}" -O1 -gline-tables-only -fno-omit-frame-pointer
    -fsanitize=address,undefined -fno-sanitize-recover=undefined
    -c "{extra}" -o "{output}" DEPENDS "{extra}" VERBATIM)
set_source_files_properties("{output}" PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
target_sources(control PRIVATE "{output}")
file(GENERATE OUTPUT "${{CMAKE_BINARY_DIR}}/custom-compile-records.json" CONTENT [==[{manifest}]==])
''')
    run('configure-custom', ['cmake', '-S', source, '-B', build])
    run('build-custom', ['cmake', '--build', build])
    records = collect()
    assert len(records) == 2
    verify_symbols(records, nm, run=run)
    # Preserve requested flags and manifest; replace only the actual custom TU.
    run('replace-custom', [compiler, '-O1', '-c', extra, '-o', output])
    with pytest.raises(ValueError, match='ASan symbol evidence'):
        verify_symbols(collect(), nm, run=run)
    (build / 'custom-compile-records.json').write_text(json.dumps([
        {'file': str(source / 'unit.cpp'), 'output': str(output)}]))
    with pytest.raises(ValueError, match='canonical CMake source'):
        collect()
