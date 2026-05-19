"""Support classification summary construction."""

from __future__ import annotations

from collections import Counter
from collections.abc import Callable
from typing import Any

from .constants import CANONICAL_SUPPORT_CLASSES
from .constants import SUMMARY_CONTRACT_ID
from .constants import SUMMARY_SCRIPT_PATH
from .models import SupportClassificationSource
from .validation import classify_surfaces
from .validation import require_existing_contract_path
from .validation import validate_demotion_triggers
from .validation import validate_evidence_families
from .validation import validate_public_claim_surfaces
from .validation import validate_support_classes


def build_support_classification_summary(
    source: SupportClassificationSource,
    *,
    path_exists: Callable[[str], bool],
) -> dict[str, Any]:
    contract = source.contract
    support_classes = validate_support_classes(contract)
    evidence_families = validate_evidence_families(contract)
    classifications = classify_surfaces(
        contract,
        support_classes,
        evidence_families,
        path_exists=path_exists,
    )
    demotion_triggers = validate_demotion_triggers(
        contract, support_classes, evidence_families
    )
    public_claim_surfaces = validate_public_claim_surfaces(
        contract,
        path_exists=path_exists,
    )

    runbook = require_existing_contract_path(
        contract,
        key="runbook",
        path_exists=path_exists,
    )
    legacy_summary_script = require_existing_contract_path(
        contract,
        key="summary_script",
        path_exists=path_exists,
    )

    class_counts = Counter(row["current_class"] for row in classifications)
    source_truth_paths = sorted(
        {
            source.contract_path,
            runbook,
            SUMMARY_SCRIPT_PATH,
            legacy_summary_script,
            *public_claim_surfaces,
            *(
                surface
                for row in classifications
                for surface in row["checked_in_surfaces"]
            ),
        }
    )
    tmp_source_truth_paths = [
        path_text
        for path_text in source_truth_paths
        if path_text.startswith("tmp/") or path_text.startswith("tmp\\")
    ]
    fail_closed_surfaces = [
        row["surface"] for row in classifications if row["fail_closed"]
    ]
    release_blocking_surfaces = [
        row["surface"] for row in classifications if row["release_blocking"]
    ]

    checks = {
        "canonical_support_classes_defined": set(CANONICAL_SUPPORT_CLASSES)
        == support_classes,
        "all_classifications_use_known_classes": all(
            row["current_class"] in support_classes for row in classifications
        ),
        "all_required_evidence_families_are_declared": all(
            set(row["required_evidence_families"]).issubset(evidence_families)
            for row in classifications
        ),
        "demotion_targets_are_known_classes": all(
            row["demotes_to"] in support_classes for row in demotion_triggers
        ),
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
        "has_supported_surface": class_counts["supported"] > 0,
        "has_fail_closed_surface": bool(fail_closed_surfaces),
    }
    status = "PASS" if all(checks.values()) else "FAIL"

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": status,
        "source_contract_id": contract["contract_id"],
        "source_contract_path": source.contract_path,
        "source_contract_sha256": source.contract_sha256,
        "runbook": runbook,
        "summary_script": SUMMARY_SCRIPT_PATH,
        "legacy_support_matrix_summary_script": legacy_summary_script,
        "support_classes": sorted(support_classes),
        "support_class_count": len(support_classes),
        "evidence_families": sorted(evidence_families),
        "evidence_family_count": len(evidence_families),
        "classification_count": len(classifications),
        "class_counts": dict(sorted(class_counts.items())),
        "classifications": classifications,
        "demotion_triggers": demotion_triggers,
        "public_claim_surfaces": public_claim_surfaces,
        "source_truth_paths": source_truth_paths,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "fail_closed_surfaces": fail_closed_surfaces,
        "release_blocking_surfaces": release_blocking_surfaces,
        "checks": checks,
    }


__all__ = ["build_support_classification_summary"]
