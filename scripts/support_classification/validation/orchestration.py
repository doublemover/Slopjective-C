"""Public orchestration for support classification validation."""

from __future__ import annotations

from typing import Any

from .contracts import ClassificationRecord
from .contracts import PathExists
from .contracts import TriggerRecord
from .normalization import require_list
from .records import classify_surface_record
from .records import validate_demotion_trigger_record


def classify_surfaces(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
    *,
    path_exists: PathExists,
) -> list[ClassificationRecord]:
    rows = require_list(contract, "support_matrix")
    return [
        classify_surface_record(
            row,
            index,
            support_classes,
            evidence_families,
            path_exists=path_exists,
        )
        for index, row in enumerate(rows)
    ]


def validate_demotion_triggers(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
) -> list[TriggerRecord]:
    rows = require_list(contract, "demotion_triggers")
    return [
        validate_demotion_trigger_record(row, index, support_classes, evidence_families)
        for index, row in enumerate(rows)
    ]


__all__ = ["classify_surfaces", "validate_demotion_triggers"]
