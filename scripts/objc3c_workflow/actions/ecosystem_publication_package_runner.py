"""Package-owned runner for package publication actions."""

from __future__ import annotations

from ..commands import run
from .ecosystem_publication_package_contracts import (
    PACKAGE_PUBLICATION_ACTION_CONTRACTS,
)
from .ecosystem_publication_owner_contracts import (
    require_ecosystem_publication_owner_contract,
)


def run_package_publication_action(action_name: str, rest: list[str] | None = None) -> int:
    require_ecosystem_publication_owner_contract(action_name)
    contract = PACKAGE_PUBLICATION_ACTION_CONTRACTS[action_name]
    return run([*contract.command(), *(rest or [])])


__all__ = ["run_package_publication_action"]
