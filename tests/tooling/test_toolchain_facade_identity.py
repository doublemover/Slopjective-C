from pathlib import Path
import os
import json

from scripts.objc3c_toolchain.facade import facade_identity


def source_repo(tmp_path: Path) -> Path:
    for name in ('native/objc3c/CMakeLists.txt', 'scripts/objc3c_toolchain/facade.py',
                 'native/objc3c/src/clang_foundation/compile_session.cpp',
                 'native/objc3c/src/runtime/public/objc3_runtime_services.h',
                 'native/objc3c/src/runtime/public/objc3_task.hpp',
                 'native/objc3c/src/runtime/public/objc3_keypath.h'):
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('original input\n')
    catalog = tmp_path / 'stdlib/module_inventory.json'
    header = tmp_path / 'stdlib/include/objc3/core.h'
    header.parent.mkdir(parents=True)
    header.write_text('original input\n')
    catalog.write_text(json.dumps({'canonical_modules': [
        {'module': 'objc3.core', 'header': 'include/objc3/core.h'}]}))
    return tmp_path


def test_same_size_same_timestamp_source_change_invalidates_frontend_identity(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    source = repo / 'native/objc3c/src/clang_foundation/compile_session.cpp'
    stamp = source.stat()
    source.write_text('modified input\n')
    os.utime(source, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert facade_identity(repo)['sha256'] != before['sha256']


def test_header_membership_and_cmake_changes_invalidate_frontend_identity(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    header = repo / 'native/objc3c/src/clang_foundation/new_contract.h'
    header.write_text('new declaration\n')
    after = facade_identity(repo)
    assert after['sha256'] != before['sha256']
    (repo / 'native/objc3c/CMakeLists.txt').write_text('new compiler flags\n')
    assert facade_identity(repo)['sha256'] != after['sha256']


def test_documentation_does_not_recompile_frontend(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    (repo / 'native/objc3c/src/clang_foundation/README.md').write_text('documentation update\n')
    assert facade_identity(repo) == before


def test_runtime_type_contract_invalidates_its_frontend_provenance(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    header = repo / 'native/objc3c/src/runtime/public/objc3_runtime_services.h'
    stamp = header.stat()
    header.write_text('modified input\n')
    os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert facade_identity(repo)['sha256'] != before['sha256']


def test_embedded_runtime_header_template_invalidates_frontend_identity(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    template = repo / 'native/objc3c/src/clang_foundation/runtime_interface_header.h.in'
    template.write_text('embedded header template\n')
    assert facade_identity(repo)['sha256'] != before['sha256']


def test_sdk_library_contract_headers_invalidate_frontend_identity(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    header = repo / 'stdlib/include/objc3/core.h'
    stamp = header.stat()
    header.write_text('modified input\n')
    os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    changed = facade_identity(repo)
    assert changed['sha256'] != before['sha256']
    assert 'stdlib/include/objc3/core.h' in changed['inputs']
    catalog = repo / 'stdlib/module_inventory.json'
    data = json.loads(catalog.read_text())
    data['canonical_modules'][0]['module'] = 'objc3.changed'
    catalog.write_text(json.dumps(data))
    assert facade_identity(repo)['sha256'] != changed['sha256']


def test_cpp_task_header_invalidates_embedded_interface_runtime(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    header = repo / 'native/objc3c/src/runtime/public/objc3_task.hpp'
    stamp = header.stat()
    header.write_text('modified input\n')
    os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    assert facade_identity(repo)['sha256'] != before['sha256']


def test_keypath_header_invalidates_embedded_interface_runtime(tmp_path):
    repo = source_repo(tmp_path)
    before = facade_identity(repo)
    header = repo / 'native/objc3c/src/runtime/public/objc3_keypath.h'
    stamp = header.stat()
    header.write_text('modified input\n')
    os.utime(header, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    changed = facade_identity(repo)
    assert changed['sha256'] != before['sha256']
    assert 'native/objc3c/src/runtime/public/objc3_keypath.h' in changed['inputs']
