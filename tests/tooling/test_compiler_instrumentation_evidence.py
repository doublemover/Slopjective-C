"""Real object controls defend compiler implementation instrumentation evidence."""
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain.compiler_instrumentation import graph_records, verify_symbols, verify_owned_shared_libraries


def test_host_loader_has_one_canonical_gnu_build_identity():
    from scripts.objc3c_toolchain.compiler_instrumentation import elf_gnu_build_id
    from scripts.objc3c_toolchain.linux_host import native_loader
    assert re.fullmatch(r'[0-9a-f]{40}', elf_gnu_build_id(native_loader()))


def test_loader_graph_rejects_selected_path_replacement_without_executing_it(tmp_path):
    from scripts.objc3c_toolchain.compiler_instrumentation import loaded_graph
    from scripts.objc3c_toolchain.linux_host import native_loader

    selected = tmp_path / 'libLLVMcontrol.so'
    replacement = tmp_path / 'replacement.so'
    shutil.copyfile(native_loader(), selected)
    shutil.copyfile(Path(sys.executable).resolve(strict=True), replacement)

    def replace_then_resolve(name, argv):
        assert name == 'loader'
        os.replace(replacement, selected)
        return f'libLLVMcontrol.so => {selected} (0x00000001)\n'

    with pytest.raises(ValueError, match='build identity differs'):
        loaded_graph(Path(sys.executable).resolve(strict=True), native_loader(),
                     [selected], required=(selected,), run=replace_then_resolve)


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


@pytest.mark.parametrize('mutation', ['', 'installed-rpath', 'foreign-byte', 'ordinary-staged-library', 'unverified-object', 'weakened-link'])
def test_staged_shared_library_binds_canonical_instrumented_link_inputs(native_graph, mutation):
    source, build, compiler, nm, run, collect = native_graph
    with (source / 'CMakeLists.txt').open('a') as stream:
        stream.write('''
add_library(owned SHARED unit.cpp)
target_link_libraries(owned PRIVATE control)
set_target_properties(owned PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
target_compile_options(owned PRIVATE -O1 -gline-tables-only -fno-omit-frame-pointer
  -fsanitize=address,undefined -fno-sanitize-recover=undefined)
target_link_options(owned PRIVATE -fsanitize=address,undefined)
target_link_options(owned PRIVATE "-Wl,-rpath,$ORIGIN/../lib:$ORIGIN/../lib/x86_64-unknown-linux-gnu:")
''')
    run('configure-shared', ['cmake', '-S', source, '-B', build])
    run('build-shared', ['cmake', '--build', build, '--target', 'owned'])
    ninja = Path(shutil.which('ninja')).resolve()
    records = graph_records(build, source, ['owned'], ninja, dict(os.environ), foundation=False, run=run)
    verify_symbols(records, nm, run=run)
    stage = source.parent / 'stage'
    stage.mkdir()
    library = stage / 'libowned.so'
    shutil.copyfile(build / 'lib/libowned.so', library)
    if mutation in ('installed-rpath', 'foreign-byte'):
        script = stage / 'install.cmake'
        rpath = '$ORIGIN/../lib:$ORIGIN/../lib/x86_64-unknown-linux-gnu'
        script.write_text('file(RPATH_CHANGE FILE [==['+str(library)+']==] OLD_RPATH [==['+
                          rpath+':]==] NEW_RPATH [==['+rpath+']==])\n')
        run('installed-rpath', ['cmake', '-P', script])
        if mutation == 'foreign-byte':
            with library.open('ab') as stream:
                stream.write(b'changed native artifact')
    if mutation == 'ordinary-staged-library':
        run('ordinary-library', [compiler, '-shared', '-fPIC', source / 'unit.cpp', '-o', library])
    if mutation == 'unverified-object':
        records = []
    def query(name, argv):
        text = run(name, argv)
        return text.replace('-fsanitize=address,undefined', '') if mutation == 'weakened-link' and 'command' in name else text
    if mutation not in ('', 'installed-rpath'):
        with pytest.raises(ValueError, match='canonical link output|verified ownership|sanitizer profile'):
            verify_owned_shared_libraries(build, [library], records, [], ninja, run=query)
    else:
        result, = verify_owned_shared_libraries(build, [library], records, [], ninja, run=query)
        assert result['sha256'] == __import__('hashlib').sha256(library.read_bytes()).hexdigest()
        assert result['canonical_output'] == str(build / 'lib/libowned.so')
        assert result['inputs'][0]['classification'] == 'verified-object'
        assert len(result['additional_archives']) == 1
        assert bool(result['stage_transform']) == (mutation == 'installed-rpath')


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


def test_actual_loader_rejects_a_foreign_llvm_library(native_graph, tmp_path):
    from scripts.objc3c_toolchain.compiler_instrumentation import elf_gnu_build_id, loaded_graph
    source, build, compiler, nm, run, collect = native_graph
    owned, foreign = (tmp_path / name for name in ('owned', 'foreign'))
    for directory in (owned, foreign):
        directory.mkdir()
    library_source = source / 'library.cpp'
    library_source.write_text('extern "C" int probe() { return 42; }\n')
    main = source / 'main.cpp'
    main.write_text('extern "C" int probe(); int main() { return probe() == 42 ? 0 : 1; }\n')
    for directory in (owned, foreign):
        run('shared', [compiler, '-shared', '-fPIC', '-fuse-ld=lld', library_source,
                       '-Wl,--build-id=sha1', '-o', directory / 'libLLVMcontrol.so'])
    selected = owned / 'libLLVMcontrol.so'
    loader = Path('/lib64/ld-linux-x86-64.so.2').resolve(strict=True)
    for directory in (owned, foreign):
        binary = build / directory.name
        run('link', [compiler, '-fuse-ld=lld', main, '-L'+str(directory), '-lLLVMcontrol',
                     '-Wl,--build-id=sha1', '-Wl,--disable-new-dtags',
                     '-Wl,-rpath,'+str(directory), '-o', binary])
        run('semantic-control', [binary])
        if directory == owned:
            graph = loaded_graph(binary, loader, [selected], required=(selected,), run=run)
            assert str(selected) in graph['libraries']
            assert graph['build_ids'][str(selected)] == elf_gnu_build_id(selected)
            assert graph['expected_build_ids'] == {str(selected): elf_gnu_build_id(selected)}
            assert graph['selected_build_ids'] == {str(selected): elf_gnu_build_id(selected)}
        else:
            with pytest.raises(ValueError, match='unselected LLVM/Clang library'):
                loaded_graph(binary, loader, [selected], run=run)
            with pytest.raises(ValueError, match='required instrumented libraries'):
                loaded_graph(binary, loader, [selected], required=(selected,), run=run)


def test_actual_loader_rejects_same_path_build_identity_replacement(tmp_path):
    from scripts.objc3c_toolchain.compiler_instrumentation import loaded_graph
    from scripts.objc3c_toolchain.linux_host import native_loader

    root = os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT')
    if not root:
        pytest.skip('requires the explicitly selected native bootstrap LLVM')
    compiler = Path(root) / 'bin' / 'clang++'
    source = tmp_path / 'source'
    source.mkdir()
    def run(name, argv):
        return subprocess.check_output(list(map(str, argv)), stderr=subprocess.STDOUT,
                                       text=True, timeout=60)
    directory = tmp_path / 'selected'
    directory.mkdir()
    selected = directory / 'libLLVMcontrol.so'
    replacement = directory / 'replacement.so'
    library_source = source / 'library.cpp'
    main = source / 'main.cpp'
    library_source.write_text('extern "C" int llvm_control() { return 7; }\n')
    main.write_text('extern "C" int llvm_control(); int main() { return llvm_control() != 9; }\n')
    run('selected-library', [compiler, '-shared', '-fPIC', library_source,
                            '-Wl,--build-id=sha1', '-Wl,-soname,libLLVMcontrol.so',
                            '-o', selected])
    library_source.write_text('extern "C" int llvm_control() { return 9; }\n')
    run('replacement-library', [compiler, '-shared', '-fPIC', library_source,
                               '-Wl,--build-id=sha1', '-Wl,-soname,libLLVMcontrol.so',
                               '-o', replacement])
    binary = tmp_path / 'loader-control'
    run('loader-control-link', [compiler, '-fuse-ld=lld', main, '-L'+str(directory),
                               '-lLLVMcontrol', '-Wl,--build-id=sha1',
                               '-Wl,--disable-new-dtags', '-Wl,-rpath,'+str(directory),
                               '-o', binary])

    def replace_before_resolution(name, argv):
        if name == 'loader':
            os.replace(replacement, selected)
        return run(name, argv)

    with pytest.raises(ValueError, match='build identity differs'):
        loaded_graph(binary, native_loader(), [selected], required=(selected,),
                     run=replace_before_resolution)


def test_real_cxx20_module_scanning_does_not_duplicate_compiler_objects(native_graph):
    source, build, compiler, nm, run, collect = native_graph
    with (source / 'CMakeLists.txt').open('a') as stream:
        stream.write('\nset_target_properties(control PROPERTIES CXX_STANDARD 20 CXX_SCAN_FOR_MODULES ON)\n')
    run('configure-cxx20', ['cmake', '-S', source, '-B', build])
    run('build-cxx20', ['cmake', '--build', build])
    records = collect()
    assert len(records) == 1
    assert Path(records[0]['dependency_scan']['output']).is_file()
    verify_symbols(records, nm, run=run)
