"""Coverage completeness is independent of attractive percentages."""
import json
from pathlib import Path
import shlex

import pytest

from scripts.objc3c_toolchain.source_coverage import (
    binary_ids, compile_inventory, elf_build_id, profile_runtime, raw_profiles, read_export,
    partition_catalogue, reclaim_indexed_export, reclaim_raw_profiles,
    single_profile_binary_id, suite_timeout, validate_foundation_identity,
)

FLAGS = ['-fprofile-instr-generate', '-fcoverage-mapping', '-fprofile-update=atomic']


def test_portable_foundation_identity_replaces_only_the_exact_coverage_source_tree():
    root = Path(__file__).resolve().parents[2]
    manifest = json.loads((root / 'toolchains/llvm/manifest.json').read_text())
    plan = {'source_build_id': 'exact-source', 'build_config_digest': 'b' * 64,
            'paths': {'build': '/owned/build'},
            'instrumentation': {'kind': 'source-coverage'}}
    identity = {'schema': 'objc3c.portable-foundation-identity.v1',
                'profile': 'compiler-source-coverage',
                'source_build_id': 'exact-source', 'build_config_digest': 'b' * 64,
                'paths': plan['paths'], 'instrumentation': plan['instrumentation'],
                'upstream_commit': manifest['upstream']['commit'], 'patch_digest': 'c' * 64}
    assert validate_foundation_identity(root, plan, identity) == identity
    for field, value in (('profile', 'compiler-asan-ubsan'),
                         ('source_build_id', 'other-source'),
                         ('build_config_digest', 'd' * 64),
                         ('paths', {'build': '/substituted'}),
                         ('instrumentation', {'kind': 'sanitizer'}),
                         ('upstream_commit', 'e' * 40),
                         ('patch_digest', 'short')):
        damaged = {**identity, field: value}
        with pytest.raises(ValueError, match='portable coverage'):
            validate_foundation_identity(root, plan, damaged)


def test_eight_coverage_partitions_form_one_exact_disjoint_catalogue():
    policy = {'frontend_tests': ['frontend-'+str(index) for index in range(35)],
              'runtime_tests': ['runtime-'+str(index) for index in range(10)]}
    partitions = [partition_catalogue(policy, index, 8) for index in range(8)]
    signatures = {signature for _, signature in partitions}
    rows = [row for selected, _ in partitions for row in selected]
    expected = [(component, name) for component in ('frontend', 'runtime')
                for name in policy[component+'_tests']]
    assert len(signatures) == 1
    assert len(rows) == len(set(rows)) and set(rows) == set(expected)
    assert max(map(len, (selected for selected, _ in partitions))) - min(
        map(len, (selected for selected, _ in partitions))) <= 1
    for index, count in ((-1, 8), (8, 8), (0, 3), (0, 16)):
        with pytest.raises(ValueError, match='bounded matrix'):
            partition_catalogue(policy, index, count)


def test_instrumented_suite_timeouts_are_reviewed_and_bounded():
    policy = {'frontend_tests': ['ordinary', 'heavy'], 'runtime_tests': [],
              'test_timeouts_seconds': {'default': 120, 'frontend:heavy': 300}}
    assert suite_timeout(policy, 'frontend', 'ordinary') == 120
    assert suite_timeout(policy, 'frontend', 'heavy') == 300
    for invalid in (
            {'default': 0},
            {'default': 120, 'frontend:unknown': 300},
            {'default': 120, 'frontend:heavy': 901}):
        policy['test_timeouts_seconds'] = invalid
        with pytest.raises(ValueError, match='timeout budgets'):
            suite_timeout(policy, 'frontend', 'heavy')


def test_profile_runtime_is_exact_nonempty_pinned_target_input(tmp_path):
    runtime = tmp_path / 'lib/clang/23/lib/x86_64-unknown-linux-gnu/libclang_rt.profile.a'
    runtime.parent.mkdir(parents=True);runtime.write_bytes(b'profile runtime')
    resource, selected = profile_runtime(tmp_path, 'x86_64-unknown-linux-gnu')
    assert resource == tmp_path / 'lib/clang/23'
    assert selected == runtime
    runtime.unlink()
    with pytest.raises(ValueError, match='profile runtime'):
        profile_runtime(tmp_path, 'x86_64-unknown-linux-gnu')


def test_binary_identity_uses_the_pinned_tools_actual_unindented_format():
    assert binary_ids('Total count: 42\nBinary IDs: \n'+'a'*40+'\n') == {'a'*40}
    for text in ('', 'Binary IDs:\n', 'Binary IDs:\nnot-a-build-id\n'):
        with pytest.raises(ValueError, match='binary identit'): binary_ids(text)


def test_elf_build_identity_requires_one_canonical_sha1_note():
    value = 'a' * 40
    assert elf_build_id('Notes at offset 0x1:\n  Build ID: '+value+'\n') == value
    for text in ('', 'Build ID: short', 'Build ID: '+value+'\nBuild ID: '+'b'*40):
        with pytest.raises(ValueError, match='one build identity'):
            elf_build_id(text)


def test_raw_module_profile_requires_one_binary_owner():
    value = 'a' * 40
    assert single_profile_binary_id('Binary IDs:\n'+value+'\n') == value
    with pytest.raises(ValueError, match='one binary identity'):
        single_profile_binary_id('Binary IDs:\n'+value+'\n'+'b'*40+'\n')


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


def test_raw_profile_size_override_remains_explicit_and_bounded(tmp_path):
    root = tmp_path / 'raw';root.mkdir();profile = root / 'profile.profraw';profile.write_bytes(b'ab')
    assert raw_profiles(root, limit=2) == [profile]
    with pytest.raises(ValueError, match='raw coverage profiles'):
        raw_profiles(root, limit=1)


def test_raw_profiles_are_hashed_and_reclaimed_after_indexing(tmp_path):
    one = tmp_path / 'one.profraw';one.write_bytes(b'one')
    two = tmp_path / 'two.profraw';two.write_bytes(b'two')
    records = reclaim_raw_profiles([one, two])
    assert records == {
        'one.profraw': {'sha256': '7692c3ad3540bb803c020b3aee66cd8887123234ea0c6e7143c0add73ff431ed', 'bytes': 3},
        'two.profraw': {'sha256': '3fc4ccfe745870e2c0d99f71f30ff0656c8dedd41cc1d7d3d376b0dbe685e2f3', 'bytes': 3},
    }
    assert not one.exists() and not two.exists()


def test_raw_coverage_export_is_hashed_and_reclaimed_after_indexing(tmp_path):
    export = tmp_path / 'coverage-export.json'
    export.write_bytes(b'large transient export')
    record = reclaim_indexed_export(export)
    assert record == {
        'path': str(export),
        'sha256': '45eb526b12364f60ebd817b62fc51a8236edce1ccf4d941b87f00cc2e3b81e77',
        'bytes': 22,
        'reclaimed': True,
    }
    assert not export.exists()


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
