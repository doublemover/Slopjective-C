"""Audience classification for public workflow action payloads."""

from __future__ import annotations

MAINTAINER_PREFIXES: tuple[str, ...] = (
    "build-",
    "check-",
    "format-",
    "lint",
    "publish-",
)

OPERATOR_PREFIXES: tuple[str, ...] = (
    "benchmark-",
    "compile-",
    "inspect-",
    "materialize-",
    "package-",
    "proof-",
    "test-",
    "validate-",
)


def action_audience(action: str) -> str:
    if action.startswith("build-native") or action == "build-default":
        return "operator"
    if action.startswith(MAINTAINER_PREFIXES):
        return "maintainer"
    if action.startswith(OPERATOR_PREFIXES):
        return "operator"
    return "operator"
