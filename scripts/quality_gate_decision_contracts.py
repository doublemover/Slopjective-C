from __future__ import annotations

if __package__:
    from .quality_gate_contracts.decision_builder import (
        build_quality_gate_decision_artifacts,
        quality_gate_status_line,
    )
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
    from .quality_gate_contracts.decision_errors import (
        ContractDriftError,
        ContractHardFailError,
    )
    from .quality_gate_contracts.decision_logic import (
        determine_decision,
        determine_qg04_result,
        recommendation_signal_for,
    )
    from .quality_gate_contracts.decision_models import QualityGateDecisionArtifacts
    from .quality_gate_contracts.decision_validation import (
        require_exact_keys,
        require_mapping,
        require_non_empty_string,
        require_string_list,
        validate_baseline_contract,
        validate_consumer_contract,
        validate_decision_semantics,
        validate_ev_artifact_mapping_contract,
        validate_evidence_contract,
        validate_gate_results_contract,
    )
    from .quality_gate_contracts.reports import render_markdown, render_status
else:
    from quality_gate_contracts.decision_builder import (
        build_quality_gate_decision_artifacts,
        quality_gate_status_line,
    )
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
    from quality_gate_contracts.decision_errors import (
        ContractDriftError,
        ContractHardFailError,
    )
    from quality_gate_contracts.decision_logic import (
        determine_decision,
        determine_qg04_result,
        recommendation_signal_for,
    )
    from quality_gate_contracts.decision_models import QualityGateDecisionArtifacts
    from quality_gate_contracts.decision_validation import (
        require_exact_keys,
        require_mapping,
        require_non_empty_string,
        require_string_list,
        validate_baseline_contract,
        validate_consumer_contract,
        validate_decision_semantics,
        validate_ev_artifact_mapping_contract,
        validate_evidence_contract,
        validate_gate_results_contract,
    )
    from quality_gate_contracts.reports import render_markdown, render_status

__all__ = [
    "ACCEPTANCE_GATE_ID",
    "ACCEPTANCE_ROLLUP",
    "ACTIVE_EXCEPTION_IDS",
    "BASELINE_CONSUMER_CONTRACT",
    "BASELINE_EV_ARTIFACT_CONTRACT",
    "BASELINE_EVIDENCE_CONTRACT",
    "BASELINE_GATE_CONTRACT",
    "BASE_GATE_RESULTS",
    "BASE_GATE_SEQUENCE",
    "CONSUMER_HANDOFF_KEYS",
    "CONTRACT_ID",
    "ContractDriftError",
    "ContractHardFailError",
    "DOWNSTREAM_HANDOFFS",
    "EVIDENCE_ITEMS",
    "EVIDENCE_ITEM_KEYS",
    "EV_ARTIFACT_MAPPING",
    "EV_ARTIFACT_MAPPING_KEYS",
    "GATE_RESULT_KEYS",
    "QualityGateDecisionArtifacts",
    "REPORT_INPUTS",
    "THRESHOLD_RESULTS",
    "UNRESOLVED_BLOCKERS",
    "VALIDATION_COMMAND_REFS",
    "VALID_EVIDENCE_STATUSES",
    "VALID_GATE_STATUSES",
    "build_quality_gate_decision_artifacts",
    "determine_decision",
    "determine_qg04_result",
    "quality_gate_status_line",
    "recommendation_signal_for",
    "render_markdown",
    "render_status",
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
