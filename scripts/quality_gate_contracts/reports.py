from __future__ import annotations

from dataclasses import dataclass
from typing import Sequence


@dataclass(frozen=True)
class QualityGateReportInputs:
    contract_id: str
    seed_id: str
    acceptance_gate_id: str
    task_id: str
    release_label: str
    release_id: str
    source_revision: str
    validated_by: str
    validation_command_refs: list[str]
    ev_artifact_mapping: Sequence[dict[str, str]]
    evidence_items: list[dict[str, object]]
    threshold_results: list[dict[str, object]]
    active_exception_ids: list[str]
    unresolved_blockers: list[dict[str, str]]
    downstream_handoffs: list[dict[str, object]]
    acceptance_rollup: list[dict[str, str]]


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
        "",
        "## EV-06..EV-08 Evidence Register",
        "",
        "| EV ID | Status | Summary | Blocking refs |",
        "| --- | --- | --- | --- |",
    ]

    for item in inputs.evidence_items:
        refs = ", ".join(item["blocking_refs"]) if item["blocking_refs"] else "none"
        lines.append(
            f"| `{item['evidence_id']}` | `{item['status']}` | {item['summary']} | {refs} |"
        )

    lines.extend(
        [
            "",
            "## Gate Results (`QG-01`..`QG-04`)",
            "",
            "| Gate ID | Result | Rationale |",
            "| --- | --- | --- |",
        ]
    )

    for gate in gate_results:
        lines.append(f"| `{gate['gate_id']}` | `{gate['status']}` | {gate['rationale']} |")

    lines.append(
        "| `QG-04` | "
        f"`{qg04_result}` | Precedence result from `QG-01`..`QG-03` and active exception set (`{len(inputs.active_exception_ids)}`). |"
    )

    lines.extend(
        [
            "",
            "## EV Contract Mapping",
            "",
            "| EV ID | Artifact path | Baseline status |",
            "| --- | --- | --- |",
        ]
    )

    ev_status_by_id = {
        item["evidence_id"]: item["status"] for item in inputs.evidence_items
    }
    for row in inputs.ev_artifact_mapping:
        evidence_id = row["evidence_id"]
        lines.append(
            f"| `{evidence_id}` | "
            f"`{row['artifact_path']}` | "
            f"`{ev_status_by_id[evidence_id]}` |"
        )

    lines.extend(
        [
            "",
            "## Threshold Results Snapshot",
            "",
            "| Threshold ID | Gate ID | Observed metric | Pass threshold | Result |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for item in inputs.threshold_results:
        lines.append(
            "| "
            f"`{item['threshold_id']}` | "
            f"`{item['gate_id']}` | "
            f"`{item['observed_metric']}` | "
            f"`{item['pass_threshold']}` | "
            f"`{item['status']}` |"
        )

    lines.extend(
        [
            "",
            "## Active Exception Set (`EV-06` linkage)",
            "",
            "- `exception_ledger_path`: `reports/releases/v011_quality_gate_exceptions.md`",
            f"- `active_exception_ids`: `{inputs.active_exception_ids}`",
            f"- `active_exception_count`: `{len(inputs.active_exception_ids)}`",
            "",
            "## Unresolved Blocker Posture",
            "",
            "| Blocker ID | Status | Owner | Due date (UTC) | Due path |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for blocker in inputs.unresolved_blockers:
        lines.append(
            f"| `{blocker['blocker_id']}` | "
            f"`{blocker['status']}` | "
            f"`{blocker['owner']}` | "
            f"`{blocker['due_date_utc']}` | "
            f"`{blocker['due_path']}` |"
        )

    lines.extend(
        [
            "",
            "## Downstream Handoff Notes",
            "",
            "| Consumer seed | Required inputs | Handoff state | Handoff note |",
            "| --- | --- | --- | --- |",
        ]
    )

    for handoff in inputs.downstream_handoffs:
        required_inputs = ", ".join(handoff["required_inputs"])
        lines.append(
            f"| `{handoff['consumer_seed']}` | "
            f"`{required_inputs}` | "
            f"`{handoff['handoff_state']}` | "
            f"{handoff['handoff_note']} |"
        )

    lines.extend(
        [
            "",
            "## Acceptance Rollup (`AC-V013-CONF-02`)",
            "",
            "| Acceptance ID | Status | Summary |",
            "| --- | --- | --- |",
        ]
    )

    for row in inputs.acceptance_rollup:
        lines.append(
            f"| `{row['acceptance_id']}` | `{row['status']}` | {row['summary']} |"
        )

    lines.extend(
        [
            "",
            "## Validation Command References",
            "",
        ]
    )

    for command in inputs.validation_command_refs:
        lines.append(f"- `{command}`")

    lines.extend(
        [
            "",
            "## Deterministic Rule",
            "",
            "- If any of `QG-01`, `QG-02`, or `QG-03` is `blocked`, then `QG-04=blocked`.",
            "- Else if any of `QG-01`, `QG-02`, or `QG-03` is `fail`, then `QG-04=fail`.",
            "- Else if all are `pass`, `QG-04=pass` when no active exceptions exist; otherwise `QG-04=conditional-pass`.",
            "- `recommendation_signal` is derived from `QG-04` (`pass` -> `go-candidate`; `conditional-pass` -> `conditional-go-candidate`; `fail` -> `no-go`; `blocked` -> `hold`).",
            "- `overall_decision` is `approve` only when `QG-04=pass`; otherwise `hold`.",
            "",
        ]
    )
    return "\n".join(lines)


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
