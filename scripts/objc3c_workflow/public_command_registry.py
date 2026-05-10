"""Public workflow registry read helpers."""

from __future__ import annotations

from collections.abc import Iterable


def public_workflow_action_names() -> list[str]:
    from .registry_views import action_names

    return action_names()


def public_workflow_action_count() -> int:
    from .registry_views import action_count

    return action_count()


def public_workflow_action_identifiers() -> set[str]:
    identifiers: set[str] = set()
    for action in public_workflow_action_names():
        function_stem = action.replace("-", "_")
        identifiers.add(action)
        identifiers.add(function_stem)
        identifiers.add(f"action_{function_stem}")
    return identifiers


def public_workflow_has_actions(actions: Iterable[str]) -> bool:
    available = set(public_workflow_action_names())
    return all(action in available for action in actions)


def public_workflow_has_action_identifiers(identifiers: Iterable[str]) -> bool:
    available = public_workflow_action_identifiers()
    return all(identifier in available for identifier in identifiers)


__all__ = [
    "public_workflow_action_count",
    "public_workflow_action_identifiers",
    "public_workflow_action_names",
    "public_workflow_has_action_identifiers",
    "public_workflow_has_actions",
]
