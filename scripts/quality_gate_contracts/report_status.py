from __future__ import annotations

from .report_inputs import QualityGateReportInputs


def render_status(
    *,
    inputs: QualityGateReportInputs,
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
) -> dict[str, object]:
    gate_results_with_qg04 = gate_results + [
        {
            "gate_id": "QG-04",
            "status": qg04_result,
            "rationale": (
                "Integrated precedence output derived from QG-01..QG-03 and active exception set."
            ),
        }
    ]
    downstream_consumers = [
        handoff["consumer_seed"] for handoff in inputs.downstream_handoffs
    ]

    return {
        "contract_id": inputs.contract_id,
        "seed_id": inputs.seed_id,
        "acceptance_gate_id": inputs.acceptance_gate_id,
        "task_id": inputs.task_id,
        "release_label": inputs.release_label,
        "release_id": inputs.release_id,
        "source_revision": inputs.source_revision,
        "generated_at_utc": generated_at,
        "validated_by": inputs.validated_by,
        "validation_command_refs": inputs.validation_command_refs,
        "ev_contract_mapping": list(inputs.ev_artifact_mapping),
        "evidence_items": inputs.evidence_items,
        "gate_results": gate_results_with_qg04,
        "threshold_results": inputs.threshold_results,
        "active_exception_ids": inputs.active_exception_ids,
        "exception_ledger": {
            "artifact_path": "reports/releases/v011_quality_gate_exceptions.md",
            "active_exception_count": len(inputs.active_exception_ids),
            "active_exception_budget_max": 2,
            "max_per_gate_domain": 1,
        },
        "qg_04_result": qg04_result,
        "recommendation_signal": recommendation_signal,
        "overall_decision": decision,
        "unresolved_blockers": inputs.unresolved_blockers,
        "downstream_consumers": downstream_consumers,
        "downstream_handoffs": inputs.downstream_handoffs,
        "acceptance_rollup": inputs.acceptance_rollup,
    }


__all__ = ("render_status",)
