"""Drift summary construction for public claim surfaces."""

from __future__ import annotations

from typing import Any, Mapping

from .constants import SUMMARY_CONTRACT_ID
from .models import JsonObject, PublicClaimDriftInputs
from .validation import (
    public_claim_drift_owner_contract,
    public_claim_scan_paths,
    scan_surface_lines,
)


def build_public_claim_drift_summary(inputs: PublicClaimDriftInputs) -> JsonObject:
    support_summary = inputs.support_summary
    classifications = support_summary["classifications"]
    public_surfaces = set(support_summary["public_claim_surfaces"])
    rows_by_path: dict[str, list[Mapping[str, Any]]] = {}
    envelope_row: Mapping[str, Any] | None = None
    for row in classifications:
        if row.get("surface") == "envelope-level-public-claims":
            envelope_row = row
        for surface in row["checked_in_surfaces"]:
            rows_by_path.setdefault(surface, []).append(row)

    scan_paths = public_claim_scan_paths(support_summary)
    tmp_source_truth_paths = [path for path in scan_paths if path.startswith("tmp/")]
    owner_contract = public_claim_drift_owner_contract(
        public_surfaces=public_surfaces,
        scan_paths=scan_paths,
    )

    claim_mappings: list[JsonObject] = []
    findings: list[JsonObject] = []
    for relative_path in scan_paths:
        mappings, surface_findings = scan_surface_lines(
            relative_path=relative_path,
            lines=inputs.surface_lines[relative_path],
            rows_by_path=rows_by_path,
            envelope_row=envelope_row,
            public_surfaces=public_surfaces,
        )
        claim_mappings.extend(mappings)
        findings.extend(surface_findings)

    checks = {
        "support_classification_passed": support_summary.get("status") == "PASS",
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
        "public_claim_surfaces_exist": all(path in inputs.surface_lines for path in public_surfaces),
        "every_claim_has_mapping": all(
            mapping["mapped_surfaces"] or mapping["guarded"] for mapping in claim_mappings
        ),
        "promotional_claims_have_evidence_or_guard": not any(
            finding["kind"] == "unmapped-promotional-public-claim" for finding in findings
        ),
        "unsupported_surfaces_fail_closed": not any(
            finding["kind"] == "unguarded-forbidden-public-claim" for finding in findings
        ),
    }
    status = "PASS" if all(checks.values()) and not findings else "FAIL"
    evidence_by_surface = {
        row["surface"]: {
            "current_class": row["current_class"],
            "fail_closed": row["fail_closed"],
            "required_evidence_families": row["required_evidence_families"],
            "checked_in_surfaces": row["checked_in_surfaces"],
        }
        for row in classifications
    }
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": status,
        "support_summary_path": inputs.support_summary_path,
        "support_summary_contract_id": support_summary.get("contract_id"),
        "support_summary_sha256": inputs.support_summary_sha256,
        "owner_contract": owner_contract,
        "blocker_metadata": owner_contract["blocker_metadata"],
        "scanned_surface_count": len(scan_paths),
        "public_claim_surface_count": len(public_surfaces),
        "claim_mapping_count": len(claim_mappings),
        "finding_count": len(findings),
        "scan_paths": scan_paths,
        "surface_sha256": dict(inputs.surface_sha256),
        "evidence_by_surface": evidence_by_surface,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "claim_mappings": claim_mappings,
        "findings": findings,
        "checks": checks,
    }


__all__ = ["build_public_claim_drift_summary"]
