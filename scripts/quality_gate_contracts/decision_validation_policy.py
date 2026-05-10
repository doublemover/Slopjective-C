"""Quality-gate baseline validation policy metadata."""

from __future__ import annotations

from dataclasses import dataclass

from .decision_data import (
    BASE_GATE_SEQUENCE,
    BASELINE_CONSUMER_CONTRACT,
    BASELINE_EV_ARTIFACT_CONTRACT,
    BASELINE_EVIDENCE_CONTRACT,
    BASELINE_GATE_CONTRACT,
    CONSUMER_HANDOFF_KEYS,
    EVIDENCE_ITEM_KEYS,
    EV_ARTIFACT_MAPPING_KEYS,
    GATE_RESULT_KEYS,
    VALID_EVIDENCE_STATUSES,
    VALID_GATE_STATUSES,
)


@dataclass(frozen=True)
class OrderedContractRule:
    row_count_subject: str
    ordering_subject: str
    expected_keys: tuple[str, ...]
    expected_order: tuple[str, ...]


EVIDENCE_CONTRACT_RULE = OrderedContractRule(
    row_count_subject="evidence_items",
    ordering_subject="evidence_id ordering drift",
    expected_keys=EVIDENCE_ITEM_KEYS,
    expected_order=tuple(
        str(row["evidence_id"]) for row in BASELINE_EVIDENCE_CONTRACT
    ),
)
EV_ARTIFACT_MAPPING_RULE = OrderedContractRule(
    row_count_subject="ev_contract_mapping",
    ordering_subject="ev_contract_mapping ordering drift",
    expected_keys=EV_ARTIFACT_MAPPING_KEYS,
    expected_order=tuple(
        str(row["evidence_id"]) for row in BASELINE_EV_ARTIFACT_CONTRACT
    ),
)
GATE_RESULTS_RULE = OrderedContractRule(
    row_count_subject="gate_results",
    ordering_subject="gate ordering drift",
    expected_keys=GATE_RESULT_KEYS,
    expected_order=BASE_GATE_SEQUENCE,
)
CONSUMER_HANDOFF_RULE = OrderedContractRule(
    row_count_subject="downstream_handoffs",
    ordering_subject="downstream consumer ordering drift",
    expected_keys=CONSUMER_HANDOFF_KEYS,
    expected_order=tuple(
        str(row["consumer_seed"]) for row in BASELINE_CONSUMER_CONTRACT
    ),
)

EXPECTED_EVIDENCE_BY_ID = {
    str(row["evidence_id"]): row for row in BASELINE_EVIDENCE_CONTRACT
}
EXPECTED_EV_ARTIFACT_BY_EVIDENCE_ID = {
    str(row["evidence_id"]): row for row in BASELINE_EV_ARTIFACT_CONTRACT
}
EXPECTED_GATE_BY_ID = {
    str(row["gate_id"]): row for row in BASELINE_GATE_CONTRACT
}
EXPECTED_CONSUMER_BY_SEED = {
    str(row["consumer_seed"]): row for row in BASELINE_CONSUMER_CONTRACT
}

__all__ = [
    "CONSUMER_HANDOFF_RULE",
    "EVIDENCE_CONTRACT_RULE",
    "EV_ARTIFACT_MAPPING_RULE",
    "EXPECTED_CONSUMER_BY_SEED",
    "EXPECTED_EV_ARTIFACT_BY_EVIDENCE_ID",
    "EXPECTED_EVIDENCE_BY_ID",
    "EXPECTED_GATE_BY_ID",
    "GATE_RESULTS_RULE",
    "OrderedContractRule",
    "VALID_EVIDENCE_STATUSES",
    "VALID_GATE_STATUSES",
]
