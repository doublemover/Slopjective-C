"""Evidence row contract validation."""

from __future__ import annotations

from ..decision_errors import ContractDriftError, ContractHardFailError
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
from .catalog import (
    EVIDENCE_CONTRACT_RULE,
    EXPECTED_EVIDENCE_BY_ID,
    VALID_EVIDENCE_STATUSES,
)


def validate_evidence_contract(evidence_items: list[dict[str, object]]) -> None:
    require_contract_row_count(evidence_items, rule=EVIDENCE_CONTRACT_RULE)

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

        require_unique_id(evidence_id, seen=seen_ids, label="evidence_id")
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

    require_contract_order(actual_order, rule=EVIDENCE_CONTRACT_RULE)


__all__ = ["validate_evidence_contract"]
