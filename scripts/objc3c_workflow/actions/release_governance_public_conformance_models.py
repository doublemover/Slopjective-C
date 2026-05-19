"""Typed public-conformance reporting workflow contract models."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Literal

from ..action_spec import ActionSpec

PublicConformanceStatus = Literal["pass", "caution", "blocked"]
PublicConformanceBadge = Literal["claim-ready", "provisional", "blocked"]


@dataclass(frozen=True)
class PublicConformanceEvidenceFamily:
    family_id: str
    claim_rule: str
    source_paths: tuple[str, ...]


@dataclass(frozen=True)
class PublicConformanceSchemaAnchor:
    surface_key: str
    registry_id: str
    schema_path: str
    identity_property: str
    identity_value: str


@dataclass(frozen=True)
class PublicConformanceScoreBand:
    band_id: PublicConformanceBadge
    min_score: int
    max_score: int
    badge: PublicConformanceBadge
    public_status: PublicConformanceStatus


@dataclass(frozen=True)
class PublicConformanceStabilityPolicy:
    contract_id: str
    allowed_public_statuses: tuple[PublicConformanceStatus, ...]
    allowed_badges: tuple[PublicConformanceBadge, ...]
    score_bands: tuple[PublicConformanceScoreBand, ...]
    fail_closed_conditions: tuple[str, ...]
    capability_truth_rules: tuple[str, ...]


@dataclass(frozen=True)
class PublicConformanceWorkflowSurface:
    contract_id: str
    required_actions: tuple[str, ...]
    report_paths: tuple[str, ...]
    artifact_paths: tuple[str, ...]
    child_report_contracts: tuple[tuple[str, str], ...]


@dataclass(frozen=True)
class PublicConformanceActionFragment:
    action: str
    summary: str
    backend: str
    guarantee_owner: str
    report_paths: tuple[str, ...] = ()
    artifact_paths: tuple[str, ...] = ()
    upstream_truth: tuple[str, ...] = ()
    validation_tier: str = "repo"

    def to_action_spec(self) -> ActionSpec:
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=self.guarantee_owner,
        )
