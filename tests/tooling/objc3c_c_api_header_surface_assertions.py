from __future__ import annotations

import re
from pathlib import Path
from typing import Iterable


def assert_contains_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet in text


def assert_excludes_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet not in text


def assert_regex_present(pattern: str, text: str, flags: int = 0) -> None:
    assert re.search(pattern, text, flags)


def assert_regex_absent(pattern: str, text: str, flags: int = 0) -> None:
    assert not re.search(pattern, text, flags)


def assert_paths_exist(paths: Iterable[Path]) -> None:
    for path in paths:
        assert path.exists(), path


def assert_paths_absent(paths: Iterable[Path]) -> None:
    for path in paths:
        assert not path.exists()


def assert_forbidden_terms_absent(
    path: Path,
    text: str,
    forbidden_terms: Iterable[str],
) -> None:
    for term in forbidden_terms:
        assert term not in text, f"{term!r} remains in {path}"
