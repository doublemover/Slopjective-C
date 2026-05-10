"""Validation violation message models for quality-gate contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class SchemaViolation:
    missing: Sequence[str]
    unexpected: Sequence[str]

    def message(self, *, context: str) -> str:
        detail_parts: list[str] = []
        if self.missing:
            detail_parts.append(f"missing keys {list(self.missing)}")
        if self.unexpected:
            detail_parts.append(f"unexpected keys {list(self.unexpected)}")
        details = "; ".join(detail_parts)
        return f"{context}: schema mismatch ({details})"


@dataclass(frozen=True)
class RowCountDrift:
    subject: str
    expected: int
    found: int

    def message(self) -> str:
        return (
            f"{self.subject} row-count drift: "
            f"expected {self.expected}, found {self.found}"
        )


@dataclass(frozen=True)
class OrderingDrift:
    subject: str
    expected: Sequence[str]
    found: Sequence[str]

    def message(self) -> str:
        return f"{self.subject}: expected {list(self.expected)}, found {list(self.found)}"


@dataclass(frozen=True)
class FieldDrift:
    entity_id: str
    field_name: str
    expected: object
    found: object
    quote_values: bool = False

    def message(self) -> str:
        return (
            f"{self.entity_id} {self.field_name} drift: "
            f"expected {self._format(self.expected)}, found {self._format(self.found)}"
        )

    def _format(self, value: object) -> str:
        if self.quote_values:
            return repr(value)
        return str(value)


__all__ = [
    "FieldDrift",
    "OrderingDrift",
    "RowCountDrift",
    "SchemaViolation",
]
