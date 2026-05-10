"""Row-level baseline contract validation for quality-gate decisions."""

from __future__ import annotations

from typing import Sequence

from .decision_errors import ContractDriftError, ContractHardFailError
from .decision_validation_helpers import (
    require_exact_keys,
    require_mapping,
    require_non_empty_string,
    require_string_list,
)
from .decision_validation_policy import (
    CONSUMER_HANDOFF_RULE,
    EVIDENCE_CONTRACT_RULE,
    EV_ARTIFACT_MAPPING_RULE,
    EXPECTED_CONSUMER_BY_SEED,
    EXPECTED_EV_ARTIFACT_BY_EVIDENCE_ID,
    EXPECTED_EVIDENCE_BY_ID,
    EXPECTED_GATE_BY_ID,
    GATE_RESULTS_RULE,
    VALID_EVIDENCE_STATUSES,
    VALID_GATE_STATUSES,
)
from .decision_validation_violations import FieldDrift, OrderingDrift, RowCountDrift


def validate_evidence_contract(evidence_items: list[dict[str, object]]) -> None:
    if len(evidence_items) != len(EVIDENCE_CONTRACT_RULE.expected_order):
        raise ContractDriftError(
            RowCountDrift(
                EVIDENCE_CONTRACT_RULE.row_count_subject,
                len(EVIDENCE_CONTRACT_RULE.expected_order),
                len(evidence_items),
            ).message()
        )

    seen_ids: set[str] = set()
    actual_order: list[str] = []
    for index, raw_item in enumerate(evidence_items):
        context = f"evidence_items[{index}]"
        item = require_mapping(raw_item, context=context)
        require_exact_keys(
            item, expected_keys=EVIDENCE_CONTRACT_RULE.expected_keys, context=context
        )
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

        expected = EXPECTED_EVIDENCE_BY_ID.get(evidence_id)
        if expected is None:
            raise ContractDriftError(f"unexpected evidence_id: {evidence_id}")
        if status != expected["status"]:
            raise ContractDriftError(
                FieldDrift(evidence_id, "status", expected["status"], status).message()
            )
        expected_summary = str(expected["summary"])
        if summary != expected_summary:
            raise ContractDriftError(
                FieldDrift(
                    evidence_id,
                    "summary",
                    expected_summary,
                    summary,
                    quote_values=True,
                ).message()
            )
        expected_blocking_refs = list(expected["blocking_refs"])
        if blocking_refs != expected_blocking_refs:
            raise ContractDriftError(
                FieldDrift(
                    evidence_id,
                    "blocking_refs",
                    expected_blocking_refs,
                    blocking_refs,
                ).message()
            )

    if tuple(actual_order) != EVIDENCE_CONTRACT_RULE.expected_order:
        raise ContractDriftError(
            OrderingDrift(
                EVIDENCE_CONTRACT_RULE.ordering_subject,
                EVIDENCE_CONTRACT_RULE.expected_order,
                actual_order,
            ).message()
        )


def validate_ev_artifact_mapping_contract(
    ev_contract_mapping: Sequence[dict[str, str]],
) -> None:
    if len(ev_contract_mapping) != len(EV_ARTIFACT_MAPPING_RULE.expected_order):
        raise ContractDriftError(
            RowCountDrift(
                EV_ARTIFACT_MAPPING_RULE.row_count_subject,
                len(EV_ARTIFACT_MAPPING_RULE.expected_order),
                len(ev_contract_mapping),
            ).message()
        )

    seen_ids: set[str] = set()
    actual_order: list[str] = []
    for index, raw_row in enumerate(ev_contract_mapping):
        context = f"ev_contract_mapping[{index}]"
        row = require_mapping(raw_row, context=context)
        require_exact_keys(
            row, expected_keys=EV_ARTIFACT_MAPPING_RULE.expected_keys, context=context
        )
        evidence_id = require_non_empty_string(row, key="evidence_id", context=context)
        artifact_path = require_non_empty_string(row, key="artifact_path", context=context)

        if evidence_id in seen_ids:
            raise ContractDriftError(f"duplicate evidence_id detected: {evidence_id}")
        seen_ids.add(evidence_id)
        actual_order.append(evidence_id)

        expected = EXPECTED_EV_ARTIFACT_BY_EVIDENCE_ID.get(evidence_id)
        if expected is None:
            raise ContractDriftError(
                f"unexpected evidence_id in ev_contract_mapping: {evidence_id}"
            )
        expected_artifact_path = str(expected["artifact_path"])
        if artifact_path != expected_artifact_path:
            raise ContractDriftError(
                FieldDrift(
                    evidence_id,
                    "artifact_path",
                    expected_artifact_path,
                    artifact_path,
                    quote_values=True,
                ).message()
            )

    if tuple(actual_order) != EV_ARTIFACT_MAPPING_RULE.expected_order:
        raise ContractDriftError(
            OrderingDrift(
                EV_ARTIFACT_MAPPING_RULE.ordering_subject,
                EV_ARTIFACT_MAPPING_RULE.expected_order,
                actual_order,
            ).message()
        )


def validate_gate_results_contract(gate_results: list[dict[str, str]]) -> None:
    if len(gate_results) != len(GATE_RESULTS_RULE.expected_order):
        raise ContractDriftError(
            RowCountDrift(
                GATE_RESULTS_RULE.row_count_subject,
                len(GATE_RESULTS_RULE.expected_order),
                len(gate_results),
            ).message()
        )

    actual_sequence: list[str] = []
    for index, raw_gate in enumerate(gate_results):
        context = f"gate_results[{index}]"
        gate = require_mapping(raw_gate, context=context)
        require_exact_keys(
            gate, expected_keys=GATE_RESULTS_RULE.expected_keys, context=context
        )
        gate_id = require_non_empty_string(gate, key="gate_id", context=context)
        status = require_non_empty_string(gate, key="status", context=context)
        rationale = require_non_empty_string(gate, key="rationale", context=context)
        actual_sequence.append(gate_id)

        if status not in VALID_GATE_STATUSES:
            raise ContractHardFailError(
                f"{context}.status: unsupported gate status '{status}'"
            )

        expected = EXPECTED_GATE_BY_ID.get(gate_id)
        if expected is None:
            raise ContractDriftError(f"unexpected gate_id: {gate_id}")
        expected_status = str(expected["status"])
        if status != expected_status:
            raise ContractDriftError(
                FieldDrift(gate_id, "status", expected_status, status).message()
            )
        expected_rationale = str(expected["rationale"])
        if rationale != expected_rationale:
            raise ContractDriftError(
                FieldDrift(
                    gate_id,
                    "rationale",
                    expected_rationale,
                    rationale,
                    quote_values=True,
                ).message()
            )

    if tuple(actual_sequence) != GATE_RESULTS_RULE.expected_order:
        raise ContractDriftError(
            OrderingDrift(
                GATE_RESULTS_RULE.ordering_subject,
                GATE_RESULTS_RULE.expected_order,
                actual_sequence,
            ).message()
        )


def validate_consumer_contract(downstream_handoffs: list[dict[str, object]]) -> None:
    if len(downstream_handoffs) != len(CONSUMER_HANDOFF_RULE.expected_order):
        raise ContractDriftError(
            RowCountDrift(
                CONSUMER_HANDOFF_RULE.row_count_subject,
                len(CONSUMER_HANDOFF_RULE.expected_order),
                len(downstream_handoffs),
            ).message()
        )

    seen: set[str] = set()
    actual_order: list[str] = []
    for index, raw_handoff in enumerate(downstream_handoffs):
        context = f"downstream_handoffs[{index}]"
        handoff = require_mapping(raw_handoff, context=context)
        require_exact_keys(
            handoff, expected_keys=CONSUMER_HANDOFF_RULE.expected_keys, context=context
        )
        consumer_seed = require_non_empty_string(
            handoff, key="consumer_seed", context=context
        )
        required_inputs = require_string_list(
            handoff, key="required_inputs", context=context
        )
        handoff_state = require_non_empty_string(
            handoff, key="handoff_state", context=context
        )
        handoff_note = require_non_empty_string(
            handoff, key="handoff_note", context=context
        )

        if consumer_seed in seen:
            raise ContractDriftError(f"duplicate consumer_seed detected: {consumer_seed}")
        seen.add(consumer_seed)
        actual_order.append(consumer_seed)

        expected = EXPECTED_CONSUMER_BY_SEED.get(consumer_seed)
        if expected is None:
            raise ContractDriftError(f"unexpected consumer_seed: {consumer_seed}")

        expected_required_inputs = list(expected["required_inputs"])
        if required_inputs != expected_required_inputs:
            raise ContractDriftError(
                FieldDrift(
                    consumer_seed,
                    "required_inputs",
                    expected_required_inputs,
                    required_inputs,
                ).message()
            )

        expected_handoff_state = str(expected["handoff_state"])
        if handoff_state != expected_handoff_state:
            raise ContractDriftError(
                FieldDrift(
                    consumer_seed,
                    "handoff_state",
                    expected_handoff_state,
                    handoff_state,
                ).message()
            )
        expected_handoff_note = str(expected["handoff_note"])
        if handoff_note != expected_handoff_note:
            raise ContractDriftError(
                FieldDrift(
                    consumer_seed,
                    "handoff_note",
                    expected_handoff_note,
                    handoff_note,
                    quote_values=True,
                ).message()
            )

    if tuple(actual_order) != CONSUMER_HANDOFF_RULE.expected_order:
        raise ContractDriftError(
            OrderingDrift(
                CONSUMER_HANDOFF_RULE.ordering_subject,
                CONSUMER_HANDOFF_RULE.expected_order,
                actual_order,
            ).message()
        )


__all__ = [
    "validate_consumer_contract",
    "validate_ev_artifact_mapping_contract",
    "validate_evidence_contract",
    "validate_gate_results_contract",
]
