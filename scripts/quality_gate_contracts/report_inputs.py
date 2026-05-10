from __future__ import annotations

from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class QualityGateReportInputs:
    contract_id: str
    seed_id: str
    acceptance_gate_id: str
    task_id: str
    release_label: str
    release_id: str
    source_revision: str
    validated_by: str
    validation_command_refs: list[str]
    ev_artifact_mapping: Sequence[dict[str, str]]
    evidence_items: list[dict[str, object]]
    threshold_results: list[dict[str, object]]
    active_exception_ids: list[str]
    unresolved_blockers: list[dict[str, str]]
    downstream_handoffs: list[dict[str, object]]
    acceptance_rollup: list[dict[str, str]]


__all__ = ("QualityGateReportInputs",)
