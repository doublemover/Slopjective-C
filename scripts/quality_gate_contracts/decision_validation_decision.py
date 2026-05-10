"""Decision-level validation orchestration for quality-gate contracts."""

from __future__ import annotations

from typing import Sequence

from .decision_errors import ContractDriftError, ContractHardFailError
from .decision_logic import determine_decision, recommendation_signal_for
from .decision_validation_contracts import (
    validate_consumer_contract,
    validate_ev_artifact_mapping_contract,
    validate_evidence_contract,
    validate_gate_results_contract,
)
from .decision_validation_helpers import require_non_empty_string
from .decision_validation_policy import VALID_GATE_STATUSES


def validate_decision_semantics(
    *,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    evidence_items: list[dict[str, object]],
) -> None:
    if qg04_result not in VALID_GATE_STATUSES:
        raise ContractHardFailError(f"qg04_result is unsupported: {qg04_result}")

    expected_recommendation = recommendation_signal_for(qg04_result)
    if recommendation_signal != expected_recommendation:
        raise ContractDriftError(
            "recommendation_signal drift: "
            f"expected {expected_recommendation}, found {recommendation_signal}"
        )

    expected_decision = determine_decision(qg04_result)
    if decision != expected_decision:
        raise ContractDriftError(
            f"overall_decision drift: expected {expected_decision}, found {decision}"
        )

    non_pass_evidence = [
        require_non_empty_string(row, key="evidence_id", context=f"evidence_items[{index}]")
        for index, row in enumerate(evidence_items)
        if require_non_empty_string(row, key="status", context=f"evidence_items[{index}]")
        != "pass"
    ]
    if decision == "approve" and non_pass_evidence:
        raise ContractDriftError(
            "approve/hold contract drift: decision=approve with non-pass evidence rows "
            f"{non_pass_evidence}"
        )


def validate_baseline_contract(
    *,
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
    ev_contract_mapping: Sequence[dict[str, str]],
    evidence_items: list[dict[str, object]],
    downstream_handoffs: list[dict[str, object]],
) -> None:
    if not generated_at.strip():
        raise ContractHardFailError("generated_at must be a non-empty string")
    validate_evidence_contract(evidence_items)
    validate_ev_artifact_mapping_contract(ev_contract_mapping)
    validate_gate_results_contract(gate_results)
    validate_consumer_contract(downstream_handoffs)
    validate_decision_semantics(
        decision=decision,
        qg04_result=qg04_result,
        recommendation_signal=recommendation_signal,
        evidence_items=evidence_items,
    )


__all__ = [
    "validate_baseline_contract",
    "validate_decision_semantics",
]
