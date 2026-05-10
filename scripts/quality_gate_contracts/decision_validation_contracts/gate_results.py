"""Gate result contract validation."""

from __future__ import annotations

from ..decision_errors import ContractDriftError, ContractHardFailError
from ..decision_validation_helpers import (
    require_exact_keys,
    require_mapping,
    require_non_empty_string,
)
from ..decision_validation_violations import FieldDrift
from .assertions import require_contract_order, require_contract_row_count
from .catalog import EXPECTED_GATE_BY_ID, GATE_RESULTS_RULE, VALID_GATE_STATUSES


def validate_gate_results_contract(gate_results: list[dict[str, str]]) -> None:
    require_contract_row_count(gate_results, rule=GATE_RESULTS_RULE)

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

    require_contract_order(actual_sequence, rule=GATE_RESULTS_RULE)


__all__ = ["validate_gate_results_contract"]
