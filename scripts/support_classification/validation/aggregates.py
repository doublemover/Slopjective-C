"""Aggregate validation for support classification contracts."""

from __future__ import annotations

from typing import Any

from .contracts import CANONICAL_SUPPORT_CLASSES
from .contracts import ContractError
from .contracts import PathExists
from .normalization import require_list
from .normalization import require_string
from .records import validate_evidence_family_record
from .records import validate_support_class_record
from .rules import missing_support_classes
from .rules import uses_tmp_source_truth


def validate_support_classes(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "support_classes")
    classes: set[str] = set()
    for index, row in enumerate(rows):
        support_class = validate_support_class_record(row, index)
        if support_class not in CANONICAL_SUPPORT_CLASSES:
            raise ContractError(
                f"unknown support class `{support_class}` in support_classes[{index}]"
            )
        classes.add(support_class)
    missing = missing_support_classes(classes)
    if missing:
        raise ContractError(f"missing canonical support classes: {missing}")
    return classes


def validate_evidence_families(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "evidence_families")
    families: set[str] = set()
    for index, row in enumerate(rows):
        family = validate_evidence_family_record(row, index)
        if family in families:
            raise ContractError(f"duplicate evidence family `{family}`")
        families.add(family)
    return families


def validate_public_claim_surfaces(
    contract: dict[str, Any],
    *,
    path_exists: PathExists,
) -> list[str]:
    surfaces: list[str] = []
    for index, raw_path in enumerate(require_list(contract, "public_claim_surfaces")):
        path_text = require_string(raw_path, f"public_claim_surfaces[{index}]")
        if uses_tmp_source_truth(path_text):
            raise ContractError(
                f"public_claim_surfaces[{index}] must not use tmp as source truth"
            )
        if not path_exists(path_text):
            raise ContractError(f"missing public claim surface `{path_text}`")
        surfaces.append(path_text.replace("\\", "/"))
    return surfaces


__all__ = [
    "validate_evidence_families",
    "validate_public_claim_surfaces",
    "validate_support_classes",
]
