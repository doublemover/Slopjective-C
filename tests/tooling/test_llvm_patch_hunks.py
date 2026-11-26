from __future__ import annotations

import io
import re
import tarfile
from pathlib import Path
from urllib.error import HTTPError

import pytest

from scripts.objc3c_toolchain import check_llvm_patch_application as patch_check


ROOT = Path(__file__).resolve().parents[2]
HUNK = re.compile(
    r"^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@"
)


def test_locked_llvm_patch_hunk_counts_are_exact() -> None:
    """Reject stale hunk metadata before a maintained patch reaches a build."""

    for patch in sorted((ROOT / "toolchains/llvm/patches").glob("*.patch")):
        lines = patch.read_text(encoding="utf-8").splitlines()
        index = 0
        while index < len(lines):
            match = HUNK.match(lines[index])
            if match is None:
                index += 1
                continue

            declared_old = int(match.group(2) or 1)
            declared_new = int(match.group(4) or 1)
            actual_old = 0
            actual_new = 0
            cursor = index + 1
            while cursor < len(lines):
                line = lines[cursor]
                if line.startswith("@@ ") or line.startswith("diff --git "):
                    break
                if line.startswith(" "):
                    actual_old += 1
                    actual_new += 1
                elif line.startswith("-"):
                    actual_old += 1
                elif line.startswith("+"):
                    actual_new += 1
                elif line == r"\ No newline at end of file":
                    pass
                else:
                    break
                cursor += 1

            assert (actual_old, actual_new) == (declared_old, declared_new), (
                f"{patch.relative_to(ROOT)}:{index + 1} declares "
                f"-{declared_old}/+{declared_new} lines but contains "
                f"-{actual_old}/+{actual_new}"
            )
            index = cursor


def test_clang_diagnostic_check_rejects_missing_ids_with_patch_location(tmp_path):
    tables = tmp_path / 'Basic'
    tables.mkdir()
    (tables / 'DiagnosticParseKinds.td').write_text('def err_expected : Error<"expected %0">;\n')
    patch = tmp_path / 'parser.patch'
    patch.write_text('+++ b/clang/lib/Parse/ParseObjc.cpp\n'
                     '+    Diag(Tok, diag::err_expected_ident);\n')
    with pytest.raises(ValueError, match=r'parser\.patch:2: .*uses undefined clang::diag::err_expected_ident'):
        patch_check.check_diagnostic_references([patch], tables)

    patch.write_text('+++ b/clang/lib/Parse/ParseObjc.cpp\n'
                     '+    Diag(Tok, diag::err_expected) << tok::identifier;\n')
    assert patch_check.check_diagnostic_references([patch], tables) == 1


def test_pinned_archive_stages_patch_inputs_and_diagnostic_tables(tmp_path, monkeypatch):
    commit = 'a' * 40
    source = 'clang/lib/Parse/ParseObjc.cpp'
    tables = patch_check.DIAGNOSTIC_ROOT
    entries = {
        source: b'parser source',
        (tables / 'Diagnostic.td').as_posix(): b'include "DiagnosticParseKinds.td"',
        (tables / 'DiagnosticParseKinds.td').as_posix(): b'def err_expected : Error<"expected %0">;',
        'llvm/unused.txt': b'unrelated archive member',
    }
    payload = io.BytesIO()
    with tarfile.open(fileobj=payload, mode='w:gz') as archive:
        for name, contents in entries.items():
            member = tarfile.TarInfo(f'llvm-project-{commit}/{name}')
            member.size = len(contents)
            archive.addfile(member, io.BytesIO(contents))

    def open_url(url, *, timeout):
        assert url == f'https://codeload.github.com/llvm/llvm-project/tar.gz/{commit}'
        return io.BytesIO(payload.getvalue())

    monkeypatch.setattr(patch_check, 'urlopen', open_url)
    patch_check.fetch_upstream_archive(tmp_path, commit, [source])
    assert (tmp_path / source).read_bytes() == entries[source]
    assert sorted(path.name for path in (tmp_path / tables).iterdir()) == ['Diagnostic.td', 'DiagnosticParseKinds.td']
    assert not (tmp_path / 'llvm').exists()
    with pytest.raises(ValueError, match='missing: llvm/missing.cpp'):
        patch_check.fetch_upstream_archive(tmp_path, commit, ['llvm/missing.cpp'])


def test_diagnostic_check_falls_back_on_github_rate_limit(tmp_path, monkeypatch):
    calls = []

    def rate_limited(url, *, timeout):
        calls.append(('raw', url))
        raise HTTPError(url, 429, 'Too Many Requests', None, None)

    def fetch_archive(root, commit, inputs):
        assert root == tmp_path
        assert not inputs
        calls.append(('archive', commit))

    monkeypatch.setattr(patch_check, 'urlopen', rate_limited)
    monkeypatch.setattr(patch_check, 'fetch_upstream_archive', fetch_archive)
    patch_check.fetch_diagnostic_tables(tmp_path, 'pinned-revision')
    assert calls == [
        ('raw', 'https://raw.githubusercontent.com/llvm/llvm-project/pinned-revision/clang/include/clang/Basic/Diagnostic.td'),
        ('archive', 'pinned-revision'),
    ]
