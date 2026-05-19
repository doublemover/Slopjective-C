"""Record-level validation for support classification contracts."""

from __future__ import annotations

from typing import Any

from .contracts import ClassificationRecord
from .contracts import ContractError
from .contracts import PathExists
from .contracts import TriggerRecord
from .normalization import normalize_checked_in_path
from .normalization import require_list
from .normalization import require_string
from .rules import support_class_policy
from .rules import unknown_evidence_families


def require_record(row: Any, label: str) -> dict[str, Any]:
    if not isinstance(row, dict):
        raise ContractError(f"{label} must be an object")
    return row


def validate_support_class_record(row: Any, index: int) -> str:
    row_label = f"support_classes[{index}]"
    record = require_record(row, row_label)
    support_class = require_string(record.get("class"), f"{row_label}.class")
    require_string(record.get("meaning"), f"{row_label}.meaning")
    return support_class


def validate_evidence_family_record(row: Any, index: int) -> str:
    row_label = f"evidence_families[{index}]"
    record = require_record(row, row_label)
    family = require_string(record.get("family"), f"{row_label}.family")
    require_string(record.get("report"), f"{row_label}.report")
    require_string(
        record.get("required_contract_id"),
        f"{row_label}.required_contract_id",
    )
    return family


def validate_surface_paths(
    paths: list[Any],
    row_label: str,
    *,
    path_exists: PathExists,
) -> list[str]:
    out: list[str] = []
    for index, raw_path in enumerate(paths):
        path_text = require_string(raw_path, f"{row_label}.checked_in_surfaces[{index}]")
        out.append(
            normalize_checked_in_path(
                path_text,
                label=f"{row_label}.checked_in_surfaces[{index}]",
                path_exists=path_exists,
            )
        )
    return out


def classify_surface_record(
    row: Any,
    index: int,
    support_classes: set[str],
    evidence_families: set[str],
    *,
    path_exists: PathExists,
) -> ClassificationRecord:
    row_label = f"support_matrix[{index}]"
    record = require_record(row, row_label)
    surface = require_string(record.get("surface"), f"{row_label}.surface")
    current_class = require_string(
        record.get("current_class"),
        f"{row_label}.current_class",
    )
    if current_class not in support_classes:
        raise ContractError(
            f"{row_label}.current_class `{current_class}` is not a defined support class"
        )

    checked_in_surfaces = validate_surface_paths(
        require_list(record, "checked_in_surfaces"),
        row_label,
        path_exists=path_exists,
    )
    required_families = [
        require_string(
            family,
            f"{row_label}.required_evidence_families[{family_index}]",
        )
        for family_index, family in enumerate(
            require_list(record, "required_evidence_families")
        )
    ]
    unknown_families = unknown_evidence_families(required_families, evidence_families)
    if unknown_families:
        raise ContractError(
            f"{row_label} references unknown evidence families: {unknown_families}"
        )

    policy = support_class_policy(current_class)
    return {
        "surface": surface,
        "current_class": current_class,
        "claim_class": policy["claim_class"],
        "fail_closed": policy["fail_closed"],
        "release_blocking": policy["release_blocking"],
        "required_evidence_families": required_families,
        "checked_in_surfaces": checked_in_surfaces,
    }


def validate_demotion_trigger_record(
    row: Any,
    index: int,
    support_classes: set[str],
    evidence_families: set[str],
) -> TriggerRecord:
    row_label = f"demotion_triggers[{index}]"
    record = require_record(row, row_label)
    trigger = require_string(record.get("trigger"), f"{row_label}.trigger")
    demotes_to = require_string(record.get("demotes_to"), f"{row_label}.demotes_to")
    if demotes_to not in support_classes:
        raise ContractError(
            f"demotion trigger `{trigger}` targets unknown class `{demotes_to}`"
        )

    required_families = [
        require_string(family, f"{row_label}.required_families[{family_index}]")
        for family_index, family in enumerate(require_list(record, "required_families"))
    ]
    unknown_families = unknown_evidence_families(required_families, evidence_families)
    if unknown_families:
        raise ContractError(
            f"demotion trigger `{trigger}` references unknown evidence families: {unknown_families}"
        )
    return {
        "trigger": trigger,
        "demotes_to": demotes_to,
        "required_families": required_families,
    }


__all__ = [
    "classify_surface_record",
    "require_record",
    "validate_demotion_trigger_record",
    "validate_evidence_family_record",
    "validate_support_class_record",
    "validate_surface_paths",
]
