"""Markdown source glob constants for workflow documentation actions."""

from __future__ import annotations

MARKDOWN_GLOBS = [
    "README.md",
    "CONTRIBUTING.md",
    "docs/**/*.md",
    "site/**/*.md",
    "spec/**/*.md",
    "showcase/**/*.md",
    "stdlib/**/*.md",
    "templates/**/*.md",
]


__all__ = ["MARKDOWN_GLOBS"]
