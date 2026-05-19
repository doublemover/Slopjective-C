"""Facade re-exports for public claim drift contracts."""

from __future__ import annotations

if __package__:
    from .public_claim_drift import (
        CLAIM_KEYWORD_RE,
        FORBIDDEN_PATTERNS,
        GUARD_RE,
        JsonObject,
        PROMOTIONAL_CLAIM_RE,
        PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA,
        PUBLIC_CLAIM_DRIFT_OWNER,
        PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
        PublicClaimDriftInputs,
        SUMMARY_CONTRACT_ID,
        build_public_claim_drift_summary,
        clipped,
        context_window,
        is_guarded,
        public_claim_drift_owner_contract,
        public_claim_scan_paths,
        render_markdown,
        scan_surface_lines,
        status_line,
    )
else:
    from public_claim_drift import (
        CLAIM_KEYWORD_RE,
        FORBIDDEN_PATTERNS,
        GUARD_RE,
        JsonObject,
        PROMOTIONAL_CLAIM_RE,
        PUBLIC_CLAIM_DRIFT_BLOCKER_METADATA,
        PUBLIC_CLAIM_DRIFT_OWNER,
        PUBLIC_CLAIM_DRIFT_OWNER_SURFACE,
        PublicClaimDriftInputs,
        SUMMARY_CONTRACT_ID,
        build_public_claim_drift_summary,
        clipped,
        context_window,
        is_guarded,
        public_claim_drift_owner_contract,
        public_claim_scan_paths,
        render_markdown,
        scan_surface_lines,
        status_line,
    )

__all__ = [
    "PublicClaimDriftInputs",
    "build_public_claim_drift_summary",
    "public_claim_scan_paths",
    "render_markdown",
    "status_line",
]
