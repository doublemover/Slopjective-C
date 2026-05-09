"""Shared runner for ecosystem publication workflow actions."""

from __future__ import annotations

from ..commands import run
from .ecosystem_publication_contracts import PUBLICATION_ARTIFACT_CONTRACTS


def run_publication_action(action_name: str, rest: list[str] | None = None) -> int:
    contract = PUBLICATION_ARTIFACT_CONTRACTS[action_name]
    return run(contract.command(rest))


__all__ = ["run_publication_action"]
