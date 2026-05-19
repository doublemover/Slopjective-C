"""Shared row-level assertions for decision validation contracts."""

from __future__ import annotations

from typing import Sequence

from ..decision_errors import ContractDriftError
from ..decision_validation_violations import OrderingDrift, RowCountDrift
from .catalog import OrderedContractRule


def require_contract_row_count(
    rows: Sequence[object],
    *,
    rule: OrderedContractRule,
) -> None:
    if len(rows) != len(rule.expected_order):
        raise ContractDriftError(
            RowCountDrift(
                rule.row_count_subject,
                len(rule.expected_order),
                len(rows),
            ).message()
        )


def require_unique_id(
    value: str,
    *,
    seen: set[str],
    label: str,
) -> None:
    if value in seen:
        raise ContractDriftError(f"duplicate {label} detected: {value}")
    seen.add(value)


def require_contract_order(
    actual_order: Sequence[str],
    *,
    rule: OrderedContractRule,
) -> None:
    if tuple(actual_order) != rule.expected_order:
        raise ContractDriftError(
            OrderingDrift(
                rule.ordering_subject,
                rule.expected_order,
                actual_order,
            ).message()
        )


__all__ = [
    "require_contract_order",
    "require_contract_row_count",
    "require_unique_id",
]
