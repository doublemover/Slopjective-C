"""Public facade for quality-gate baseline contract validation."""

from __future__ import annotations

from typing import Sequence

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
from .decision_errors import ContractDriftError, ContractHardFailError
from .decision_logic import determine_decision, recommendation_signal_for
from .decision_validation_contracts import (
    validate_consumer_contract,
    validate_ev_artifact_mapping_contract,
    validate_evidence_contract,
    validate_gate_results_contract,
)
from .decision_validation_decision import (
    validate_baseline_contract,
    validate_decision_semantics,
)
from .decision_validation_helpers import (
    require_exact_keys,
    require_mapping,
    require_non_empty_string,
    require_string_list,
)

__all__ = [
    "require_exact_keys",
    "require_mapping",
    "require_non_empty_string",
    "require_string_list",
    "validate_baseline_contract",
    "validate_consumer_contract",
    "validate_decision_semantics",
    "validate_ev_artifact_mapping_contract",
    "validate_evidence_contract",
    "validate_gate_results_contract",
]
