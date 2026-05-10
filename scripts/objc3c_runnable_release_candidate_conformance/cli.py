"""CLI entry point for runnable release-candidate conformance."""

from __future__ import annotations

from .orchestration import run_release_candidate_conformance


def main() -> int:
    run_release_candidate_conformance()
    return 0


__all__ = ["main"]
