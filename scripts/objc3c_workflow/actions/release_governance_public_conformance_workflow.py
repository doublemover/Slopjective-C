"""Public-conformance workflow surface contract."""

from __future__ import annotations

from .release_governance_public_conformance_ids import (
    PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_CONTRACT_ID,
)
from .release_governance_public_conformance_models import (
    PublicConformanceWorkflowSurface,
)
from .release_governance_public_conformance_paths import (
    PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS,
    PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
    PUBLIC_CONFORMANCE_REPORT_PATHS,
    PUBLIC_CONFORMANCE_REQUIRED_ACTIONS,
)


PUBLIC_CONFORMANCE_WORKFLOW_SURFACE = PublicConformanceWorkflowSurface(
    contract_id=PUBLIC_CONFORMANCE_WORKFLOW_SURFACE_CONTRACT_ID,
    required_actions=PUBLIC_CONFORMANCE_REQUIRED_ACTIONS,
    report_paths=PUBLIC_CONFORMANCE_REPORT_PATHS,
    artifact_paths=PUBLIC_CONFORMANCE_PUBLISHED_ARTIFACT_PATHS,
    child_report_contracts=PUBLIC_CONFORMANCE_CHILD_REPORT_CONTRACTS,
)


__all__ = ("PUBLIC_CONFORMANCE_WORKFLOW_SURFACE",)
