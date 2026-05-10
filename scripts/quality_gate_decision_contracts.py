from __future__ import annotations

from dataclasses import dataclass
from typing import Sequence

if __package__:
    from .quality_gate_contracts.decision_data import (
        ACCEPTANCE_GATE_ID,
        ACCEPTANCE_ROLLUP,
        ACTIVE_EXCEPTION_IDS,
        BASE_GATE_RESULTS,
        BASE_GATE_SEQUENCE,
        BASELINE_CONSUMER_CONTRACT,
        BASELINE_EV_ARTIFACT_CONTRACT,
        BASELINE_EVIDENCE_CONTRACT,
        BASELINE_GATE_CONTRACT,
        CONSUMER_HANDOFF_KEYS,
        CONTRACT_ID,
        DOWNSTREAM_HANDOFFS,
        EVIDENCE_ITEM_KEYS,
        EVIDENCE_ITEMS,
        EV_ARTIFACT_MAPPING,
        EV_ARTIFACT_MAPPING_KEYS,
        GATE_RESULT_KEYS,
        REPORT_INPUTS,
        THRESHOLD_RESULTS,
        UNRESOLVED_BLOCKERS,
        VALIDATION_COMMAND_REFS,
        VALID_EVIDENCE_STATUSES,
        VALID_GATE_STATUSES,
    )
    from .quality_gate_contracts.reports import render_markdown, render_status
else:
    from quality_gate_contracts.decision_data import (
        ACCEPTANCE_GATE_ID,
        ACCEPTANCE_ROLLUP,
        ACTIVE_EXCEPTION_IDS,
        BASE_GATE_RESULTS,
        BASE_GATE_SEQUENCE,
        BASELINE_CONSUMER_CONTRACT,
        BASELINE_EV_ARTIFACT_CONTRACT,
        BASELINE_EVIDENCE_CONTRACT,
        BASELINE_GATE_CONTRACT,
        CONSUMER_HANDOFF_KEYS,
        CONTRACT_ID,
        DOWNSTREAM_HANDOFFS,
        EVIDENCE_ITEM_KEYS,
        EVIDENCE_ITEMS,
        EV_ARTIFACT_MAPPING,
        EV_ARTIFACT_MAPPING_KEYS,
        GATE_RESULT_KEYS,
        REPORT_INPUTS,
        THRESHOLD_RESULTS,
        UNRESOLVED_BLOCKERS,
        VALIDATION_COMMAND_REFS,
        VALID_EVIDENCE_STATUSES,
        VALID_GATE_STATUSES,
    )
    from quality_gate_contracts.reports import render_markdown, render_status

class ContractDriftError(RuntimeError):
    """Raised when baseline contract rows are structurally valid but semantically drifted."""


class ContractHardFailError(RuntimeError):
    """Raised when baseline contract rows are malformed and not safely consumable."""


@dataclass(frozen=True)
class QualityGateDecisionArtifacts:
    markdown: str
    status: dict[str, object]
    decision: str
    qg04_result: str
    recommendation_signal: str
    evidence_item_count: int


def require_mapping(value: object, *, context: str) -> dict[str, object]:
    if not isinstance(value, dict):
        raise ContractHardFailError(f"{context}: expected object")
    return value


def require_exact_keys(
    payload: dict[str, object],
    *,
    expected_keys: tuple[str, ...],
    context: str,
) -> None:
    missing = [key for key in expected_keys if key not in payload]
    unexpected = [key for key in payload if key not in expected_keys]
    if missing or unexpected:
        detail_parts: list[str] = []
        if missing:
            detail_parts.append(f"missing keys {missing}")
        if unexpected:
            detail_parts.append(f"unexpected keys {unexpected}")
        details = "; ".join(detail_parts)
        raise ContractHardFailError(f"{context}: schema mismatch ({details})")


def require_non_empty_string(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value.strip():
        raise ContractHardFailError(f"{context}.{key}: expected non-empty string")
    return value.strip()


def require_string_list(
    payload: dict[str, object],
    *,
    key: str,
    context: str,
) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise ContractHardFailError(f"{context}.{key}: expected array of strings")
    normalized: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item.strip():
            raise ContractHardFailError(
                f"{context}.{key}[{index}]: expected non-empty string"
            )
        normalized.append(item.strip())
    return normalized


def validate_evidence_contract(evidence_items: list[dict[str, object]]) -> None:
    expected_by_id = {
        str(row["evidence_id"]): row for row in BASELINE_EVIDENCE_CONTRACT
    }
    expected_order = tuple(str(row["evidence_id"]) for row in BASELINE_EVIDENCE_CONTRACT)
    seen_ids: set[str] = set()
    actual_order: list[str] = []

    if len(evidence_items) != len(expected_order):
        raise ContractDriftError(
            "evidence_items row-count drift: "
            f"expected {len(expected_order)}, found {len(evidence_items)}"
        )

    for index, raw_item in enumerate(evidence_items):
        context = f"evidence_items[{index}]"
        item = require_mapping(raw_item, context=context)
        require_exact_keys(item, expected_keys=EVIDENCE_ITEM_KEYS, context=context)
        evidence_id = require_non_empty_string(item, key="evidence_id", context=context)
        status = require_non_empty_string(item, key="status", context=context)
        summary = require_non_empty_string(item, key="summary", context=context)
        blocking_refs = require_string_list(item, key="blocking_refs", context=context)

        if status not in VALID_EVIDENCE_STATUSES:
            raise ContractHardFailError(
                f"{context}.status: unsupported evidence status '{status}'"
            )

        if evidence_id in seen_ids:
            raise ContractDriftError(f"duplicate evidence_id detected: {evidence_id}")
        seen_ids.add(evidence_id)
        actual_order.append(evidence_id)

        expected = expected_by_id.get(evidence_id)
        if expected is None:
            raise ContractDriftError(f"unexpected evidence_id: {evidence_id}")
        if status != expected["status"]:
            raise ContractDriftError(
                f"{evidence_id} status drift: expected {expected['status']}, found {status}"
            )
        expected_summary = str(expected["summary"])
        if summary != expected_summary:
            raise ContractDriftError(
                f"{evidence_id} summary drift: expected {expected_summary!r}, found {summary!r}"
            )
        expected_blocking_refs = list(expected["blocking_refs"])
        if blocking_refs != expected_blocking_refs:
            raise ContractDriftError(
                f"{evidence_id} blocking_refs drift: expected {expected_blocking_refs}, "
                f"found {blocking_refs}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            f"evidence_id ordering drift: expected {list(expected_order)}, found {actual_order}"
        )


def validate_ev_artifact_mapping_contract(
    ev_contract_mapping: Sequence[dict[str, str]],
) -> None:
    expected_order = tuple(
        str(row["evidence_id"]) for row in BASELINE_EV_ARTIFACT_CONTRACT
    )
    expected_by_id = {
        str(row["evidence_id"]): row for row in BASELINE_EV_ARTIFACT_CONTRACT
    }

    if len(ev_contract_mapping) != len(expected_order):
        raise ContractDriftError(
            "ev_contract_mapping row-count drift: "
            f"expected {len(expected_order)}, found {len(ev_contract_mapping)}"
        )

    seen_ids: set[str] = set()
    actual_order: list[str] = []
    for index, raw_row in enumerate(ev_contract_mapping):
        context = f"ev_contract_mapping[{index}]"
        row = require_mapping(raw_row, context=context)
        require_exact_keys(row, expected_keys=EV_ARTIFACT_MAPPING_KEYS, context=context)
        evidence_id = require_non_empty_string(row, key="evidence_id", context=context)
        artifact_path = require_non_empty_string(row, key="artifact_path", context=context)

        if evidence_id in seen_ids:
            raise ContractDriftError(f"duplicate evidence_id detected: {evidence_id}")
        seen_ids.add(evidence_id)
        actual_order.append(evidence_id)

        expected = expected_by_id.get(evidence_id)
        if expected is None:
            raise ContractDriftError(f"unexpected evidence_id in ev_contract_mapping: {evidence_id}")
        expected_artifact_path = str(expected["artifact_path"])
        if artifact_path != expected_artifact_path:
            raise ContractDriftError(
                f"{evidence_id} artifact_path drift: expected {expected_artifact_path!r}, "
                f"found {artifact_path!r}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            "ev_contract_mapping ordering drift: "
            f"expected {list(expected_order)}, found {actual_order}"
        )


def validate_gate_results_contract(gate_results: list[dict[str, str]]) -> None:
    if len(gate_results) != len(BASE_GATE_SEQUENCE):
        raise ContractDriftError(
            "gate_results row-count drift: "
            f"expected {len(BASE_GATE_SEQUENCE)}, found {len(gate_results)}"
        )

    actual_sequence: list[str] = []
    for index, raw_gate in enumerate(gate_results):
        context = f"gate_results[{index}]"
        gate = require_mapping(raw_gate, context=context)
        require_exact_keys(gate, expected_keys=GATE_RESULT_KEYS, context=context)
        gate_id = require_non_empty_string(gate, key="gate_id", context=context)
        status = require_non_empty_string(gate, key="status", context=context)
        rationale = require_non_empty_string(gate, key="rationale", context=context)
        actual_sequence.append(gate_id)

        if status not in VALID_GATE_STATUSES:
            raise ContractHardFailError(
                f"{context}.status: unsupported gate status '{status}'"
            )

        expected = next(
            (row for row in BASELINE_GATE_CONTRACT if row["gate_id"] == gate_id),
            None,
        )
        if expected is None:
            raise ContractDriftError(f"unexpected gate_id: {gate_id}")
        expected_status = str(expected["status"])
        if status != expected_status:
            raise ContractDriftError(
                f"{gate_id} status drift: expected {expected_status}, found {status}"
            )
        expected_rationale = str(expected["rationale"])
        if rationale != expected_rationale:
            raise ContractDriftError(
                f"{gate_id} rationale drift: expected {expected_rationale!r}, found {rationale!r}"
            )

    if tuple(actual_sequence) != BASE_GATE_SEQUENCE:
        raise ContractDriftError(
            f"gate ordering drift: expected {list(BASE_GATE_SEQUENCE)}, found {actual_sequence}"
        )


def validate_consumer_contract(downstream_handoffs: list[dict[str, object]]) -> None:
    expected_order = tuple(
        str(row["consumer_seed"]) for row in BASELINE_CONSUMER_CONTRACT
    )
    expected_by_seed = {
        str(row["consumer_seed"]): row for row in BASELINE_CONSUMER_CONTRACT
    }

    if len(downstream_handoffs) != len(expected_order):
        raise ContractDriftError(
            "downstream_handoffs row-count drift: "
            f"expected {len(expected_order)}, found {len(downstream_handoffs)}"
        )

    seen: set[str] = set()
    actual_order: list[str] = []
    for index, raw_handoff in enumerate(downstream_handoffs):
        context = f"downstream_handoffs[{index}]"
        handoff = require_mapping(raw_handoff, context=context)
        require_exact_keys(handoff, expected_keys=CONSUMER_HANDOFF_KEYS, context=context)
        consumer_seed = require_non_empty_string(handoff, key="consumer_seed", context=context)
        required_inputs = require_string_list(handoff, key="required_inputs", context=context)
        handoff_state = require_non_empty_string(handoff, key="handoff_state", context=context)
        handoff_note = require_non_empty_string(handoff, key="handoff_note", context=context)

        if consumer_seed in seen:
            raise ContractDriftError(f"duplicate consumer_seed detected: {consumer_seed}")
        seen.add(consumer_seed)
        actual_order.append(consumer_seed)

        expected = expected_by_seed.get(consumer_seed)
        if expected is None:
            raise ContractDriftError(f"unexpected consumer_seed: {consumer_seed}")

        expected_required_inputs = list(expected["required_inputs"])
        if required_inputs != expected_required_inputs:
            raise ContractDriftError(
                f"{consumer_seed} required_inputs drift: expected {expected_required_inputs}, "
                f"found {required_inputs}"
            )

        expected_handoff_state = str(expected["handoff_state"])
        if handoff_state != expected_handoff_state:
            raise ContractDriftError(
                f"{consumer_seed} handoff_state drift: expected {expected_handoff_state}, "
                f"found {handoff_state}"
            )
        expected_handoff_note = str(expected["handoff_note"])
        if handoff_note != expected_handoff_note:
            raise ContractDriftError(
                f"{consumer_seed} handoff_note drift: expected {expected_handoff_note!r}, "
                f"found {handoff_note!r}"
            )

    if tuple(actual_order) != expected_order:
        raise ContractDriftError(
            "downstream consumer ordering drift: "
            f"expected {list(expected_order)}, found {actual_order}"
        )


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


def determine_qg04_result(
    gate_results: list[dict[str, str]], active_exception_ids: list[str]
) -> str:
    statuses = {item["status"] for item in gate_results}
    if "blocked" in statuses:
        return "blocked"
    if "fail" in statuses:
        return "fail"
    if statuses == {"pass"}:
        return "conditional-pass" if active_exception_ids else "pass"
    return "conditional-pass"


def recommendation_signal_for(qg04_result: str) -> str:
    return {
        "pass": "go-candidate",
        "conditional-pass": "conditional-go-candidate",
        "fail": "no-go",
        "blocked": "hold",
    }[qg04_result]


def determine_decision(qg04_result: str) -> str:
    return "approve" if qg04_result == "pass" else "hold"


def build_quality_gate_decision_artifacts(
    generated_at: str,
) -> QualityGateDecisionArtifacts:
    qg04_result = determine_qg04_result(BASE_GATE_RESULTS, ACTIVE_EXCEPTION_IDS)
    recommendation_signal = recommendation_signal_for(qg04_result)
    decision = determine_decision(qg04_result)

    validate_baseline_contract(
        generated_at=generated_at,
        decision=decision,
        qg04_result=qg04_result,
        recommendation_signal=recommendation_signal,
        gate_results=BASE_GATE_RESULTS,
        ev_contract_mapping=EV_ARTIFACT_MAPPING,
        evidence_items=EVIDENCE_ITEMS,
        downstream_handoffs=DOWNSTREAM_HANDOFFS,
    )

    return QualityGateDecisionArtifacts(
        markdown=render_markdown(
            inputs=REPORT_INPUTS,
            generated_at=generated_at,
            decision=decision,
            qg04_result=qg04_result,
            recommendation_signal=recommendation_signal,
            gate_results=BASE_GATE_RESULTS,
        ),
        status=render_status(
            inputs=REPORT_INPUTS,
            generated_at=generated_at,
            decision=decision,
            qg04_result=qg04_result,
            recommendation_signal=recommendation_signal,
            gate_results=BASE_GATE_RESULTS,
        ),
        decision=decision,
        qg04_result=qg04_result,
        recommendation_signal=recommendation_signal,
        evidence_item_count=len(EVIDENCE_ITEMS),
    )


def quality_gate_status_line(artifacts: QualityGateDecisionArtifacts) -> str:
    return (
        "quality-gate-generator: OK "
        f"(decision={artifacts.decision}, qg04={artifacts.qg04_result}, "
        f"recommendation={artifacts.recommendation_signal}, "
        f"ev_items={artifacts.evidence_item_count})"
    )
