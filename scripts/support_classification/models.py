"""Support classification report models."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


class ContractError(RuntimeError):
    pass


@dataclass(frozen=True)
class SupportClassificationReport:
    summary: dict[str, Any]
    markdown: str
    json_report: str
    console_json: str
    status: str


@dataclass(frozen=True)
class SupportClassificationSource:
    contract: dict[str, Any]
    contract_path: str
    contract_sha256: str


__all__ = [
    "ContractError",
    "SupportClassificationReport",
    "SupportClassificationSource",
]
