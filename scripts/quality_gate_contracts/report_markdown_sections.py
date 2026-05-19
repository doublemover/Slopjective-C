from __future__ import annotations

from .report_inputs import QualityGateReportInputs


def append_evidence_register(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
    lines.extend(
        [
            "",
            "## EV-06..EV-08 Evidence Register",
            "",
            "| EV ID | Status | Summary | Blocking refs |",
            "| --- | --- | --- | --- |",
        ]
    )

    for item in inputs.evidence_items:
        refs = ", ".join(item["blocking_refs"]) if item["blocking_refs"] else "none"
        lines.append(
            f"| `{item['evidence_id']}` | `{item['status']}` | {item['summary']} | {refs} |"
        )


def append_gate_results(
    lines: list[str],
    inputs: QualityGateReportInputs,
    gate_results: list[dict[str, str]],
    qg04_result: str,
) -> None:
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


def append_ev_contract_mapping(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
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


def append_threshold_results(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
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


def append_exception_set(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
    lines.extend(
        [
            "",
            "## Active Exception Set (`EV-06` linkage)",
            "",
            "- `exception_ledger_path`: `reports/releases/v011_quality_gate_exceptions.md`",
            f"- `active_exception_ids`: `{inputs.active_exception_ids}`",
            f"- `active_exception_count`: `{len(inputs.active_exception_ids)}`",
        ]
    )


def append_unresolved_blockers(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
    lines.extend(
        [
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


def append_downstream_handoff_notes(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
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


def append_acceptance_rollup(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
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


def append_validation_commands(
    lines: list[str],
    inputs: QualityGateReportInputs,
) -> None:
    lines.extend(
        [
            "",
            "## Validation Command References",
            "",
        ]
    )

    for command in inputs.validation_command_refs:
        lines.append(f"- `{command}`")


def append_deterministic_rule(lines: list[str]) -> None:
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


__all__ = (
    "append_acceptance_rollup",
    "append_deterministic_rule",
    "append_downstream_handoff_notes",
    "append_evidence_register",
    "append_exception_set",
    "append_gate_results",
    "append_threshold_results",
    "append_unresolved_blockers",
    "append_validation_commands",
    "append_ev_contract_mapping",
)
