from __future__ import annotations

from pathlib import Path
from typing import Iterable

from objc3c_native_static_owner_contracts_sources import repo_relative


def assert_contains_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet in text


def assert_excludes_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet not in text


def assert_same_paths(actual: Iterable[Path], expected: Iterable[Path]) -> None:
    assert set(actual) == set(expected)


def assert_header_excludes_include_shards(header_path: Path, header_text: str) -> None:
    assert ".inc" not in header_text, repo_relative(header_path)
