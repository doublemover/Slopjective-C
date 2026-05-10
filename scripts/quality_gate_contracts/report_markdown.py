from __future__ import annotations

from .report_inputs import QualityGateReportInputs
from .report_markdown_sections import (
    append_acceptance_rollup,
    append_deterministic_rule,
    append_downstream_handoff_notes,
    append_evidence_register,
    append_exception_set,
    append_gate_results,
    append_threshold_results,
    append_unresolved_blockers,
    append_validation_commands,
    append_ev_contract_mapping,
)


def render_markdown(
    *,
    inputs: QualityGateReportInputs,
    generated_at: str,
    decision: str,
    qg04_result: str,
    recommendation_signal: str,
    gate_results: list[dict[str, str]],
) -> str:
    lines = [
        f"# v0.11 Quality Gate Decision (`{inputs.seed_id}`)",
        "",
        f"_Generated at {generated_at}_",
        "",
        "## Decision Summary",
        "",
        f"- `contract_id`: `{inputs.contract_id}`",
        f"- `seed_id`: `{inputs.seed_id}`",
        f"- `acceptance_gate_id`: `{inputs.acceptance_gate_id}`",
        f"- `task_id`: `{inputs.task_id}`",
        f"- `release_label`: `{inputs.release_label}`",
        f"- `release_id`: `{inputs.release_id}`",
        f"- `source_revision`: `{inputs.source_revision}`",
        f"- `overall_decision`: `{decision}`",
        f"- `QG-04`: `{qg04_result}`",
        f"- `recommendation_signal`: `{recommendation_signal}`",
    ]

    append_evidence_register(lines, inputs)
    append_gate_results(lines, inputs, gate_results, qg04_result)
    append_ev_contract_mapping(lines, inputs)
    append_threshold_results(lines, inputs)
    append_exception_set(lines, inputs)
    append_unresolved_blockers(lines, inputs)
    append_downstream_handoff_notes(lines, inputs)
    append_acceptance_rollup(lines, inputs)
    append_validation_commands(lines, inputs)
    append_deterministic_rule(lines)
    return "\n".join(lines)


__all__ = ("render_markdown",)
