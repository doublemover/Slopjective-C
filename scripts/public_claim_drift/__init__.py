"""Importable public claim drift contract package."""

from __future__ import annotations

from .constants import (
    CLAIM_KEYWORD_RE,
    FORBIDDEN_PATTERNS,
    GUARD_RE,
    PROMOTIONAL_CLAIM_RE,
    PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA,
    PUBLIC_CLAIM_DRIFT_OWNER,
    PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
    SUMMARY_CONTRACT_ID,
)
from .models import JsonObject, PublicClaimDriftInputs
from .rendering import render_markdown, status_line
from .summary import build_public_claim_drift_summary
from .validation import (
    clipped,
    context_window,
    is_guarded,
    public_claim_drift_owner_contract,
    public_claim_scan_paths,
    scan_surface_lines,
)

__all__ = [
    "CLAIM_KEYWORD_RE",
    "FORBIDDEN_PATTERNS",
    "GUARD_RE",
    "JsonObject",
    "PROMOTIONAL_CLAIM_RE",
    "PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA",
    "PUBLIC_CLAIM_DRIFT_OWNER",
    "PUBLIC_CLAIM_DRIFT_OWNER_SURFACE",
    "PublicClaimDriftInputs",
    "SUMMARY_CONTRACT_ID",
    "build_public_claim_drift_summary",
    "clipped",
    "context_window",
    "is_guarded",
    "public_claim_drift_owner_contract",
    "public_claim_scan_paths",
    "render_markdown",
    "scan_surface_lines",
    "status_line",
]
