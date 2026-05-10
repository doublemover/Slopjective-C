"""Path contracts for governance sustainability evidence reports."""

from __future__ import annotations


TEMP_REPORT_ROOT = ("tmp", "reports")
GOVERNANCE_REPORT_FAMILY = (*TEMP_REPORT_ROOT, "governance-sustainability")


def evidence_path(*parts: str) -> str:
    return "/".join(parts)


SELF_GENERATED_REPORTS = {
    evidence_path(*GOVERNANCE_REPORT_FAMILY, "evidence-summary.json"),
    evidence_path(*GOVERNANCE_REPORT_FAMILY, "publication-summary.json"),
    evidence_path(
        *GOVERNANCE_REPORT_FAMILY,
        "closeout-gate",
        "governance_sustainability_closeout_gate.json",
    ),
}


__all__ = [
    "GOVERNANCE_REPORT_FAMILY",
    "SELF_GENERATED_REPORTS",
    "TEMP_REPORT_ROOT",
    "evidence_path",
]
