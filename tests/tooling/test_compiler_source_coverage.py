"""Patch-owned compiler coverage cannot silently shrink its denominator."""
import json
import os
from pathlib import Path
import subprocess
from zipfile import ZipFile

import pytest

from scripts.objc3c_toolchain.compiler_source_coverage import (
    EXPORT_FLAGS, FLAGS, PROFILE, SCHEMA, changed_branches, compare_coverage, compile_inventory, coverage_baseline,
    inspect_objects, lit_inventory, merged_ranges, patched_inventory, read_export,
    require_binary_ids, select_coverage_profiles, unique_binary_paths,
)
from scripts.objc3c_toolchain.source_coverage import COVERAGE_SECTIONS, coverage_section_command
from scripts.objc3c_toolchain.binary_packages import digest


def fixture(tmp_path):
    repo, source, build = tmp_path / 'repo', tmp_path / 'source', tmp_path / 'build'
    patch = repo / 'toolchains/llvm/patches/001.patch';patch.parent.mkdir(parents=True)
    unit = source / 'clang/lib/Sema/SemaObjC3.cpp';unit.parent.mkdir(parents=True)
    unit.write_text('int a;\nint f() { return a; }\nint g() { return f(); }\n')
    header = source / 'clang/include/clang/Sema/ObjC3.h';header.parent.mkdir(parents=True)
    header.write_text('int f();\n')
    patch.write_text('--- a/clang/lib/Sema/SemaObjC3.cpp\n+++ b/clang/lib/Sema/SemaObjC3.cpp\n'
        '@@ -1,1 +1,2 @@\n+ int a;\n+int f();\n@@ -3,1 +3,1 @@\n-int x;\n+int g();\n'
        '--- a/clang/include/clang/Sema/ObjC3.h\n+++ b/clang/include/clang/Sema/ObjC3.h\n@@ -1,0 +1,1 @@\n+int f();\n')
    build.mkdir();obj = build / 'unit.o';obj.write_bytes(b'covered object')
    argv = ['clang++', *FLAGS, '-c', str(unit), '-o', str(obj)]
    (build / 'compile_commands.json').write_text(json.dumps([{
        'directory': str(build), 'file': str(unit), 'arguments': argv, 'output': str(obj)}]))
    return repo, source, build, unit


def test_patch_inventory_merges_hunks_and_binds_every_implementation_object(tmp_path):
    repo, source, build, unit = fixture(tmp_path)
    eligible, changed = patched_inventory(repo, source)
    assert set(eligible) == {'clang/include/clang/Sema/ObjC3.h', 'clang/lib/Sema/SemaObjC3.cpp'}
    assert changed['clang/lib/Sema/SemaObjC3.cpp'] == [[1, 3]]
    rows = compile_inventory(build, source, eligible)
    assert [row['source'] for row in rows] == ['clang/lib/Sema/SemaObjC3.cpp']
    document = json.loads((build / 'compile_commands.json').read_text())
    document[0]['arguments'].remove('-fcoverage-mapping')
    (build / 'compile_commands.json').write_text(json.dumps(document))
    with pytest.raises(ValueError, match='lacks source coverage'):
        compile_inventory(build, source, eligible)


def test_export_retains_zero_hit_files_and_changed_region_evidence(tmp_path):
    repo, source, build, unit = fixture(tmp_path)
    eligible, changed = patched_inventory(repo, source)
    summary = {name: {'count': 2, 'covered': 1} for name in ('functions', 'regions', 'branches', 'lines')}
    export = tmp_path / 'export.json'
    export.write_text(json.dumps({'type': 'llvm.coverage.json.export', 'data': [{'files': [{
        'filename': str(unit), 'summary': summary,
        'segments': [[1, 1, 0, True, True], [2, 1, 3, True, True], [3, 1, 0, True, True],
                     [2, 2, 0, True, True, True]],
        'branches': [[2, 3, 2, 8, 3, 0, 0, 0, 4]]}]}]}))
    rows = read_export(export, source=source, eligible=eligible, changed=changed)
    key = 'clang/lib/Sema/SemaObjC3.cpp'
    assert rows[key]['state'] == 'executed'
    assert rows[key]['covered_changed_region_entries'] == 1
    assert rows[key]['uncovered_changed_lines'] == [1, 3]
    assert rows[key]['changed_branches'] == {'arms': 2, 'covered_arms': 1,
        'decisions': [{'start': [2, 3], 'end': [2, 8], 'true_count': 3, 'false_count': 0}]}
    assert rows['clang/include/clang/Sema/ObjC3.h']['state'] == 'no-emitted-mapping'
    assert rows['clang/include/clang/Sema/ObjC3.h']['changed_branches'] is None


def test_changed_branch_span_can_enter_the_hunk_after_its_first_line():
    branches = [[1, 2, 2, 3, 2, 3, 0, 0, 4], [5, 1, 5, 2, 0, 1, 0, 0, 4]]
    result = changed_branches(branches, [[2, 3]])
    assert result == {'arms': 2, 'covered_arms': 2, 'decisions': [
        {'start': [1, 2], 'end': [2, 3], 'true_count': 2, 'false_count': 3}]}
    for invalid in ([2, 1, 1, 2, 1, 2, 0, 0, 4], [1, 1, 2, 2, -1, 0, 0, 0, 4]):
        with pytest.raises(ValueError, match='coverage branch'):
            changed_branches([invalid], [[2, 3]])


def test_baseline_compares_hits_and_exposes_denominator_changes():
    def row(hits, *, count=2, column=2):
        return {'summary': {name: {'count': count, 'covered': hits}
                            for name in ('functions', 'regions', 'branches', 'lines')},
                'changed_lines': [[1, 3]], 'changed_region_entries': 2,
                'covered_changed_region_entries': hits,
                'changed_branches': changed_branches([[2, 1, 2, column, 1, hits - 1, 0, 0, 4]], [[1, 3]])}

    baseline = {'schema': SCHEMA, 'profile': PROFILE, 'status': 'PASS', 'revision': 'a' * 40,
                'host': 'linux-x86_64', 'coverage_owner': 'shared-clang-cpp-patched-implementation',
                'eligible': {'old.cpp': 'same', 'different.cpp': 'before', 'same.cpp': 'same', 'shape.cpp': 'same'},
                'source_coverage': {name: row(1) for name in ('old.cpp', 'different.cpp', 'same.cpp', 'shape.cpp')}}
    current = {'revision': 'b' * 40, 'host': 'linux-x86_64',
               'eligible': {'new.cpp': 'same', 'different.cpp': 'after', 'same.cpp': 'same', 'shape.cpp': 'same'},
               'source_coverage': {'new.cpp': row(2), 'different.cpp': row(2), 'same.cpp': row(2),
                                   'shape.cpp': row(2, column=3)}}
    result = compare_coverage(baseline, current)
    assert result['added_files'] == ['new.cpp'] and result['removed_files'] == ['old.cpp']
    assert result['changed_files'] == {'different.cpp': 'source-bytes', 'shape.cpp': 'emitted-mapping'}
    assert result['comparable_files'] == ['same.cpp']
    assert result['hit_deltas']['same.cpp']['changed_region_entries'] == 1
    assert result['hit_deltas']['same.cpp']['changed_branch_arms'] == 1
    assert result['gained_branch_arms'] == {'same.cpp': [
        {'start': [2, 1], 'end': [2, 2], 'arm': 'false', 'occurrences': 1}]}
    assert result['lost_branch_arms'] == {}
    del baseline['source_coverage']['same.cpp']['changed_branches']
    result = compare_coverage(baseline, current)
    assert result['missing_branch_baseline'] == ['same.cpp']
    assert 'changed_branch_arms' not in result['hit_deltas']['same.cpp']
    with pytest.raises(ValueError, match='complete patched-compiler report'):
        compare_coverage({**baseline, 'host': 'linux-aarch64'}, current)


def test_baseline_reports_a_lost_branch_arm_with_the_same_aggregate():
    name = 'clang/lib/Sema/SemaObjC3.cpp'
    ranges = [[2, 2]]
    old = changed_branches([[2, 1, 2, 4, 10, 0, 0, 0, 4]], ranges)
    new = changed_branches([[2, 1, 2, 4, 0, 15, 0, 0, 4]], ranges)
    row = {'summary': {kind: {'count': 2, 'covered': 1}
                       for kind in ('functions', 'regions', 'branches', 'lines')},
           'changed_lines': ranges, 'changed_region_entries': 1,
           'covered_changed_region_entries': 1}
    baseline = {'schema': SCHEMA, 'profile': PROFILE, 'status': 'PASS', 'revision': 'a' * 40,
                'host': 'linux-x86_64', 'coverage_owner': 'shared-clang-cpp-patched-implementation',
                'eligible': {name: 'same'}, 'source_coverage': {name: {**row, 'changed_branches': old}}}
    current = {**baseline, 'revision': 'b' * 40, 'source_coverage': {name: {**row, 'changed_branches': new}}}
    result = compare_coverage(baseline, current)
    assert result['comparable_files'] == [name]
    assert result['hit_deltas'] == {}
    point = {'start': [2, 1], 'end': [2, 4], 'occurrences': 1}
    assert result['lost_branch_arms'] == {name: [{**point, 'arm': 'true'}]}
    assert result['gained_branch_arms'] == {name: [{**point, 'arm': 'false'}]}


@pytest.mark.parametrize('defect', [None, 'missing', 'duplicate', 'missing-reason', 'unexpected-result'])
def test_coverage_lit_requires_every_discovered_case_and_actual_outcomes(defect):
    source = '-- Available Tests --\n  Clang :: ObjC3/a.m\n  Clang :: ObjC3/b.m\n'
    rows = [{'name': 'Clang :: ObjC3/a.m', 'code': 'PASS'},
            {'name': 'Clang :: ObjC3/b.m', 'code': 'UNSUPPORTED', 'output': 'Needs an unavailable target'}]
    if defect == 'missing':
        rows.pop()
    elif defect == 'duplicate':
        rows[1] = rows[0]
    elif defect == 'missing-reason':
        del rows[1]['output']
    elif defect == 'unexpected-result':
        rows[1]['code'] = 'UNRESOLVED'
    if defect is None:
        assert lit_inventory(source, {'tests': rows}) == {'PASS': 1, 'XFAIL': 0, 'UNSUPPORTED': 1}
    else:
        with pytest.raises(ValueError, match='lit execution|lit suite'):
            lit_inventory(source, {'tests': rows})


def test_reviewed_baseline_binds_archive_receipt_and_architecture(tmp_path):
    report = {'revision': 'a'*40, 'covered_binary_ids': {'/sdk/libclang.so': 'b'*40}}
    receipt = tmp_path / 'receipt.json'
    receipt.write_text(json.dumps(report))
    archive = tmp_path / 'known.zip'
    name = 'source-coverage/compiler/qualification.json'
    with ZipFile(archive, 'w') as zipped:
        zipped.write(receipt, name)
    rule = {'archive_sha256': digest(archive), 'receipt_sha256': digest(receipt),
            'shared_clang_build_id': 'b'*40, 'run_id': 1, 'artifact_id': 2}
    policy = {'schema': 'objc3c.reviewed-source-coverage-baselines.v1', 'revision': 'a'*40,
              'compiler_receipt': name, 'hosts': {'linux-x86_64': rule}}
    config = tmp_path / 'toolchains/llvm/source-coverage-baselines.json'
    config.parent.mkdir(parents=True)
    config.write_text(json.dumps(policy))
    admitted, provenance = coverage_baseline(tmp_path, archive)
    assert admitted == {**report, 'host': 'linux-x86_64'}
    assert provenance['artifact_id'] == 2 and provenance['sha256'] == digest(archive)
    archive.write_bytes(archive.read_bytes()+b'altered')
    with pytest.raises(ValueError, match='reviewed release artifact'):
        coverage_baseline(tmp_path, archive)
    rule['archive_sha256'] = digest(archive)
    rule['receipt_sha256'] = 'd'*64
    config.write_text(json.dumps(policy))
    with pytest.raises(ValueError, match='different release receipt'):
        coverage_baseline(tmp_path, archive)


def test_object_instrumentation_is_verified_even_if_database_flags_stay_identical(tmp_path):
    bootstrap = os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT')
    if not bootstrap:
        pytest.skip('requires the pinned LLVM bootstrap')
    tools = Path(bootstrap) / 'bin'
    repo, source, build, unit = fixture(tmp_path)
    eligible, _ = patched_inventory(repo, source)
    obj = build / 'unit.o'

    def compile_unit(*flags):
        subprocess.run([tools / 'clang++', *flags, '-c', unit, '-o', obj], check=True, timeout=30)
        return compile_inventory(build, source, eligible)

    def sections(_, path):
        return subprocess.check_output(coverage_section_command(tools / 'llvm-objdump', path), text=True, timeout=30)

    records = compile_unit(*FLAGS)
    inspect_objects(records, sections)
    assert records[0]['coverage_sections'] == list(COVERAGE_SECTIONS)
    records = compile_unit()
    assert set(FLAGS) <= set(records[0]['argv'])
    with pytest.raises(ValueError, match='SemaObjC3.cpp: actual object has no coverage counters/mapping'):
        inspect_objects(records, sections)


def test_changed_range_normalization_is_stable():
    assert merged_ranges([[7, 9], [1, 2], [3, 5], [9, 12]]) == [[1, 5], [7, 12]]


def test_export_omits_unused_global_function_table():
    assert EXPORT_FLAGS == ('--check-binary-ids', '--skip-expansions', '--skip-functions')


def test_profile_ids_require_mains_and_reject_foreign_modules():
    require_binary_ids({'clang', 'llvm', 'clang-cpp'}, required={'clang'},
                       allowed={'clang', 'llvm', 'clang-cpp'}, kind='profile')
    for observed in ({'llvm', 'clang-cpp'}, {'clang', 'llvm', 'foreign'}):
        with pytest.raises(ValueError, match='binary identities'):
            require_binary_ids(observed, required={'clang'},
                               allowed={'clang', 'llvm', 'clang-cpp'}, kind='profile')


def test_export_uses_one_object_per_admitted_build_identity(tmp_path):
    clang, installed_clang, support = (tmp_path / name for name in ('clang', 'installed-clang', 'support'))
    identities = {str(clang): 'compiler', str(installed_clang): 'compiler', str(support): 'support'}
    assert unique_binary_paths([clang, installed_clang, support], identities) == [clang, support]
    with pytest.raises(ValueError, match='admitted identity'):
        unique_binary_paths([tmp_path / 'foreign'], identities)


def test_support_tool_profiles_are_excluded_from_compiler_denominator(tmp_path):
    lit = tmp_path / 'objc3-lit';unit = tmp_path / 'unit';lit.mkdir();unit.mkdir()
    clang, library, support = (lit / name for name in ('clang.profraw', 'library.profraw', 'support.profraw'))
    unit_clang = unit / 'clang.profraw'
    selected, excluded = select_coverage_profiles(
        {clang: 'clang', library: 'library', support: 'support', unit_clang: 'clang'},
        {'clang', 'library'}, suite='objc3-lit')
    assert selected == [clang, library]
    assert excluded == [support, unit_clang]
    with pytest.raises(ValueError, match='binary identities'):
        select_coverage_profiles({clang: 'clang', support: 'support'},
                                 {'clang', 'library'}, suite='objc3-lit')


def test_successive_patch_insertions_and_deletions_keep_final_coverage_coordinates(tmp_path):
    from scripts.objc3c_toolchain.compiler_source_coverage import remap_patch_ranges
    # Prefix insertion shifts old ownership, while its new lines also belong
    # to the maintained stack. This must include a previously owned final line.
    assert remap_patch_ranges([[3,5]],[(0,0,1,2)]) == [[1,2],[5,7]]
    assert remap_patch_ranges([[1,10]],[(3,3,3,0)]) == [[1,7]]
    assert remap_patch_ranges([[2,8]],[(3,2,3,4),(7,1,9,0)]) == [[2,9]]
    repo, source, _, unit = fixture(tmp_path)
    unit.write_text('prefix1\nprefix2\nint a;\nint f(){return a;}\nint g(){return f();}\n')
    later = repo / 'toolchains/llvm/patches/002.patch'
    later.write_text('--- a/clang/lib/Sema/SemaObjC3.cpp\n+++ b/clang/lib/Sema/SemaObjC3.cpp\n'
                     '@@ -0,0 +1,2 @@\n+prefix1\n+prefix2\n')
    _, ranges = patched_inventory(repo,source)
    assert ranges['clang/lib/Sema/SemaObjC3.cpp'] == [[1,5]]
