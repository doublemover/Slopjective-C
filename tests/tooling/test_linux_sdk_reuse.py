from pathlib import Path
import os

from scripts.objc3c_toolchain.linux_sdk import copy_foundation, foundation_identity


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


def test_cached_foundation_cannot_restore_removed_project_components(tmp_path):
    prefix = sdk(tmp_path / 'cached')
    stale = ('bin/objc3c-clang', 'bin/objc3-managed-sample.o',
             'lib/libobjc3c_removed.a', 'lib/cmake/objc3c/Removed.cmake',
             'include/objc/retired.h', 'include/objc3c/retired.h',
             'include/runtime/retired.h', 'share/objc3c/examples/retired.m',
             'objc3-sdk.json')
    for name in stale:
        path = prefix / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('removed project component')
    (prefix / 'bin/clang++').symlink_to('clang')
    output = tmp_path / 'candidate'
    copy_foundation(prefix, output)
    for name in (*stale, 'bin/objc3c', 'bin/clang-scan-deps', 'lib/libobjc.so'):
        assert not (output / name).exists()
    assert (output / 'lib/libclangAST.a').read_text() == 'native AST'
    assert (output / 'bin/clang++').is_symlink()
    assert (output / 'bin/clang++').read_text() == 'compiler'


def test_editor_qualification_changes_invalidate_release_reuse(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import linux_sdk
    from scripts.objc3c_native_validation import plan
    repo = tmp_path / 'project'
    fixture = 'tests/tooling/fixtures/objc3_editor/qualification-cases.json'
    for name in ('scripts/install_objc3c_linux.sh',
                 'out/toolchains/objc3-development/build-linux-x86_64/bin/llvm-tblgen', fixture):
        path = repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('initial contents')
    monkeypatch.setattr(linux_sdk, 'build_plans', lambda *a, **kw: {'configure': {'identity': 'foundation'}})
    monkeypatch.setattr(linux_sdk, 'facade_identity', lambda *a: {'sha256': 'frontend'})
    monkeypatch.setattr(plan, 'build_plan', lambda *a, **kw: {
        'selected_nodes': [{'identity_inputs': [fixture]}]})
    prefix = sdk(tmp_path / 'sdk')
    before = linux_sdk.input_identity(repo, prefix)['sha256']
    (repo / fixture).write_text('updated qualification expectations')
    assert linux_sdk.input_identity(repo, prefix)['sha256'] != before
