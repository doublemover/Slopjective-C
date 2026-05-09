from __future__ import annotations

from typing import Any, Iterable


SOURCE_HYGIENE_SCAN_ROOT_OWNER = "source-hygiene.scan-root-owner"
SOURCE_HYGIENE_PATTERN_OWNER = "source-hygiene.pattern-owner"
SOURCE_HYGIENE_GENERATED_REPORT_OWNER = "source-hygiene.generated-report-owner"
SOURCE_HYGIENE_BLOCKER_METADATA_OWNER = "source-hygiene.blocker-metadata-owner"

SOURCE_HYGIENE_SCAN_ROOT_OWNER_SURFACE = "scripts/source_hygiene/roots.py"
SOURCE_HYGIENE_PATTERN_OWNER_SURFACE = "scripts/source_hygiene/patterns.py"
SOURCE_HYGIENE_GENERATED_REPORT_OWNER_SURFACE = "scripts/source_hygiene/generated_reports.py"
SOURCE_HYGIENE_BLOCKER_METADATA_OWNER_SURFACE = "scripts/source_hygiene/owners.py"

SOURCE_HYGIENE_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-fail-closed",
    "blocker_scope": "source-hygiene-docs-capability-truth",
    "blocker_owner": SOURCE_HYGIENE_BLOCKER_METADATA_OWNER,
    "blocker_owner_surface": SOURCE_HYGIENE_BLOCKER_METADATA_OWNER_SURFACE,
}


def owner_record(owner_id: str, owner_surface: str) -> dict[str, str]:
    return {
        "owner_id": owner_id,
        "owner_surface": owner_surface,
    }


def scan_root_owner_summary(scan_roots: Iterable[str]) -> dict[str, Any]:
    return {
        **owner_record(
            SOURCE_HYGIENE_SCAN_ROOT_OWNER,
            SOURCE_HYGIENE_SCAN_ROOT_OWNER_SURFACE,
        ),
        "scan_roots": list(scan_roots),
    }


def pattern_owner_summary(pattern_owner_surfaces: Iterable[str]) -> dict[str, Any]:
    return {
        **owner_record(
            SOURCE_HYGIENE_PATTERN_OWNER,
            SOURCE_HYGIENE_PATTERN_OWNER_SURFACE,
        ),
        "pattern_owner_surfaces": sorted(set(pattern_owner_surfaces)),
    }


def generated_report_owner_summary(boundary_paths: Iterable[str]) -> dict[str, Any]:
    return {
        **owner_record(
            SOURCE_HYGIENE_GENERATED_REPORT_OWNER,
            SOURCE_HYGIENE_GENERATED_REPORT_OWNER_SURFACE,
        ),
        "generated_truth_outputs": list(boundary_paths),
    }


def source_hygiene_owner_contract_summary(
    *,
    scan_roots: Iterable[str],
    pattern_owner_surfaces: Iterable[str],
    generated_truth_outputs: Iterable[str],
) -> dict[str, Any]:
    return {
        "scan_root_owner": scan_root_owner_summary(scan_roots),
        "pattern_owner": pattern_owner_summary(pattern_owner_surfaces),
        "generated_report_owner": generated_report_owner_summary(
            generated_truth_outputs
        ),
        "blocker_metadata": dict(SOURCE_HYGIENE_BLOCKER_METADATA),
    }
