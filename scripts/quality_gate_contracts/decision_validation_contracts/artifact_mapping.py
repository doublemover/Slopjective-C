"""EV artifact mapping contract validation."""

from __future__ import annotations

from typing import Sequence

from ..decision_errors import ContractDriftError
from ..decision_validation_helpers import (
    require_exact_keys,
    require_mapping,
    require_non_empty_string,
)
from ..decision_validation_violations import FieldDrift
from .assertions import (
    require_contract_order,
    require_contract_row_count,
    require_unique_id,
)
from .catalog import EV_ARTIFACT_MAPPING_RULE, EXPECTED_EV_ARTIFACT_BY_EVIDENCE_ID


def validate_ev_artifact_mapping_contract(
    ev_contract_mapping: Sequence[dict[str, str]],
) -> None:
    require_contract_row_count(ev_contract_mapping, rule=EV_ARTIFACT_MAPPING_RULE)

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

        require_unique_id(evidence_id, seen=seen_ids, label="evidence_id")
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

    require_contract_order(actual_order, rule=EV_ARTIFACT_MAPPING_RULE)


__all__ = ["validate_ev_artifact_mapping_contract"]
