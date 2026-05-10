"""Quality-gate decision artifact assembly."""

from __future__ import annotations

from .decision_data import (
    ACTIVE_EXCEPTION_IDS,
    BASE_GATE_RESULTS,
    DOWNSTREAM_HANDOFFS,
    EVIDENCE_ITEMS,
    EV_ARTIFACT_MAPPING,
    REPORT_INPUTS,
)
from .decision_logic import determine_decision
from .decision_logic import determine_qg04_result
from .decision_logic import recommendation_signal_for
from .decision_models import QualityGateDecisionArtifacts
from .decision_validation import validate_baseline_contract
from .reports import render_markdown, render_status


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


__all__ = ["build_quality_gate_decision_artifacts", "quality_gate_status_line"]
