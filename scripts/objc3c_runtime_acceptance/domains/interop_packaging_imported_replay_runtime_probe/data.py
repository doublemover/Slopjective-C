"""Imported-runtime replay probe assertion data."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    ImportedRuntimeStartupDispatchValues,
)


@dataclass(frozen=True)
class PayloadEqualityExpectation:
    key: str
    expected: Any
    message: str


@dataclass(frozen=True)
class PayloadMinimumExpectation:
    key: str
    minimum: int
    default: int
    message: str


@dataclass(frozen=True)
class PayloadStatusFlagExpectation:
    status_key: str
    flag_key: str
    message: str


@dataclass(frozen=True)
class PayloadResolvedMethodExpectation:
    status_key: str
    found_key: str
    resolved_key: str
    message: str


@dataclass(frozen=True)
class StartupDispatchReplayExpectation:
    key: str
    startup_value: int
    expected: int
    message: str


@dataclass(frozen=True)
class ImportedRuntimeReplayProbeAssertionData:
    link_plan: dict[str, Any]
    provider_identity: str
    consumer_identity: str
    startup_values: ImportedRuntimeStartupDispatchValues
    translation_unit_expectations: tuple[PayloadEqualityExpectation, ...]
    startup_dispatch_expectations: tuple[StartupDispatchReplayExpectation, ...]


__all__ = [
    "ImportedRuntimeReplayProbeAssertionData",
    "PayloadEqualityExpectation",
    "PayloadMinimumExpectation",
    "PayloadResolvedMethodExpectation",
    "PayloadStatusFlagExpectation",
    "StartupDispatchReplayExpectation",
]
