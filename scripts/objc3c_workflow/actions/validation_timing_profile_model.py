"""Validation timing profile rule model helpers."""

from __future__ import annotations

from collections.abc import Sequence

ValidationProfileRule = dict[str, object]


def profile_rule(
    *,
    path_prefixes: Sequence[str],
    recommended_actions: Sequence[str],
    exhaustive_actions: Sequence[str],
    skipped_by_default: Sequence[str],
) -> ValidationProfileRule:
    return {
        "path_prefixes": tuple(path_prefixes),
        "recommended_actions": tuple(recommended_actions),
        "exhaustive_actions": tuple(exhaustive_actions),
        "skipped_by_default": tuple(skipped_by_default),
    }
