"""Shared constants for open-blocker audit scope handling."""

from __future__ import annotations

import re


DEFAULT_EXCLUDE_PATHS: tuple[str, ...] = (
    ".git/**",
    ".github/**",
    ".pytest_cache/**",
    "node_modules/**",
    "reports/**",
    "tests/**",
    "tmp/**",
)

ISO_UTC_SECOND_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$")
WILDCARD_CHARS = set("*?[")


__all__ = (
    "DEFAULT_EXCLUDE_PATHS",
    "ISO_UTC_SECOND_RE",
    "WILDCARD_CHARS",
)
