"""Patch-owned compiler coverage cannot silently shrink its denominator."""
import json
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.compiler_source_coverage import (
    FLAGS, compile_inventory, merged_ranges, patched_inventory, read_export,
    require_binary_ids,
)


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
        'segments': [[1, 1, 0, True, True], [2, 1, 3, True, True], [3, 1, 0, True, True]]}]}]}))
    rows = read_export(export, source=source, eligible=eligible, changed=changed)
    key = 'clang/lib/Sema/SemaObjC3.cpp'
    assert rows[key]['state'] == 'executed'
    assert rows[key]['covered_changed_region_entries'] == 1
    assert rows[key]['uncovered_changed_lines'] == [1, 3]
    assert rows['clang/include/clang/Sema/ObjC3.h']['state'] == 'no-emitted-mapping'


def test_changed_range_normalization_is_stable():
    assert merged_ranges([[7, 9], [1, 2], [3, 5], [9, 12]]) == [[1, 5], [7, 12]]


def test_profile_ids_require_mains_and_reject_foreign_modules():
    require_binary_ids({'clang', 'llvm', 'clang-cpp'}, required={'clang'},
                       allowed={'clang', 'llvm', 'clang-cpp'}, kind='profile')
    for observed in ({'llvm', 'clang-cpp'}, {'clang', 'llvm', 'foreign'}):
        with pytest.raises(ValueError, match='binary identities'):
            require_binary_ids(observed, required={'clang'},
                               allowed={'clang', 'llvm', 'clang-cpp'}, kind='profile')
