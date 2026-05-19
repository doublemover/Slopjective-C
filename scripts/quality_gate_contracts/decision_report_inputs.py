from __future__ import annotations

from .decision_identity import (
    ACCEPTANCE_GATE_ID,
    CONTRACT_ID,
    RELEASE_ID,
    RELEASE_LABEL,
    SEED_ID,
    SOURCE_REVISION,
    TASK_ID,
    VALIDATED_BY,
)
from .decision_report_rows import (
    ACCEPTANCE_ROLLUP,
    ACTIVE_EXCEPTION_IDS,
    DOWNSTREAM_HANDOFFS,
    EVIDENCE_ITEMS,
    EV_ARTIFACT_MAPPING,
    THRESHOLD_RESULTS,
    UNRESOLVED_BLOCKERS,
    VALIDATION_COMMAND_REFS,
)
from .reports import QualityGateReportInputs

REPORT_INPUTS = QualityGateReportInputs(
    contract_id=CONTRACT_ID,
    seed_id=SEED_ID,
    acceptance_gate_id=ACCEPTANCE_GATE_ID,
    task_id=TASK_ID,
    release_label=RELEASE_LABEL,
    release_id=RELEASE_ID,
    source_revision=SOURCE_REVISION,
    validated_by=VALIDATED_BY,
    validation_command_refs=VALIDATION_COMMAND_REFS,
    ev_artifact_mapping=EV_ARTIFACT_MAPPING,
    evidence_items=EVIDENCE_ITEMS,
    threshold_results=THRESHOLD_RESULTS,
    active_exception_ids=ACTIVE_EXCEPTION_IDS,
    unresolved_blockers=UNRESOLVED_BLOCKERS,
    downstream_handoffs=DOWNSTREAM_HANDOFFS,
    acceptance_rollup=ACCEPTANCE_ROLLUP,
)

__all__ = ["REPORT_INPUTS"]
