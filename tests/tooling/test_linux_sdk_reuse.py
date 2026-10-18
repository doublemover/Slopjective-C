from pathlib import Path
import os

from scripts.objc3c_toolchain.linux_sdk import foundation_identity


def sdk(tmp_path: Path) -> Path:
    for name, content in {'bin/clang': 'compiler', 'lib/libclangAST.a': 'native AST',
                          'include/clang/AST/Type.h': 'native types',
                          'bin/objc3c': 'frontend', 'bin/clang-scan-deps': 'scanner',
                          'lib/libobjc.so': 'runtime'}.items():
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
    return tmp_path


def test_project_updates_reuse_the_same_llvm_foundation(tmp_path):
    prefix = sdk(tmp_path)
    before = foundation_identity(prefix)
    for name in ('bin/objc3c', 'bin/clang-scan-deps', 'lib/libobjc.so'):
        (prefix / name).write_text('updated project component')
    assert foundation_identity(prefix) == before


def test_llvm_library_change_invalidates_reuse_even_with_preserved_timestamp(tmp_path):
    prefix = sdk(tmp_path)
    before = foundation_identity(prefix)
    library = prefix / 'lib/libclangAST.a'
    prior = library.stat()
    library.write_bytes(b'X' * prior.st_size)
    os.utime(library, ns=(prior.st_atime_ns, prior.st_mtime_ns))
    assert foundation_identity(prefix) != before
