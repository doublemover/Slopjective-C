from __future__ import annotations

from typing import Any

from .owners import SOURCE_HYGIENE_BLOCKER_METADATA


HARD_CUTOVER_GATE_ID = "source-hygiene-hard-cutover"
CLOSURE_ISSUES: tuple[str, ...] = ("8149", "8150")

REQUIRED_RESIDUE_CLASSES: tuple[str, ...] = (
    "alias-residue",
    "shim-fallback-language",
    "old-public-command-surface",
    "projected-behavior-claim",
    "report-only-completion",
    "legacy-compatibility-text",
)

RETIRED_ALLOWLIST_REPORT_FIELDS: tuple[str, ...] = (
    "allowlist",
    "allowlisted_findings",
    "allowed_findings",
    "allowed_finding_count",
)


def gate_contract_summary() -> dict[str, Any]:
    return {
        "gate_id": HARD_CUTOVER_GATE_ID,
        "closure_issues": list(CLOSURE_ISSUES),
        "required_residue_classes": list(REQUIRED_RESIDUE_CLASSES),
        "retired_allowlist_report_fields": list(RETIRED_ALLOWLIST_REPORT_FIELDS),
        "blocker_metadata": dict(SOURCE_HYGIENE_BLOCKER_METADATA),
    }


def build_gate_stats(
    *,
    finding_count: int,
    tracked_generated_report_count: int,
    generated_truth_boundary_finding_count: int,
) -> dict[str, int]:
    return {
        "finding_count": finding_count,
        "active_finding_count": finding_count,
        "tracked_generated_report_count": tracked_generated_report_count,
        "generated_truth_boundary_finding_count": generated_truth_boundary_finding_count,
    }
