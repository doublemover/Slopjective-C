"""Keep implementation analysis complete across flags, variants and headers."""
import json
import shlex
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain.implementation_analysis import (
    dependencies, dependency_arguments, inventory, matched_pch_arguments, owner_changes,
    selected_variants, textual_pch,
)


@pytest.fixture
def database(tmp_path):
    source = tmp_path / 'service.cpp'
    source.write_text('int value;\n')
    rows = [{'file': str(source), 'directory': str(tmp_path),
        'arguments': ['clang++', '-x', 'objective-c++', '-DVARIANT='+str(index),
            '-c', str(source), '-o', str(tmp_path / (str(index)+'.o'))]} for index in range(2)]
    path = tmp_path / 'compile_commands.json'
    path.write_text(json.dumps(rows))
    graph = '\n'.join(shlex.join(row['arguments']) for row in rows)
    return path, rows, graph, source


def test_actual_compilation_variants_preserve_language_and_feature_defines(database):
    path, _, graph, source = database
    rows = inventory(path, graph, {source})
    assert len(rows) == 2
    assert all(row['arguments'][1:3] == ['-x', 'objective-c++'] for row in rows)
    assert rows[0]['arguments'][3] != rows[1]['arguments'][3]


@pytest.mark.parametrize('mutation', ['missing', 'duplicate', 'flags', 'source', 'output'])
def test_missing_conflicting_and_flag_changed_commands_fail(database, mutation):
    path, rows, graph, source = database
    if mutation == 'missing': rows.pop()
    elif mutation == 'duplicate': rows.append(rows[0])
    elif mutation == 'flags': rows[0]['arguments'][3] = '-DWRONG'
    elif mutation == 'source': rows[0]['arguments'][-3] = '/wrong-source.cpp'
    elif mutation == 'output': rows[0]['arguments'][-1] = '/wrong-output.o'
    path.write_text(json.dumps(rows))
    with pytest.raises(ValueError):
        inventory(path, graph, {source})


def test_response_files_are_bound_and_missing_response_files_fail(database):
    path, rows, _, source = database
    response = path.parent / 'flags.rsp'
    response.write_text('-DHEADER_CONTRACT=1')
    for row in rows:
        row['arguments'].insert(1, '@'+str(response))
    graph = '\n'.join(shlex.join(row['arguments']) for row in rows)
    path.write_text(json.dumps(rows))
    result = inventory(path, graph, {source})
    assert all(len(row['response_files'][str(response)]) == 64 for row in result)
    response.unlink()
    with pytest.raises(FileNotFoundError):
        inventory(path, graph, {source})


def test_header_edit_selects_dependent_variants_without_a_changed_line_filter(database):
    path, _, graph, source = database
    rows = inventory(path, graph, {source})
    header = str(path.parent / 'service_internal.h')
    headers = {rows[0]['output']: [str(source), header], rows[1]['output']: [str(source)]}
    assert selected_variants(rows, headers, [header]) == rows[:1]
    assert selected_variants(rows, headers, [source]) == rows
    assert selected_variants(rows, headers, [path.parent / 'new-header.h']) == rows


def test_changed_native_scopes_do_not_select_unaffected_implementation_owners(tmp_path):
    repo = tmp_path / 'repo';repo.mkdir()
    runtime = repo / 'native/objc3c/src/runtime/service.cpp'
    frontend = repo / 'native/objc3c/src/clang_foundation/frontend.cpp'
    cmake = repo / 'native/cmake/Objc3BuildWarnings.cmake'
    assert owner_changes(repo, 'frontend', [runtime]) == (runtime,)
    assert owner_changes(repo, 'standalone-runtime', [runtime]) == (runtime,)
    assert owner_changes(repo, 'maintained-clang', [runtime]) is None
    assert owner_changes(repo, 'frontend', [frontend]) == (frontend,)
    assert owner_changes(repo, 'standalone-runtime', [frontend]) is None
    assert owner_changes(repo, 'maintained-clang', [frontend]) is None
    assert owner_changes(repo, 'frontend', [cmake]) == (cmake,)
    assert owner_changes(repo, 'standalone-runtime', [cmake]) == (cmake,)


@pytest.mark.parametrize('relative', [
    'native/analysis/SelectedChecks.cpp',
    'toolchains/llvm/implementation-analysis.json',
    'scripts/objc3c_toolchain/implementation_analysis.py',
    'scripts/objc3c_implementation_assurance/runner.py',
    'unknown/build-owner.py',
])
def test_shared_or_unknown_analysis_changes_conservatively_select_every_owner(tmp_path, relative):
    repo = tmp_path / 'repo';repo.mkdir()
    changed = repo / relative
    for owner in ('frontend', 'standalone-runtime', 'maintained-clang'):
        assert owner_changes(repo, owner, [changed]) == ()


def test_dependency_closure_is_required_and_stale_selected_records_fail(tmp_path):
    header = tmp_path / 'contract.h'
    header.touch()
    text = f'unit.o: #deps 1, deps mtime 123 (VALID)\n    {header}\n\n'
    expected = {str(tmp_path / 'unit.o'): [str(header)]}
    assert dependencies(text, build=tmp_path) == expected
    with pytest.raises(ValueError, match='stale'):
        dependencies(text.replace('VALID', 'STALE'), build=tmp_path)
    with pytest.raises(ValueError, match='truncated'):
        dependencies(text.replace('#deps 1', '#deps 2'), build=tmp_path)
    assert dependencies(text.replace('VALID', 'STALE'), build=tmp_path, selected_outputs=set()) == {}


def test_bootstrap_pch_is_replaced_only_by_its_already_forced_exact_wrapper(tmp_path):
    pch, wrapper = tmp_path / 'cmake_pch.hxx.pch', tmp_path / 'cmake_pch.hxx'
    pch.write_bytes(b'bootstrap serialization')
    wrapper.write_text('#include "contract.h"\n')
    prefix = ['clang++', '-DREAL_FEATURE=1', '-Ireal/include', '-std=c++20']
    textual = ['-Xclang', '-include', '-Xclang', str(wrapper)]
    row = {'directory': str(tmp_path), 'arguments': [*prefix, '-Xclang', '-include-pch', '-Xclang', str(pch), *textual]}
    args, proof = textual_pch(row)
    assert args == [*prefix, *textual]
    assert proof[0]['bootstrap_pch'] == str(pch) and len(proof[0]['wrapper_sha256']) == 64
    row['arguments'] = row['arguments'][:-4]
    with pytest.raises(ValueError, match='textual include counterpart'):
        textual_pch(row)


def test_bootstrap_pch_is_rebuilt_by_matched_fork_with_exact_semantic_flags(tmp_path):
    bootstrap, matched = tmp_path / 'bootstrap-clang++', tmp_path / 'matched-clang++'
    bootstrap.touch();matched.touch()
    pch, wrapper = tmp_path / 'cmake_pch.hxx.pch', tmp_path / 'cmake_pch.hxx'
    pch.write_bytes(b'bootstrap serialization');wrapper.write_text('#include "contract.h"\n')
    source, obj, dep = tmp_path / 'source.mm', tmp_path / 'source.o', tmp_path / 'source.d'
    source.write_text('int value;\n')
    forced = ['-Xclang', '-include', '-Xclang', str(wrapper)]
    row = {'file': str(source), 'directory': str(tmp_path), 'arguments': [str(bootstrap),
        '-DREAL_FEATURE=1', '-Ireal/include', '-x', 'objective-c++', '-MD', '-MF', str(dep),
        '-Xclang', '-include-pch', '-Xclang', str(pch), *forced,
        '-o', str(obj), '-c', str(source)]}
    target = tmp_path / 'matched.pch'
    command = matched_pch_arguments(row, matched, target)
    assert command[0] == str(matched)
    assert '-DREAL_FEATURE=1' in command and '-Ireal/include' in command
    assert command[-5:] == ['-x', 'objective-c++-header', str(wrapper), '-o', str(target)]
    assert all(str(path) not in command for path in (bootstrap, pch, source, obj, dep))
    target.write_bytes(b'fork matched serialization')
    projected, proof = textual_pch(row, replacement=target)
    assert str(target) in projected and str(pch) not in projected
    assert forced == projected[projected.index(str(target))+1:projected.index(str(target))+5]
    assert proof[0]['matched_pch_sha256'] and proof[0]['bootstrap_pch'] == str(pch)


def test_dependency_discovery_expands_the_pch_wrapper_with_real_defines(tmp_path):
    pch, wrapper = tmp_path / 'cmake_pch.hxx.pch', tmp_path / 'cmake_pch.hxx'
    pch.write_bytes(b'serialized header');wrapper.write_text('#include "shared.h"\n')
    row = {'directory': str(tmp_path), 'arguments': ['clang++', '-DREAL=1', '-x', 'objective-c++',
        '-Xclang', '-include-pch', '-Xclang', str(pch),
        '-Xclang', '-include', '-Xclang', str(wrapper),
        '-MD', '-MF', 'old.d', '-c', 'source.cpp', '-o', 'source.o']}
    argv = dependency_arguments(row, tmp_path / 'fresh.d')
    assert str(pch) not in argv and '-include-pch' not in argv
    assert argv[:4] == ['clang++', '-DREAL=1', '-x', 'objective-c++']
    assert str(wrapper) in argv and 'source.cpp' in argv
    assert '-c' not in argv and 'source.o' not in argv and 'old.d' not in argv
    assert argv[-5:] == ['-M', '-MF', str(tmp_path / 'fresh.d'), '-MT', 'implementation-analysis']


@pytest.mark.parametrize('fault', ['truncated', 'linked-bootstrap', 'linked-replacement', 'empty-replacement'])
def test_pch_projection_rejects_missing_or_unowned_serializations(tmp_path, fault):
    pch, wrapper, replacement = (tmp_path / name for name in
                                 ('cmake_pch.hxx.pch', 'cmake_pch.hxx', 'matched.pch'))
    pch.write_bytes(b'bootstrap');wrapper.write_text('header');replacement.write_bytes(b'matched')
    row = {'directory': str(tmp_path), 'arguments': ['clang++',
        '-Xclang', '-include-pch', '-Xclang', str(pch),
        '-Xclang', '-include', '-Xclang', str(wrapper)]}
    if fault == 'truncated': row['arguments'] = row['arguments'][:4]
    elif fault == 'linked-bootstrap': pch.unlink();pch.symlink_to(replacement)
    elif fault == 'linked-replacement': replacement.unlink();replacement.symlink_to(pch)
    else: replacement.write_bytes(b'')
    with pytest.raises(ValueError): textual_pch(row, replacement=replacement)


def test_real_cmake_pch_dependency_discovery_keeps_shared_header_edits(tmp_path):
    """Exercise the PCH mechanism with real tools; this is not fork qualification."""
    compiler = shutil.which('clang++')
    if not compiler: pytest.skip('Clang unavailable for native PCH mechanism control')
    source = tmp_path / 'source';source.mkdir()
    build = tmp_path / 'build'
    header = source / 'shared.h';header.write_text('#pragma once\nconstexpr int shared = FEATURE;\n')
    unit = source / 'unit.cpp';unit.write_text('int value() { return shared; }\n')
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(pch_dependency_control LANGUAGES CXX)\n'
        'set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n'
        'add_library(control STATIC unit.cpp)\n'
        'target_compile_definitions(control PRIVATE FEATURE=42)\n'
        'target_precompile_headers(control PRIVATE shared.h)\n')
    subprocess.run(['cmake', '-S', source, '-B', build, '-G', 'Ninja',
                    '-DCMAKE_CXX_COMPILER='+compiler], check=True, capture_output=True)
    subprocess.run(['cmake', '--build', build], check=True, capture_output=True)
    rows = json.loads((build / 'compile_commands.json').read_text())
    row = next(row for row in rows if row['file'] == str(unit))
    row['arguments'] = shlex.split(row['command'])
    depfile = tmp_path / 'expanded.d'
    subprocess.run(dependency_arguments(row, depfile), cwd=build, check=True, capture_output=True)
    from scripts.objc3c_toolchain.sdk_example import dependencies as header_dependencies
    closure = [str(path) for path in header_dependencies(depfile, build)]
    assert str(header) in closure and str(unit) in closure
    assert selected_variants([{'file': str(unit), 'output': 'unit.o'}],
        {'unit.o': closure}, [header]) == [{'file': str(unit), 'output': 'unit.o'}]


def test_analysis_tool_configuration_uses_admitted_ninja_with_a_clean_path(tmp_path):
    """Run real CMake/Ninja with the admitted Ninja outside the process PATH."""
    import os
    from pathlib import Path
    from scripts.objc3c_toolchain.implementation_analysis import tool_configuration
    compiler, cmake, ninja = map(shutil.which, ('clang++', 'cmake', 'ninja'))
    if not all((compiler, cmake, ninja)):
        pytest.skip('requires CMake, Clang and Ninja for clean-host tool control')
    bootstrap = Path(compiler).resolve().parent.parent
    if not (bootstrap / 'bin/clang++').exists(): pytest.skip('Clang installation root is unavailable')
    owned = tmp_path / 'owned tools';owned.mkdir()
    wrapper, observed = owned / 'ninja-owned', owned / 'observed'
    wrapper.write_text('#!/bin/sh\nprintf "invoked\\n" >> '+shlex.quote(str(observed))+'\nexec '+
                       shlex.quote(ninja)+' "$@"\n');wrapper.chmod(0o755)
    source = tmp_path / 'repo/native/analysis';source.mkdir(parents=True)
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(clean_host_analysis CXX)\nadd_executable(control main.cpp)\n')
    (source / 'main.cpp').write_text('int main() { return 0; }\n')
    build = tmp_path / 'build'
    plan = {'host_environment': {'bootstrap_root': str(bootstrap), 'cmake_path': cmake,
                                'ninja_path': str(wrapper)},
            'paths': {'stage': str(tmp_path / 'stage'), 'source': str(tmp_path / 'llvm')}}
    arguments, environment = tool_configuration(tmp_path / 'repo', build, plan)
    assert str(owned) in environment['PATH'].split(os.pathsep)
    assert '-DCMAKE_MAKE_PROGRAM='+str(wrapper) in arguments
    environment['PATH'] = str(bootstrap / 'bin')+os.pathsep+os.defpath
    assert str(owned) not in environment['PATH'].split(os.pathsep)
    subprocess.run(list(map(str, arguments)), check=True, capture_output=True, env=environment, timeout=60)
    subprocess.run([cmake, '--build', build, '--parallel', '1'], check=True, capture_output=True,
                   env=environment, timeout=60)
    assert observed.read_text().count('invoked') >= 1
    cache = (build / 'CMakeCache.txt').read_text().splitlines()
    assert next(line for line in cache if line.startswith('CMAKE_MAKE_PROGRAM:')).endswith('='+str(wrapper))
