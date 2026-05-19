"""CLI entry point for the performance-governance source-surface checker."""

from __future__ import annotations

from .config import default_config
from .runner import run


def main() -> int:
    return run(default_config())
