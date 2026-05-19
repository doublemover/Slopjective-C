from __future__ import annotations

from typing import Iterable


def assert_contains_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet in text


def assert_excludes_all(text: str, snippets: Iterable[str]) -> None:
    for snippet in snippets:
        assert snippet not in text


def assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index
