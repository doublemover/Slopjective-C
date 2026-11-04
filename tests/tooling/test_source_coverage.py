"""Coverage completeness is independent of attractive percentages."""
import json
import shlex

import pytest

from scripts.objc3c_toolchain.source_coverage import binary_ids, compile_inventory, raw_profiles, read_export

FLAGS = ['-fprofile-instr-generate', '-fcoverage-mapping', '-fprofile-update=atomic']


def test_binary_identity_uses_the_pinned_tools_actual_unindented_format():
    assert binary_ids('Total count: 42\nBinary IDs: \n'+'a'*40+'\n') == {'a'*40}
    for text in ('', 'Binary IDs:\n', 'Binary IDs:\nnot-a-build-id\n'):
        with pytest.raises(ValueError, match='binary identit'): binary_ids(text)


@pytest.mark.parametrize('mutation', [None, 'flag', 'disabled', 'owner', 'duplicate', 'source'])
def test_real_graph_cannot_lose_coverage_objects_or_instrumentation(tmp_path, mutation):
    source, output = tmp_path / 'source.cpp', tmp_path / 'source.o'
    source.write_text('int function() { return 42; }');output.write_bytes(b'object sections checked by native tool')
    argv = ['clang++', *FLAGS, '-c', str(source), '-o', str(output)]
    row = {'directory': str(tmp_path), 'file': str(source), 'arguments': list(argv)}
    if mutation == 'flag': argv.remove('-fcoverage-mapping');row['arguments'] = list(argv)
    elif mutation == 'disabled': argv.append('-fno-profile-instr-generate');row['arguments'] = list(argv)
    elif mutation == 'source': row['file'] = str(tmp_path / 'wrong.cpp')
    (tmp_path / 'compile_commands.json').write_text(json.dumps([] if mutation == 'owner' else [row]))
    graph = shlex.join(argv)+ ('\n'+shlex.join(argv) if mutation == 'duplicate' else '')
    if mutation:
        with pytest.raises(ValueError): compile_inventory(tmp_path, graph, required_flags=FLAGS)
    else:
        rows = compile_inventory(tmp_path, graph, required_flags=FLAGS)
        assert len(rows) == 1 and rows[0]['source'] == str(source)


@pytest.mark.parametrize('mutation', ['empty', 'zero-byte', 'symlink', 'unowned'])
def test_missing_or_unowned_profiles_never_pass(tmp_path, mutation):
    root = tmp_path / 'raw';root.mkdir()
    if mutation == 'zero-byte': (root / 'profile.profraw').touch()
    elif mutation == 'symlink':
        external = tmp_path / 'external';external.write_bytes(b'profile');(root / 'profile.profraw').symlink_to(external)
    elif mutation == 'unowned': (root / 'not-a-profile.txt').write_text('wrong')
    with pytest.raises(ValueError, match='raw coverage profiles'):
        raw_profiles(root)


def test_unreached_owned_sources_stay_in_inventory_and_system_headers_do_not(tmp_path):
    repo, build = tmp_path / 'repo', tmp_path / 'build';repo.mkdir();build.mkdir()
    source = repo / 'native.cpp';source.write_text('int function() { return 42; }')
    summary = {key: {'count': 2, 'covered': 0} for key in ('functions', 'regions', 'branches', 'lines')}
    value = {'type': 'llvm.coverage.json.export', 'data': [{'files': [
        {'filename': str(source), 'summary': summary, 'branches': [[1, 1, 1, 2, 0, 0]]},
        {'filename': '/usr/include/header.h', 'summary': summary}]}]}
    path = tmp_path / 'export.json';path.write_text(json.dumps(value))
    rows = read_export(path, repo=repo, build=build, eligible={'native.cpp', 'uninstantiated.h'})
    assert set(rows) == {'native.cpp', 'uninstantiated.h'}
    assert rows['native.cpp']['state'] == 'zero-hit'
    assert rows['uninstantiated.h'] == {'state': 'no-emitted-mapping', 'summary': None}
    value['data'][0]['files'][0]['summary']['regions']['covered'] = 3
    path.write_text(json.dumps(value))
    with pytest.raises(ValueError, match='denominator'):
        read_export(path, repo=repo, build=build, eligible={'native.cpp'})
