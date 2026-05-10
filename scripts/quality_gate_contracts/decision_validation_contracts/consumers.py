"""Downstream consumer handoff contract validation."""

from __future__ import annotations

from ..decision_errors import ContractDriftError
from ..decision_validation_helpers import (
    require_exact_keys,
    require_mapping,
    require_non_empty_string,
    require_string_list,
)
from ..decision_validation_violations import FieldDrift
from .assertions import (
    require_contract_order,
    require_contract_row_count,
    require_unique_id,
)
from .catalog import CONSUMER_HANDOFF_RULE, EXPECTED_CONSUMER_BY_SEED


def validate_consumer_contract(downstream_handoffs: list[dict[str, object]]) -> None:
    require_contract_row_count(downstream_handoffs, rule=CONSUMER_HANDOFF_RULE)

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

        require_unique_id(consumer_seed, seen=seen, label="consumer_seed")
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

    require_contract_order(actual_order, rule=CONSUMER_HANDOFF_RULE)


__all__ = ["validate_consumer_contract"]
