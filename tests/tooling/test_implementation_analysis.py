"""Keep implementation analysis complete across flags, variants and headers."""
import json
import shlex

import pytest

from scripts.objc3c_toolchain.implementation_analysis import dependencies, inventory, selected_variants, textual_pch


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


def test_dependency_closure_is_required_and_stale_selected_records_fail(tmp_path):
    header = tmp_path / 'contract.h'
    header.touch()
    text = f'unit.o: #deps 1, deps mtime 123 (VALID)\n    {header}\n\n'
    expected = {str(tmp_path / 'unit.o'): [str(header)]}
    assert dependencies(text, build=tmp_path) == expected
    with pytest.raises(ValueError, match='stale'):
        dependencies(text.replace('VALID', 'STALE'), build=tmp_path)
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
