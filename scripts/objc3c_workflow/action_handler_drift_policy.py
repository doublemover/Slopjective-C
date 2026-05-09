"""Owned drift policy for workflow action handler coverage."""

from __future__ import annotations

from collections.abc import Iterable

ACTION_HANDLER_DRIFT_POLICY_OWNER = "objc3c-workflow-action-handler-drift-policy"
ACTION_HANDLER_COMPLETENESS_POLICY_OWNER = "objc3c-workflow-action-handler-completeness-policy"


def sorted_missing_handlers(catalog_actions: Iterable[str], handler_actions: Iterable[str]) -> list[str]:
    return sorted(set(catalog_actions) - set(handler_actions))


def sorted_orphan_handlers(catalog_actions: Iterable[str], handler_actions: Iterable[str]) -> list[str]:
    return sorted(set(handler_actions) - set(catalog_actions))


def handler_registry_complete(missing: Iterable[str], orphaned: Iterable[str]) -> bool:
    return not list(missing) and not list(orphaned)


__all__ = [
    "ACTION_HANDLER_COMPLETENESS_POLICY_OWNER",
    "ACTION_HANDLER_DRIFT_POLICY_OWNER",
    "handler_registry_complete",
    "sorted_missing_handlers",
    "sorted_orphan_handlers",
]
