"""Shared validation contracts, constants, and error model."""

from __future__ import annotations

from collections.abc import Callable
from typing import Any
from typing import TypedDict

from ..constants import CANONICAL_SUPPORT_CLASSES
from ..models import ContractError

PathExists = Callable[[str], bool]
Contract = dict[str, Any]
ClassificationRecord = dict[str, Any]
TriggerRecord = dict[str, Any]


class SupportClassPolicy(TypedDict):
    claim_class: str
    fail_closed: bool
    release_blocking: bool


__all__ = [
    "CANONICAL_SUPPORT_CLASSES",
    "ClassificationRecord",
    "Contract",
    "ContractError",
    "PathExists",
    "SupportClassPolicy",
    "TriggerRecord",
]
