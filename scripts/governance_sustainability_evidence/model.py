"""Public model types for governance sustainability evidence."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping


JsonObject = dict[str, Any]


@dataclass(frozen=True)
class GovernanceSustainabilityEvidenceInputs:
    contract: Mapping[str, Any]
    waiver_registry: Mapping[str, Any]
    generated_reports: list[str]
    report_payloads: Mapping[str, JsonObject]
    artifact_contract_path: str
    evidence_artifact_path: str


@dataclass(frozen=True)
class GovernanceSustainabilityEvidencePayloads:
    evidence: JsonObject
    summary: JsonObject
    failures: list[str]


__all__ = [
    "GovernanceSustainabilityEvidenceInputs",
    "GovernanceSustainabilityEvidencePayloads",
    "JsonObject",
]
