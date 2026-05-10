"""Support classification contract validation."""

from __future__ import annotations

from collections.abc import Callable
from typing import Any

from .constants import CANONICAL_SUPPORT_CLASSES
from .models import ContractError


def require_list(contract: dict[str, Any], key: str) -> list[Any]:
    value = contract.get(key)
    if not isinstance(value, list):
        raise ContractError(f"contract field `{key}` must be a list")
    return value


def require_string(value: Any, label: str) -> str:
    if not isinstance(value, str) or not value:
        raise ContractError(f"{label} must be a non-empty string")
    return value


def validate_support_classes(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "support_classes")
    classes: set[str] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"support_classes[{index}] must be an object")
        support_class = require_string(
            row.get("class"),
            f"support_classes[{index}].class",
        )
        require_string(row.get("meaning"), f"support_classes[{index}].meaning")
        if support_class not in CANONICAL_SUPPORT_CLASSES:
            raise ContractError(
                f"unknown support class `{support_class}` in support_classes[{index}]"
            )
        classes.add(support_class)
    missing = set(CANONICAL_SUPPORT_CLASSES) - classes
    if missing:
        raise ContractError(f"missing canonical support classes: {sorted(missing)}")
    return classes


def validate_evidence_families(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "evidence_families")
    families: set[str] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"evidence_families[{index}] must be an object")
        family = require_string(row.get("family"), f"evidence_families[{index}].family")
        require_string(row.get("report"), f"evidence_families[{index}].report")
        require_string(
            row.get("required_contract_id"),
            f"evidence_families[{index}].required_contract_id",
        )
        if family in families:
            raise ContractError(f"duplicate evidence family `{family}`")
        families.add(family)
    return families


def normalize_checked_in_path(
    path_text: str,
    *,
    label: str,
    path_exists: Callable[[str], bool],
) -> str:
    if path_text.startswith("tmp/") or path_text.startswith("tmp\\"):
        raise ContractError(f"{label} must not use tmp as source truth")
    if not path_exists(path_text):
        raise ContractError(f"missing checked-in surface path `{path_text}`")
    return path_text.replace("\\", "/")


def validate_surface_paths(
    paths: list[Any],
    row_label: str,
    *,
    path_exists: Callable[[str], bool],
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


def classify_surfaces(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
    *,
    path_exists: Callable[[str], bool],
) -> list[dict[str, Any]]:
    rows = require_list(contract, "support_matrix")
    classifications: list[dict[str, Any]] = []
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"support_matrix[{index}] must be an object")
        row_label = f"support_matrix[{index}]"
        surface = require_string(row.get("surface"), f"{row_label}.surface")
        current_class = require_string(
            row.get("current_class"),
            f"{row_label}.current_class",
        )
        if current_class not in support_classes:
            raise ContractError(
                f"{row_label}.current_class `{current_class}` is not a defined support class"
            )
        checked_in_surfaces = validate_surface_paths(
            require_list(row, "checked_in_surfaces"),
            row_label,
            path_exists=path_exists,
        )
        required_families = [
            require_string(
                family,
                f"{row_label}.required_evidence_families[{family_index}]",
            )
            for family_index, family in enumerate(
                require_list(row, "required_evidence_families")
            )
        ]
        unknown_families = sorted(set(required_families) - evidence_families)
        if unknown_families:
            raise ContractError(
                f"{row_label} references unknown evidence families: {unknown_families}"
            )
        policy = CANONICAL_SUPPORT_CLASSES[current_class]
        classifications.append(
            {
                "surface": surface,
                "current_class": current_class,
                "claim_class": policy["claim_class"],
                "fail_closed": policy["fail_closed"],
                "release_blocking": policy["release_blocking"],
                "required_evidence_families": required_families,
                "checked_in_surfaces": checked_in_surfaces,
            }
        )
    return classifications


def validate_demotion_triggers(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
) -> list[dict[str, Any]]:
    rows = require_list(contract, "demotion_triggers")
    triggers: list[dict[str, Any]] = []
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"demotion_triggers[{index}] must be an object")
        trigger = require_string(
            row.get("trigger"),
            f"demotion_triggers[{index}].trigger",
        )
        demotes_to = require_string(
            row.get("demotes_to"), f"demotion_triggers[{index}].demotes_to"
        )
        if demotes_to not in support_classes:
            raise ContractError(
                f"demotion trigger `{trigger}` targets unknown class `{demotes_to}`"
            )
        required_families = [
            require_string(family, f"demotion_triggers[{index}].required_families[{i}]")
            for i, family in enumerate(require_list(row, "required_families"))
        ]
        unknown_families = sorted(set(required_families) - evidence_families)
        if unknown_families:
            raise ContractError(
                f"demotion trigger `{trigger}` references unknown evidence families: {unknown_families}"
            )
        triggers.append(
            {
                "trigger": trigger,
                "demotes_to": demotes_to,
                "required_families": required_families,
            }
        )
    return triggers


def validate_public_claim_surfaces(
    contract: dict[str, Any],
    *,
    path_exists: Callable[[str], bool],
) -> list[str]:
    surfaces: list[str] = []
    for index, raw_path in enumerate(require_list(contract, "public_claim_surfaces")):
        path_text = require_string(raw_path, f"public_claim_surfaces[{index}]")
        if path_text.startswith("tmp/") or path_text.startswith("tmp\\"):
            raise ContractError(
                f"public_claim_surfaces[{index}] must not use tmp as source truth"
            )
        if not path_exists(path_text):
            raise ContractError(f"missing public claim surface `{path_text}`")
        surfaces.append(path_text.replace("\\", "/"))
    return surfaces


def require_existing_contract_path(
    contract: dict[str, Any],
    *,
    key: str,
    path_exists: Callable[[str], bool],
) -> str:
    path_text = require_string(contract.get(key), key)
    if not path_exists(path_text):
        raise ContractError(f"{key} path is missing: `{path_text}`")
    return path_text.replace("\\", "/")


__all__ = [
    "classify_surfaces",
    "normalize_checked_in_path",
    "require_existing_contract_path",
    "require_list",
    "require_string",
    "validate_demotion_triggers",
    "validate_evidence_families",
    "validate_public_claim_surfaces",
    "validate_support_classes",
    "validate_surface_paths",
]
