"""Audience prefix tables for public workflow actions."""

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


__all__ = ["MAINTAINER_PREFIXES", "OPERATOR_PREFIXES"]
