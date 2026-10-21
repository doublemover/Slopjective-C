from pathlib import Path
import os

from scripts.objc3c_toolchain.facade import facade_identity


def source_repo(tmp_path: Path) -> Path:
    for name in ('native/objc3c/CMakeLists.txt', 'scripts/objc3c_toolchain/facade.py',
                 'native/objc3c/src/clang_foundation/compile_session.cpp'):
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('original input\n')
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
