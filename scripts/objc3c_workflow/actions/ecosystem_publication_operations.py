"""Long-horizon operations publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_contracts import (
    LONG_HORIZON_OPERATIONS_INTEGRATION_PY,
    LONG_HORIZON_OPERATIONS_PUBLICATION_PY,
)
from .ecosystem_publication_runner import run_publication_action


def action_validate_long_horizon_operations(_: list[str]) -> int:
    return run_publication_action("validate-long-horizon-operations")


def action_publish_long_horizon_operations(_: list[str]) -> int:
    return run_publication_action("publish-long-horizon-operations")


__all__ = [
    "LONG_HORIZON_OPERATIONS_INTEGRATION_PY",
    "LONG_HORIZON_OPERATIONS_PUBLICATION_PY",
    "action_publish_long_horizon_operations",
    "action_validate_long_horizon_operations",
]
