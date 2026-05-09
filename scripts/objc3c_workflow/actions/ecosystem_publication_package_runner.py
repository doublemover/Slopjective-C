"""Package-owned runner for package publication actions."""

from __future__ import annotations

from ..commands import run
from .ecosystem_publication_package_contracts import (
    PACKAGE_PUBLICATION_ACTION_CONTRACTS,
)


def run_package_publication_action(action_name: str) -> int:
    contract = PACKAGE_PUBLICATION_ACTION_CONTRACTS[action_name]
    return run(contract.command())


__all__ = ["run_package_publication_action"]
