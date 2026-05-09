"""Audience prefix tables for public workflow actions."""

from __future__ import annotations


OPERATOR_EXACT_ACTIONS: tuple[str, ...] = (
    "build-default",
)

MAINTAINER_PREFIXES: tuple[str, ...] = (
    "build-",
    "check-",
    "format-",
    "lint",
    "publish-",
)

OPERATOR_PREFIXES: tuple[str, ...] = (
    "benchmark-",
    "build-native-",
    "compile-",
    "inspect-",
    "materialize-",
    "package-",
    "proof-",
    "test-",
    "validate-",
)


__all__ = ["MAINTAINER_PREFIXES", "OPERATOR_EXACT_ACTIONS", "OPERATOR_PREFIXES"]
