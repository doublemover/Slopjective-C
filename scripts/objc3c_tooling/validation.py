"""Small validation predicates shared by repository tooling scripts."""

from __future__ import annotations

from collections.abc import Sequence


def contains_all(text: str, tokens: Sequence[str]) -> dict[str, bool]:
    return {token: token in text for token in tokens}

