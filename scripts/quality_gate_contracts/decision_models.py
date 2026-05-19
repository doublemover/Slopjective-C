"""Quality-gate decision artifact models."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class QualityGateDecisionArtifacts:
    markdown: str
    status: dict[str, object]
    decision: str
    qg04_result: str
    recommendation_signal: str
    evidence_item_count: int


__all__ = ["QualityGateDecisionArtifacts"]
