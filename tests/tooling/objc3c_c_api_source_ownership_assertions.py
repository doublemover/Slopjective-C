from __future__ import annotations

from pathlib import Path
from typing import Iterable


def assert_contains_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet in text


def assert_excludes_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet not in text


def assert_paths_exist(paths: Iterable[Path]) -> None:
    for path in paths:
        assert path.exists(), path


def assert_contract_collection_in_text(
    values: Iterable[str],
    text: str,
    *,
    prefix: str = "",
    suffix: str = "",
) -> None:
    for value in values:
        assert f"{prefix}{value}{suffix}" in text
