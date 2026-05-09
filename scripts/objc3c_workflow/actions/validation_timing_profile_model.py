"""Validation timing profile rule model helpers."""

from __future__ import annotations

from collections.abc import Sequence

ValidationProfileRule = dict[str, object]


def profile_rule(
    *,
    path_prefixes: Sequence[str],
    recommended_actions: Sequence[str],
    exhaustive_actions: Sequence[str],
    deferred_actions: Sequence[str],
    profile_owner: str,
    source_owner: str,
    hard_blocking_decision_owner: str,
) -> ValidationProfileRule:
    return {
        "path_prefixes": tuple(path_prefixes),
        "recommended_actions": tuple(recommended_actions),
        "exhaustive_actions": tuple(exhaustive_actions),
        "deferred_actions": tuple(deferred_actions),
        "profile_owner": profile_owner,
        "source_owner": source_owner,
        "hard_blocking_decision_owner": hard_blocking_decision_owner,
    }
