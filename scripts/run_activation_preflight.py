#!/usr/bin/env python3
"""Run deterministic activation preflight orchestration and persist evidence artifacts."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import textwrap
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG_JSON = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT / "spec" / "planning"
DEFAULT_OUTPUT_DIR = ROOT / "reports" / "activation_preflight"

ACTIVATION_CHECK_SCRIPT_PATH = ROOT / "scripts" / "check_activation_triggers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"
CAPTURE_SNAPSHOTS_SCRIPT_PATH = ROOT / "scripts" / "capture_activation_snapshots.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"

ACTIVATION_JSON_FILENAME = "check_activation_triggers.json"
ACTIVATION_MD_FILENAME = "check_activation_triggers.md"
SPEC_LINT_LOG_FILENAME = "spec_lint.log"
SNAPSHOT_CAPTURE_LOG_FILENAME = "capture_activation_snapshots.log"
OPEN_BLOCKERS_REFRESH_LOG_FILENAME = "extract_open_blockers.log"
SUMMARY_JSON_FILENAME = "activation_preflight_summary.json"
REPORT_MD_FILENAME = "activation_preflight_report.md"
OPEN_BLOCKERS_REFRESH_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
OPEN_BLOCKERS_TRIGGER_ID = "T5-OPEN-BLOCKERS"
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")

EXIT_GATE_CLOSED = 0
EXIT_GATE_OPEN = 1
EXIT_RUNNER_ERROR = 2


@dataclass(frozen=True)
class CommandSpec:
    name: str
    script_path: Path
    actual_args: tuple[str, ...]
    display_args: tuple[str, ...]


@dataclass(frozen=True)
class CommandResult:
    spec: CommandSpec
    exit_code: int
    stdout: str
    stderr: str


def normalize_newlines(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_repo_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / OPEN_BLOCKERS_REFRESH_RELATIVE_PATH


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(normalize_newlines(content), encoding="utf-8", newline="\n")


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def parse_non_negative_int(raw: str) -> int:
    try:
        value = int(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("must be an integer") from exc
    if value < 0:
        raise argparse.ArgumentTypeError("must be >= 0")
    return value


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES

    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        status = raw.strip().lower()
        if not status:
            raise ValueError("actionable status filters must be non-empty")
        if status in seen:
            continue
        seen.add(status)
        normalized.append(status)
    if not normalized:
        raise ValueError("no actionable statuses were provided")
    return tuple(normalized)


def markdown_freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def find_markdown_table_row(markdown_text: str, label: str) -> list[str] | None:
    row_prefix = f"| {label} |"
    for line in markdown_text.splitlines():
        if not line.startswith(row_prefix):
            continue
        cells = [cell.strip() for cell in line.split("|")[1:-1]]
        if len(cells) != 6:
            return []
        return cells
    return None


def validate_markdown_freshness_row(
    *,
    markdown_text: str,
    label: str,
    freshness_entry: dict[str, Any],
) -> str | None:
    cells = find_markdown_table_row(markdown_text, label)
    if cells is None:
        return (
            "check_activation_triggers(markdown) missing deterministic line "
            f"for freshness row {label!r}."
        )
    if not cells:
        return (
            "check_activation_triggers(markdown) malformed freshness row "
            f"for {label!r}: expected 6 columns."
        )

    expected_cells = (
        label,
        markdown_freshness_cell(freshness_entry.get("requested")),
        markdown_freshness_cell(freshness_entry.get("max_age_seconds")),
        markdown_freshness_cell(freshness_entry.get("generated_at_utc")),
        markdown_freshness_cell(freshness_entry.get("age_seconds")),
        markdown_freshness_cell(freshness_entry.get("fresh")),
    )
    if cells[0] != expected_cells[0]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: label={cells[0]!r} expected={expected_cells[0]!r}."
        )
    if cells[1] != expected_cells[1]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: requested={cells[1]!r} expected={expected_cells[1]!r}."
        )
    if cells[2] != expected_cells[2]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: max_age_seconds={cells[2]!r} expected={expected_cells[2]!r}."
        )
    if cells[3] != expected_cells[3]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: generated_at_utc={cells[3]!r} expected={expected_cells[3]!r}."
        )

    expected_age_cell = expected_cells[4]
    if expected_age_cell == "_none_":
        if cells[4] != "_none_":
            return (
                "check_activation_triggers(markdown) freshness row drift for "
                f"{label!r}: age_seconds={cells[4]!r} expected='_none_'."
            )
        if cells[5] != expected_cells[5]:
            return (
                "check_activation_triggers(markdown) freshness row drift for "
                f"{label!r}: fresh={cells[5]!r} expected={expected_cells[5]!r}."
            )
        return None

    if not (cells[4].startswith("`") and cells[4].endswith("`")):
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: age_seconds cell {cells[4]!r} is not markdown-quoted."
        )
    age_text = cells[4][1:-1]
    if not age_text.isdigit():
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: age_seconds={cells[4]!r} must be a non-negative integer."
        )
    if cells[5] != expected_cells[5]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: fresh={cells[5]!r} expected={expected_cells[5]!r}."
        )
    return None


def run_command(spec: CommandSpec) -> CommandResult:
    command = [sys.executable, str(spec.script_path), *spec.actual_args]
    proc = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    return CommandResult(
        spec=spec,
        exit_code=int(proc.returncode),
        stdout=normalize_newlines(proc.stdout),
        stderr=normalize_newlines(proc.stderr),
    )


def parse_activation_payload(
    result: CommandResult,
    *,
    expected_issues_path: Path,
    expected_milestones_path: Path,
    expected_catalog_path: Path,
    expected_open_blockers_path: Path | None,
    expected_t4_overlay_path: Path | None,
    expected_actionable_statuses: Sequence[str],
    expected_issues_max_age_seconds: int | None,
    expected_milestones_max_age_seconds: int | None,
) -> tuple[dict[str, Any] | None, str | None]:
    if result.exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN):
        return None, (
            "check_activation_triggers(json) returned unexpected exit code "
            f"{result.exit_code}."
        )

    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        return (
            None,
            (
                "check_activation_triggers(json) emitted invalid JSON: "
                f"{exc.msg} at {exc.lineno}:{exc.colno}."
            ),
        )

    if not isinstance(payload, dict):
        return None, "check_activation_triggers(json) output root must be an object."

    mode = payload.get("mode")
    if mode != "offline-deterministic":
        return None, (
            "check_activation_triggers(json) missing deterministic "
            "mode 'offline-deterministic'."
        )

    inputs = payload.get("inputs")
    if not isinstance(inputs, dict):
        return None, "check_activation_triggers(json) missing object 'inputs'."

    expected_issues_display_path = display_path(expected_issues_path)
    expected_milestones_display_path = display_path(expected_milestones_path)
    expected_catalog_display_path = display_path(expected_catalog_path)
    expected_open_blockers_display_path = (
        display_path(expected_open_blockers_path)
        if expected_open_blockers_path is not None
        else None
    )
    expected_t4_overlay_display_path = (
        display_path(expected_t4_overlay_path)
        if expected_t4_overlay_path is not None
        else None
    )

    issues_input = inputs.get("issues_json")
    if issues_input != expected_issues_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.issues_json={issues_input!r} expected={expected_issues_display_path!r}."
        )

    milestones_input = inputs.get("milestones_json")
    if milestones_input != expected_milestones_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.milestones_json={milestones_input!r} "
            f"expected={expected_milestones_display_path!r}."
        )

    catalog_input = inputs.get("catalog_json")
    if catalog_input != expected_catalog_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.catalog_json={catalog_input!r} expected={expected_catalog_display_path!r}."
        )

    open_blockers_input = inputs.get("open_blockers_json")
    if open_blockers_input != expected_open_blockers_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.open_blockers_json={open_blockers_input!r} "
            f"expected={expected_open_blockers_display_path!r}."
        )

    t4_overlay_input = inputs.get("t4_governance_overlay_json")
    if t4_overlay_input != expected_t4_overlay_display_path:
        return None, (
            "check_activation_triggers(json) inputs provenance drift: "
            f"inputs.t4_governance_overlay_json={t4_overlay_input!r} "
            f"expected={expected_t4_overlay_display_path!r}."
        )

    actionable_statuses = payload.get("actionable_statuses")
    if not isinstance(actionable_statuses, list) or not all(
        isinstance(status, str) and status for status in actionable_statuses
    ):
        return None, (
            "check_activation_triggers(json) missing non-empty string list "
            "'actionable_statuses'."
        )
    if len(set(actionable_statuses)) != len(actionable_statuses):
        return None, (
            "check_activation_triggers(json) has duplicate entries in "
            "'actionable_statuses'."
        )
    if tuple(actionable_statuses) != tuple(expected_actionable_statuses):
        return None, (
            "check_activation_triggers(json) actionable status drift: "
            f"actionable_statuses={actionable_statuses!r} "
            f"expected={list(expected_actionable_statuses)!r}."
        )

    freshness = payload.get("freshness")
    if not isinstance(freshness, dict):
        return None, "check_activation_triggers(json) missing object 'freshness'."

    def validate_freshness_entry(
        label: str,
        entry: Any,
        *,
        expected_max_age_seconds: int | None,
    ) -> str | None:
        if not isinstance(entry, dict):
            return f"check_activation_triggers(json) freshness.{label} must be an object."

        requested = entry.get("requested")
        if not isinstance(requested, bool):
            return (
                "check_activation_triggers(json) freshness."
                f"{label}.requested must be boolean."
            )

        max_age_seconds = entry.get("max_age_seconds")
        if max_age_seconds is not None and (
            isinstance(max_age_seconds, bool)
            or not isinstance(max_age_seconds, int)
            or max_age_seconds < 0
        ):
            return (
                "check_activation_triggers(json) freshness."
                f"{label}.max_age_seconds must be null or non-negative integer."
            )

        generated_at_utc = entry.get("generated_at_utc")
        if generated_at_utc is not None and (
            not isinstance(generated_at_utc, str) or not generated_at_utc
        ):
            return (
                "check_activation_triggers(json) freshness."
                f"{label}.generated_at_utc must be null or non-empty string."
            )

        age_seconds = entry.get("age_seconds")
        if age_seconds is not None and (
            isinstance(age_seconds, bool) or not isinstance(age_seconds, int) or age_seconds < 0
        ):
            return (
                "check_activation_triggers(json) freshness."
                f"{label}.age_seconds must be null or non-negative integer."
            )

        fresh = entry.get("fresh")
        if fresh is not None and not isinstance(fresh, bool):
            return (
                "check_activation_triggers(json) freshness."
                f"{label}.fresh must be null or boolean."
            )

        if expected_max_age_seconds is None:
            if requested:
                return (
                    "check_activation_triggers(json) freshness drift: "
                    f"freshness.{label}.requested={requested!r} expected=False."
                )
            if (
                max_age_seconds is not None
                or generated_at_utc is not None
                or age_seconds is not None
                or fresh is not None
            ):
                return (
                    "check_activation_triggers(json) freshness drift: "
                    f"freshness.{label} should use null metadata when request is omitted."
                )
            return None

        if not requested:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.requested={requested!r} expected=True."
            )
        if max_age_seconds != expected_max_age_seconds:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.max_age_seconds={max_age_seconds!r} "
                f"expected={expected_max_age_seconds!r}."
            )
        if not isinstance(generated_at_utc, str) or not generated_at_utc:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.generated_at_utc must be a non-empty string "
                "when freshness is requested."
            )
        if isinstance(age_seconds, bool) or not isinstance(age_seconds, int) or age_seconds < 0:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.age_seconds must be a non-negative integer "
                "when freshness is requested."
            )
        if fresh is not True:
            return (
                "check_activation_triggers(json) freshness drift: "
                f"freshness.{label}.fresh={fresh!r} expected=True."
            )
        return None

    issues_freshness_error = validate_freshness_entry(
        "issues",
        freshness.get("issues"),
        expected_max_age_seconds=expected_issues_max_age_seconds,
    )
    if issues_freshness_error is not None:
        return None, issues_freshness_error

    milestones_freshness_error = validate_freshness_entry(
        "milestones",
        freshness.get("milestones"),
        expected_max_age_seconds=expected_milestones_max_age_seconds,
    )
    if milestones_freshness_error is not None:
        return None, milestones_freshness_error

    gate_open = payload.get("gate_open")
    if not isinstance(gate_open, bool):
        return None, "check_activation_triggers(json) missing boolean 'gate_open'."

    payload_exit_code = payload.get("exit_code")
    if (
        isinstance(payload_exit_code, bool)
        or not isinstance(payload_exit_code, int)
        or payload_exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN)
    ):
        return None, (
            "check_activation_triggers(json) missing deterministic integer "
            "'exit_code' in {0,1}."
        )

    queue_state = payload.get("queue_state")
    if not isinstance(queue_state, str) or not queue_state:
        return None, "check_activation_triggers(json) missing non-empty 'queue_state'."

    activation_required = payload.get("activation_required")
    if not isinstance(activation_required, bool):
        return None, "check_activation_triggers(json) missing boolean 'activation_required'."

    active_trigger_ids = payload.get("active_trigger_ids")
    if not isinstance(active_trigger_ids, list) or not all(
        isinstance(item, str) for item in active_trigger_ids
    ):
        return None, "check_activation_triggers(json) missing string list 'active_trigger_ids'."
    if len(set(active_trigger_ids)) != len(active_trigger_ids):
        return None, "check_activation_triggers(json) has duplicate entries in 'active_trigger_ids'."

    trigger_rows = payload.get("triggers")
    if not isinstance(trigger_rows, list):
        return None, "check_activation_triggers(json) missing list 'triggers'."
    fired_trigger_ids: list[str] = []
    trigger_rows_by_id: dict[str, tuple[int, bool]] = {}
    for index, row in enumerate(trigger_rows):
        if not isinstance(row, dict):
            return None, f"check_activation_triggers(json) trigger row {index} must be an object."

        row_id = row.get("id")
        if not isinstance(row_id, str) or not row_id:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{index} missing non-empty string 'id'."
            )
        if row_id in trigger_rows_by_id:
            return None, (
                "check_activation_triggers(json) has duplicate trigger row id "
                f"{row_id!r}."
            )

        row_condition = row.get("condition")
        if not isinstance(row_condition, str) or not row_condition:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing non-empty string 'condition'."
            )

        row_count = row.get("count")
        if isinstance(row_count, bool) or not isinstance(row_count, int) or row_count < 0:
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing non-negative integer 'count'."
            )

        row_fired = row.get("fired")
        if not isinstance(row_fired, bool):
            return None, (
                "check_activation_triggers(json) trigger row "
                f"{row_id!r} missing boolean 'fired'."
            )

        trigger_rows_by_id[row_id] = (row_count, row_fired)
        if row_fired:
            fired_trigger_ids.append(row_id)

    if active_trigger_ids != fired_trigger_ids:
        return None, (
            "check_activation_triggers(json) active trigger ordering drift: "
            f"active_trigger_ids={active_trigger_ids!r} fired_ids={fired_trigger_ids!r}."
        )

    expected_activation_required = bool(active_trigger_ids)
    if activation_required is not expected_activation_required:
        return None, (
            "check_activation_triggers(json) activation reduction mismatch: "
            f"activation_required={activation_required!r} "
            f"expected={expected_activation_required!r}."
        )

    t4_overlay = payload.get("t4_governance_overlay")
    if not isinstance(t4_overlay, dict):
        return None, "check_activation_triggers(json) missing object 't4_governance_overlay'."

    t4_new_scope_publish = t4_overlay.get("new_scope_publish")
    if not isinstance(t4_new_scope_publish, bool):
        return None, (
            "check_activation_triggers(json) missing boolean "
            "'t4_governance_overlay.new_scope_publish'."
        )

    t4_source = t4_overlay.get("source")
    if not isinstance(t4_source, str) or not t4_source:
        return None, (
            "check_activation_triggers(json) missing non-empty string "
            "'t4_governance_overlay.source'."
        )

    expected_gate_open = activation_required or t4_new_scope_publish
    if gate_open is not expected_gate_open:
        return None, (
            "check_activation_triggers(json) gate reduction mismatch: "
            f"gate_open={gate_open!r} expected={expected_gate_open!r}."
        )

    open_blockers = payload.get("open_blockers")
    if not isinstance(open_blockers, dict):
        return None, "check_activation_triggers(json) missing object 'open_blockers'."

    open_blocker_count = open_blockers.get("count")
    if (
        isinstance(open_blocker_count, bool)
        or not isinstance(open_blocker_count, int)
        or open_blocker_count < 0
    ):
        return None, (
            "check_activation_triggers(json) missing non-negative integer "
            "'open_blockers.count'."
        )

    open_blockers_trigger_id = open_blockers.get("trigger_id")
    if open_blockers_trigger_id != OPEN_BLOCKERS_TRIGGER_ID:
        return None, (
            "check_activation_triggers(json) missing deterministic "
            f"'open_blockers.trigger_id={OPEN_BLOCKERS_TRIGGER_ID}'."
        )

    open_blockers_trigger_fired = open_blockers.get("trigger_fired")
    if not isinstance(open_blockers_trigger_fired, bool):
        return None, (
            "check_activation_triggers(json) missing boolean "
            "'open_blockers.trigger_fired'."
        )

    expected_open_blockers_trigger = open_blocker_count > 0
    if open_blockers_trigger_fired is not expected_open_blockers_trigger:
        return None, (
            "check_activation_triggers(json) inconsistent open blocker trigger state: "
            f"count={open_blocker_count} trigger_fired={open_blockers_trigger_fired}."
        )

    has_open_blockers_trigger = OPEN_BLOCKERS_TRIGGER_ID in active_trigger_ids
    if has_open_blockers_trigger is not expected_open_blockers_trigger:
        return None, (
            "check_activation_triggers(json) open blocker trigger mismatch: "
            f"active_trigger_ids contains {OPEN_BLOCKERS_TRIGGER_ID!r}="
            f"{has_open_blockers_trigger} while count={open_blocker_count}."
        )

    if OPEN_BLOCKERS_TRIGGER_ID not in trigger_rows_by_id:
        return None, (
            "check_activation_triggers(json) missing trigger row "
            f"{OPEN_BLOCKERS_TRIGGER_ID!r}."
        )
    blockers_row_count, blockers_row_fired = trigger_rows_by_id[OPEN_BLOCKERS_TRIGGER_ID]
    if blockers_row_count != open_blocker_count or blockers_row_fired is not open_blockers_trigger_fired:
        return None, (
            "check_activation_triggers(json) open blocker row drift: "
            f"trigger_row=(count={blockers_row_count}, fired={blockers_row_fired}) "
            f"payload=(count={open_blocker_count}, fired={open_blockers_trigger_fired})."
        )

    expected_queue_state = "dispatch-open" if gate_open else "idle"
    if queue_state != expected_queue_state:
        return None, (
            "check_activation_triggers(json) queue_state drift: "
            f"queue_state={queue_state!r} expected={expected_queue_state!r}."
        )

    expected_exit = EXIT_GATE_OPEN if gate_open else EXIT_GATE_CLOSED
    if payload_exit_code != expected_exit:
        return (
            None,
            (
                "check_activation_triggers(json) payload gate_open/exit mismatch: "
                f"gate_open={gate_open!r} payload_exit={payload_exit_code}."
            ),
        )

    if expected_exit != result.exit_code:
        return (
            None,
            (
                "check_activation_triggers(json) gate_open/exit mismatch: "
                f"gate_open={gate_open!r} exit={result.exit_code}."
            ),
        )

    return payload, None


def check_markdown_gate_consistency(
    markdown_result: CommandResult,
    *,
    activation_payload: dict[str, Any],
) -> str | None:
    if markdown_result.exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN):
        return (
            "check_activation_triggers(markdown) returned unexpected exit code "
            f"{markdown_result.exit_code}."
        )

    gate_open = bool(activation_payload["gate_open"])
    activation_required = bool(activation_payload["activation_required"])
    queue_state = str(activation_payload["queue_state"])
    expected_exit = int(activation_payload["exit_code"])

    t4_overlay = activation_payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    t4_new_scope_publish = bool(t4_overlay["new_scope_publish"])
    t4_source = str(t4_overlay["source"])

    open_blockers = activation_payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    open_blockers_count = int(open_blockers["count"])
    open_blockers_trigger_fired = bool(open_blockers["trigger_fired"])

    inputs = activation_payload["inputs"]
    assert isinstance(inputs, dict)
    open_blockers_input = inputs.get("open_blockers_json")
    open_blockers_input_text = (
        f"`{open_blockers_input}`" if open_blockers_input is not None else "_none_"
    )
    t4_overlay_input = inputs.get("t4_governance_overlay_json")
    t4_overlay_input_text = (
        f"`{t4_overlay_input}`" if t4_overlay_input is not None else "_none_"
    )

    freshness = activation_payload["freshness"]
    assert isinstance(freshness, dict)
    issues_freshness = freshness.get("issues")
    milestones_freshness = freshness.get("milestones")
    assert isinstance(issues_freshness, dict)
    assert isinstance(milestones_freshness, dict)

    if markdown_result.exit_code != expected_exit:
        return (
            "check_activation_triggers(markdown) gate_open/exit mismatch: "
            f"gate_open={gate_open!r} exit={markdown_result.exit_code}."
        )

    expected_lines = (
        f"- Mode: `{activation_payload['mode']}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        f"- Open blockers JSON: {open_blockers_input_text}",
        f"- T4 governance overlay JSON: {t4_overlay_input_text}",
        f"- Activation required: `{bool_text(activation_required)}`",
        f"- T4 new scope publish: `{bool_text(t4_new_scope_publish)}`",
        f"- T4 source: `{t4_source}`",
        f"- Gate open: `{bool_text(gate_open)}`",
        f"- Queue state: `{queue_state}`",
        f"- Exit code: `{expected_exit}`",
        f"- Open blockers count: `{open_blockers_count}`",
        f"- Open blockers trigger fired: `{bool_text(open_blockers_trigger_fired)}`",
        "## Snapshot Freshness",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "## Trigger Results",
        "| Trigger ID | Fired | Count | Condition |",
    )
    for expected_line in expected_lines:
        if expected_line in markdown_result.stdout:
            continue
        return (
            "check_activation_triggers(markdown) missing deterministic line "
            f"{expected_line!r}."
        )

    issues_freshness_error = validate_markdown_freshness_row(
        markdown_text=markdown_result.stdout,
        label="Issues",
        freshness_entry=issues_freshness,
    )
    if issues_freshness_error is not None:
        return issues_freshness_error

    milestones_freshness_error = validate_markdown_freshness_row(
        markdown_text=markdown_result.stdout,
        label="Milestones",
        freshness_entry=milestones_freshness,
    )
    if milestones_freshness_error is not None:
        return milestones_freshness_error

    active_trigger_ids = activation_payload["active_trigger_ids"]
    assert isinstance(active_trigger_ids, list)
    if active_trigger_ids:
        expected_active_line = "- Active triggers: " + ", ".join(
            f"`{trigger_id}`" for trigger_id in active_trigger_ids
        )
    else:
        expected_active_line = "- Active triggers: _none_"
    if expected_active_line not in markdown_result.stdout:
        return (
            "check_activation_triggers(markdown) missing deterministic gate line "
            f"{expected_active_line!r}."
        )

    return None


def render_command_log(title: str, result: CommandResult) -> str:
    lines = [
        f"# {title}",
        "",
        "## stdout",
        "",
        result.stdout.rstrip("\n") if result.stdout else "_empty_",
        "",
        "## stderr",
        "",
        result.stderr.rstrip("\n") if result.stderr else "_empty_",
        "",
    ]
    return "\n".join(lines)


def render_spec_lint_log(result: CommandResult) -> str:
    return render_command_log("spec_lint command output", result)


def summarize_command(result: CommandResult) -> dict[str, Any]:
    return {
        "argv": [
            "python",
            display_path(result.spec.script_path),
            *list(result.spec.display_args),
        ],
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }


def determine_final_exit(
    *,
    activation_payload: dict[str, Any] | None,
    spec_lint_exit_code: int,
    errors: Sequence[str],
) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"

    assert activation_payload is not None
    gate_open = bool(activation_payload["gate_open"])
    if gate_open:
        return EXIT_GATE_OPEN, "activation-open"

    if spec_lint_exit_code == 0:
        return EXIT_GATE_CLOSED, "ok"

    return EXIT_RUNNER_ERROR, "spec-lint-failed"


def build_summary_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    output_dir: Path,
    spec_globs: Sequence[str],
    snapshot_refresh_requested: bool,
    snapshot_refresh_result: CommandResult | None,
    open_blockers_refresh_requested: bool,
    open_blockers_refresh_result: CommandResult | None,
    open_blockers_refresh_root: Path | None,
    open_blockers_refresh_generated_at_utc: str | None,
    open_blockers_refresh_source: str | None,
    issues_max_age_seconds: int | None,
    milestones_max_age_seconds: int | None,
    snapshot_generated_at_utc: str | None,
    activation_payload: dict[str, Any] | None,
    activation_json_result: CommandResult,
    activation_markdown_result: CommandResult,
    spec_lint_result: CommandResult,
    errors: Sequence[str],
    final_exit_code: int,
    final_status: str,
) -> dict[str, Any]:
    gate_open = None
    queue_state = None
    activation_required = None
    active_trigger_ids: list[str] = []
    activation_exit_code = activation_json_result.exit_code
    open_blocker_count = None
    open_blockers_trigger_fired = None

    if activation_payload is not None:
        gate_open = bool(activation_payload["gate_open"])
        queue_state = str(activation_payload["queue_state"])
        activation_required = bool(activation_payload["activation_required"])
        active_trigger_ids = [str(item) for item in activation_payload["active_trigger_ids"]]
        activation_exit_code = int(activation_payload.get("exit_code", activation_json_result.exit_code))
        open_blockers = activation_payload.get("open_blockers")
        if isinstance(open_blockers, dict):
            raw_open_blocker_count = open_blockers.get("count")
            raw_open_blockers_trigger_fired = open_blockers.get("trigger_fired")
            if isinstance(raw_open_blocker_count, int) and not isinstance(raw_open_blocker_count, bool):
                open_blocker_count = raw_open_blocker_count
            if isinstance(raw_open_blockers_trigger_fired, bool):
                open_blockers_trigger_fired = raw_open_blockers_trigger_fired

    commands: dict[str, Any] = {}
    if snapshot_refresh_result is not None:
        commands["capture_activation_snapshots"] = summarize_command(snapshot_refresh_result)
    if open_blockers_refresh_result is not None:
        commands["extract_open_blockers_snapshot_json"] = summarize_command(
            open_blockers_refresh_result
        )
    commands["check_activation_triggers_json"] = summarize_command(activation_json_result)
    commands["check_activation_triggers_markdown"] = summarize_command(activation_markdown_result)
    commands["spec_lint"] = summarize_command(spec_lint_result)

    return {
        "runner": "activation-preflight/v0.13",
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
            "open_blockers_refresh": open_blockers_refresh_requested,
            "open_blockers_root": (
                display_path(open_blockers_refresh_root)
                if open_blockers_refresh_root is not None
                else None
            ),
            "open_blockers_generated_at_utc": open_blockers_refresh_generated_at_utc,
            "open_blockers_source": open_blockers_refresh_source,
            "spec_globs": list(spec_globs),
            "snapshot_refresh": snapshot_refresh_requested,
            "issues_max_age_seconds": issues_max_age_seconds,
            "milestones_max_age_seconds": milestones_max_age_seconds,
            "snapshot_generated_at_utc": snapshot_generated_at_utc,
        },
        "artifacts": {
            "output_dir": display_path(output_dir),
            "check_activation_json": ACTIVATION_JSON_FILENAME,
            "check_activation_markdown": ACTIVATION_MD_FILENAME,
            "spec_lint_log": SPEC_LINT_LOG_FILENAME,
            "snapshot_capture_log": (
                SNAPSHOT_CAPTURE_LOG_FILENAME if snapshot_refresh_result is not None else None
            ),
            "open_blockers_refresh_log": (
                OPEN_BLOCKERS_REFRESH_LOG_FILENAME
                if open_blockers_refresh_result is not None
                else None
            ),
            "summary_json": SUMMARY_JSON_FILENAME,
            "report_markdown": REPORT_MD_FILENAME,
        },
        "snapshot": {
            "refresh_requested": snapshot_refresh_requested,
            "refresh_attempted": snapshot_refresh_result is not None,
            "refresh_exit_code": (
                snapshot_refresh_result.exit_code if snapshot_refresh_result is not None else None
            ),
            "open_blockers_refresh_requested": open_blockers_refresh_requested,
            "open_blockers_refresh_attempted": open_blockers_refresh_result is not None,
            "open_blockers_refresh_exit_code": (
                open_blockers_refresh_result.exit_code
                if open_blockers_refresh_result is not None
                else None
            ),
            "open_blockers_refresh_root": (
                display_path(open_blockers_refresh_root)
                if open_blockers_refresh_root is not None
                else None
            ),
            "open_blockers_generated_at_utc": open_blockers_refresh_generated_at_utc,
            "open_blockers_source": open_blockers_refresh_source,
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
            "issues_max_age_seconds": issues_max_age_seconds,
            "milestones_max_age_seconds": milestones_max_age_seconds,
            "snapshot_generated_at_utc": snapshot_generated_at_utc,
        },
        "activation": {
            "gate_open": gate_open,
            "activation_required": activation_required,
            "queue_state": queue_state,
            "active_trigger_ids": active_trigger_ids,
            "open_blocker_count": open_blocker_count,
            "open_blockers_trigger_fired": open_blockers_trigger_fired,
            "exit_code": activation_exit_code,
        },
        "spec_lint": {
            "exit_code": spec_lint_result.exit_code,
            "ok": spec_lint_result.exit_code == 0,
        },
        "commands": commands,
        "errors": list(errors),
        "final_status": final_status,
        "final_exit_code": final_exit_code,
    }


def render_markdown_report(summary: dict[str, Any]) -> str:
    inputs = summary["inputs"]
    artifacts = summary["artifacts"]
    snapshot = summary["snapshot"]
    activation = summary["activation"]
    spec_lint = summary["spec_lint"]
    commands = summary["commands"]
    errors = summary["errors"]

    assert isinstance(inputs, dict)
    assert isinstance(artifacts, dict)
    assert isinstance(snapshot, dict)
    assert isinstance(activation, dict)
    assert isinstance(spec_lint, dict)
    assert isinstance(commands, dict)
    assert isinstance(errors, list)

    gate_open = activation["gate_open"]
    gate_open_text = "`unknown`"
    if isinstance(gate_open, bool):
        gate_open_text = f"`{bool_text(gate_open)}`"

    def optional_literal(value: Any) -> str:
        if value is None:
            return "_none_"
        return f"`{value}`"

    def optional_bool_literal(value: Any) -> str:
        if value is None:
            return "_none_"
        if isinstance(value, bool):
            return f"`{bool_text(value)}`"
        return f"`{value}`"

    lines = [
        "# Activation Preflight Orchestration",
        "",
        "## Inputs",
        "",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        "- Open blockers JSON: " + optional_literal(inputs["open_blockers_json"]),
        "- Open blockers refresh requested: "
        + optional_bool_literal(inputs["open_blockers_refresh"]),
        "- Open blockers scan root: " + optional_literal(inputs["open_blockers_root"]),
        "- Open blockers generated_at_utc: "
        + optional_literal(inputs["open_blockers_generated_at_utc"]),
        "- Open blockers source: " + optional_literal(inputs["open_blockers_source"]),
        (
            "- Spec lint globs: "
            + (
                ", ".join(f"`{glob}`" for glob in inputs["spec_globs"])
                if inputs["spec_globs"]
                else "_default spec_lint globs_"
            )
        ),
        "",
        "## Snapshot Refresh + Freshness",
        "",
        f"- Snapshot refresh requested: `{bool_text(bool(snapshot['refresh_requested']))}`",
        f"- Snapshot refresh attempted: `{bool_text(bool(snapshot['refresh_attempted']))}`",
        f"- Snapshot refresh exit code: {optional_literal(snapshot['refresh_exit_code'])}",
        (
            "- Open blockers refresh requested: "
            f"`{bool_text(bool(snapshot['open_blockers_refresh_requested']))}`"
        ),
        (
            "- Open blockers refresh attempted: "
            f"`{bool_text(bool(snapshot['open_blockers_refresh_attempted']))}`"
        ),
        (
            "- Open blockers refresh exit code: "
            + optional_literal(snapshot["open_blockers_refresh_exit_code"])
        ),
        "- Open blockers refresh root: " + optional_literal(snapshot["open_blockers_refresh_root"]),
        (
            "- Open blockers refresh generated_at_utc: "
            + optional_literal(snapshot["open_blockers_generated_at_utc"])
        ),
        (
            "- Open blockers refresh source: "
            + optional_literal(snapshot["open_blockers_source"])
        ),
        f"- Issues snapshot path: `{snapshot['issues_json']}`",
        f"- Milestones snapshot path: `{snapshot['milestones_json']}`",
        f"- Open blockers snapshot path: {optional_literal(snapshot['open_blockers_json'])}",
        f"- Issues max age seconds: {optional_literal(snapshot['issues_max_age_seconds'])}",
        f"- Milestones max age seconds: {optional_literal(snapshot['milestones_max_age_seconds'])}",
        f"- Snapshot generated_at_utc override: {optional_literal(snapshot['snapshot_generated_at_utc'])}",
        "",
        "## Activation State",
        "",
        f"- Gate open: {gate_open_text}",
        f"- Activation required: `{activation['activation_required']}`",
        f"- Queue state: `{activation['queue_state']}`",
        (
            "- Active trigger IDs: "
            + (
                ", ".join(f"`{item}`" for item in activation["active_trigger_ids"])
                if activation["active_trigger_ids"]
                else "_none_"
            )
        ),
        f"- Open blocker count: {optional_literal(activation['open_blocker_count'])}",
        (
            "- Open blockers trigger fired: "
            + optional_bool_literal(activation["open_blockers_trigger_fired"])
        ),
        f"- Activation checker exit code: `{activation['exit_code']}`",
        "",
        "## Spec Lint",
        "",
        f"- spec_lint exit code: `{spec_lint['exit_code']}`",
        f"- spec_lint ok: `{bool_text(bool(spec_lint['ok']))}`",
        "",
        "## Final Outcome",
        "",
        f"- Final status: `{summary['final_status']}`",
        f"- Final exit code: `{summary['final_exit_code']}`",
        "",
        "## Artifacts",
        "",
        f"- Output directory: `{artifacts['output_dir']}`",
        f"- Activation JSON: `{artifacts['check_activation_json']}`",
        f"- Activation markdown: `{artifacts['check_activation_markdown']}`",
        f"- spec_lint log: `{artifacts['spec_lint_log']}`",
        "- Snapshot capture log: "
        + (
            f"`{artifacts['snapshot_capture_log']}`"
            if artifacts["snapshot_capture_log"] is not None
            else "_none_"
        ),
        "- Open blockers refresh log: "
        + (
            f"`{artifacts['open_blockers_refresh_log']}`"
            if artifacts["open_blockers_refresh_log"] is not None
            else "_none_"
        ),
        f"- Summary JSON: `{artifacts['summary_json']}`",
        f"- Report markdown: `{artifacts['report_markdown']}`",
        "",
        "## Command Exit Codes",
        "",
        "| Command | Exit Code |",
        "| --- | --- |",
    ]

    for key, payload in commands.items():
        assert isinstance(payload, dict)
        lines.append(f"| `{key}` | `{payload['exit_code']}` |")

    lines.extend(["", "## Errors", ""])
    if errors:
        for entry in errors:
            lines.append(f"- {entry}")
    else:
        lines.append("- _none_")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_activation_preflight.py",
        description=(
            "Run deterministic activation preflight orchestration "
            "(check_activation_triggers json+markdown + spec_lint) and persist evidence artifacts."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: gate closed and spec_lint succeeded.
              1: gate open (activation-open signal is preserved deterministically).
              2: orchestration error, malformed activation output, or spec_lint failure while gate is closed.

            Output artifact files (under --output-dir):
              - {ACTIVATION_JSON_FILENAME}
              - {ACTIVATION_MD_FILENAME}
              - {SPEC_LINT_LOG_FILENAME}
              - {SNAPSHOT_CAPTURE_LOG_FILENAME} (when --refresh-snapshots is used)
              - {OPEN_BLOCKERS_REFRESH_LOG_FILENAME} (when --refresh-open-blockers is used)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to offline issues snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to offline milestones snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--refresh-snapshots",
        action="store_true",
        help=(
            "Refresh issues/milestones snapshots before running activation checks by invoking "
            "scripts/capture_activation_snapshots.py."
        ),
    )
    parser.add_argument(
        "--snapshot-generated-at-utc",
        help=(
            "Optional generated_at_utc timestamp forwarded to capture_activation_snapshots "
            "when --refresh-snapshots is set."
        ),
    )
    parser.add_argument(
        "--issues-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for issues.",
    )
    parser.add_argument(
        "--milestones-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for milestones.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON passed to check_activation_triggers. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional open blockers JSON forwarded to check_activation_triggers "
            "for deterministic blocker-trigger gating. When --refresh-open-blockers "
            "is set and this path is omitted, the refreshed snapshot is written to "
            f"{OPEN_BLOCKERS_REFRESH_RELATIVE_PATH.as_posix()} under --output-dir."
        ),
    )
    parser.add_argument(
        "--refresh-open-blockers",
        action="store_true",
        help=(
            "Refresh open-blockers snapshot before activation checks by invoking "
            "scripts/extract_open_blockers.py with --format snapshot-json."
        ),
    )
    parser.add_argument(
        "--open-blockers-root",
        type=Path,
        default=DEFAULT_OPEN_BLOCKERS_ROOT,
        help=(
            "Root directory scanned for blocker rows when --refresh-open-blockers is set. "
            f"Default: {display_path(DEFAULT_OPEN_BLOCKERS_ROOT)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-generated-at-utc",
        help=(
            "generated_at_utc metadata forwarded to extract_open_blockers "
            "snapshot-json mode (required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--open-blockers-source",
        help=(
            "source metadata forwarded to extract_open_blockers snapshot-json mode "
            "(required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--actionable-status",
        action="append",
        dest="actionable_statuses",
        help=(
            "Repeatable actionable status forwarded to check_activation_triggers. "
            "Defaults are inherited when omitted."
        ),
    )
    t4_group = parser.add_mutually_exclusive_group()
    t4_group.add_argument(
        "--t4-governance-overlay-json",
        type=Path,
        help="Optional governance overlay JSON forwarded to check_activation_triggers.",
    )
    t4_group.add_argument(
        "--t4-new-scope-publish",
        action="store_true",
        help="Optional T4 override flag forwarded to check_activation_triggers.",
    )
    parser.add_argument(
        "--spec-glob",
        action="append",
        default=[],
        dest="spec_globs",
        help=(
            "Repeatable glob passed to spec_lint via --glob. "
            "If omitted, spec_lint defaults are used."
        ),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=(
            "Directory where deterministic artifacts are written. "
            f"Default: {display_path(DEFAULT_OUTPUT_DIR)}."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    output_dir = resolve_repo_path(args.output_dir)
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )
    try:
        expected_actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
    except ValueError as exc:
        expected_actionable_statuses = tuple(
            status for status in (args.actionable_statuses or []) if status
        )
        if not expected_actionable_statuses:
            expected_actionable_statuses = DEFAULT_ACTIONABLE_STATUSES
        errors = [f"invalid actionable-status input: {exc}."]
    else:
        errors: list[str] = []

    snapshot_refresh_result: CommandResult | None = None
    open_blockers_refresh_result: CommandResult | None = None
    open_blockers_refresh_root = (
        resolve_repo_path(args.open_blockers_root) if args.refresh_open_blockers else None
    )

    if args.refresh_snapshots:
        capture_actual_args: list[str] = [
            "--issues-output",
            str(issues_path),
            "--milestones-output",
            str(milestones_path),
        ]
        capture_display_args: list[str] = [
            "--issues-output",
            display_path(issues_path),
            "--milestones-output",
            display_path(milestones_path),
        ]
        if args.snapshot_generated_at_utc is not None:
            capture_actual_args.extend(["--generated-at-utc", args.snapshot_generated_at_utc])
            capture_display_args.extend(["--generated-at-utc", args.snapshot_generated_at_utc])

        capture_spec = CommandSpec(
            name="capture_activation_snapshots",
            script_path=CAPTURE_SNAPSHOTS_SCRIPT_PATH,
            actual_args=tuple(capture_actual_args),
            display_args=tuple(capture_display_args),
        )
        snapshot_refresh_result = run_command(capture_spec)
        if snapshot_refresh_result.exit_code != 0:
            errors.append(
                "capture_activation_snapshots returned unexpected exit code "
                f"{snapshot_refresh_result.exit_code}."
            )

    if args.refresh_open_blockers:
        if open_blockers_path is None:
            open_blockers_path = default_open_blockers_output_path(output_dir)

        assert open_blockers_refresh_root is not None
        extract_actual_args: list[str] = [
            "--root",
            str(open_blockers_refresh_root),
            "--format",
            "snapshot-json",
        ]
        extract_display_args: list[str] = [
            "--root",
            display_path(open_blockers_refresh_root),
            "--format",
            "snapshot-json",
        ]
        if args.open_blockers_generated_at_utc is not None:
            extract_actual_args.extend(
                ["--generated-at-utc", args.open_blockers_generated_at_utc]
            )
            extract_display_args.extend(
                ["--generated-at-utc", args.open_blockers_generated_at_utc]
            )
        if args.open_blockers_source is not None:
            extract_actual_args.extend(["--source", args.open_blockers_source])
            extract_display_args.extend(["--source", args.open_blockers_source])

        extract_spec = CommandSpec(
            name="extract_open_blockers_snapshot_json",
            script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
            actual_args=tuple(extract_actual_args),
            display_args=tuple(extract_display_args),
        )
        open_blockers_refresh_result = run_command(extract_spec)
        if open_blockers_refresh_result.exit_code != 0:
            errors.append(
                "extract_open_blockers(snapshot-json) returned unexpected exit code "
                f"{open_blockers_refresh_result.exit_code}."
            )
        else:
            try:
                write_text(open_blockers_path, open_blockers_refresh_result.stdout)
            except OSError as exc:
                errors.append(
                    "unable to persist refreshed open blockers snapshot to "
                    f"{display_path(open_blockers_path)}: {exc}."
                )

    activation_actual_args = [
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(catalog_path),
    ]
    activation_display_args = [
        "--issues-json",
        display_path(issues_path),
        "--milestones-json",
        display_path(milestones_path),
        "--catalog-json",
        display_path(catalog_path),
    ]
    if open_blockers_path is not None:
        activation_actual_args.extend(["--open-blockers-json", str(open_blockers_path)])
        activation_display_args.extend(["--open-blockers-json", display_path(open_blockers_path)])

    if args.actionable_statuses:
        for status in args.actionable_statuses:
            activation_actual_args.extend(["--actionable-status", status])
            activation_display_args.extend(["--actionable-status", status])

    if args.issues_max_age_seconds is not None:
        activation_actual_args.extend(["--issues-max-age-seconds", str(args.issues_max_age_seconds)])
        activation_display_args.extend(["--issues-max-age-seconds", str(args.issues_max_age_seconds)])
    if args.milestones_max_age_seconds is not None:
        activation_actual_args.extend(
            ["--milestones-max-age-seconds", str(args.milestones_max_age_seconds)]
        )
        activation_display_args.extend(
            ["--milestones-max-age-seconds", str(args.milestones_max_age_seconds)]
        )

    if t4_overlay_path is not None:
        activation_actual_args.extend(["--t4-governance-overlay-json", str(t4_overlay_path)])
        activation_display_args.extend(
            ["--t4-governance-overlay-json", display_path(t4_overlay_path)]
        )
    elif args.t4_new_scope_publish:
        activation_actual_args.append("--t4-new-scope-publish")
        activation_display_args.append("--t4-new-scope-publish")

    activation_json_spec = CommandSpec(
        name="check_activation_triggers_json",
        script_path=ACTIVATION_CHECK_SCRIPT_PATH,
        actual_args=tuple([*activation_actual_args, "--format", "json"]),
        display_args=tuple([*activation_display_args, "--format", "json"]),
    )
    activation_markdown_spec = CommandSpec(
        name="check_activation_triggers_markdown",
        script_path=ACTIVATION_CHECK_SCRIPT_PATH,
        actual_args=tuple([*activation_actual_args, "--format", "markdown"]),
        display_args=tuple([*activation_display_args, "--format", "markdown"]),
    )

    spec_lint_actual_args: list[str] = []
    spec_lint_display_args: list[str] = []
    for glob in args.spec_globs:
        spec_lint_actual_args.extend(["--glob", glob])
        spec_lint_display_args.extend(["--glob", glob])

    spec_lint_spec = CommandSpec(
        name="spec_lint",
        script_path=SPEC_LINT_SCRIPT_PATH,
        actual_args=tuple(spec_lint_actual_args),
        display_args=tuple(spec_lint_display_args),
    )

    activation_json_result = run_command(activation_json_spec)
    activation_markdown_result = run_command(activation_markdown_spec)
    spec_lint_result = run_command(spec_lint_spec)

    activation_payload, activation_error = parse_activation_payload(
        activation_json_result,
        expected_issues_path=issues_path,
        expected_milestones_path=milestones_path,
        expected_catalog_path=catalog_path,
        expected_open_blockers_path=open_blockers_path,
        expected_t4_overlay_path=t4_overlay_path,
        expected_actionable_statuses=expected_actionable_statuses,
        expected_issues_max_age_seconds=args.issues_max_age_seconds,
        expected_milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    if activation_error is not None:
        errors.append(activation_error)

    if activation_payload is not None:
        markdown_error = check_markdown_gate_consistency(
            activation_markdown_result,
            activation_payload=activation_payload,
        )
        if markdown_error is not None:
            errors.append(markdown_error)

    final_exit_code, final_status = determine_final_exit(
        activation_payload=activation_payload,
        spec_lint_exit_code=spec_lint_result.exit_code,
        errors=errors,
    )

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        activation_json_path = output_dir / ACTIVATION_JSON_FILENAME
        activation_md_path = output_dir / ACTIVATION_MD_FILENAME
        spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
        snapshot_capture_log_path = output_dir / SNAPSHOT_CAPTURE_LOG_FILENAME
        open_blockers_refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
        summary_json_path = output_dir / SUMMARY_JSON_FILENAME
        report_md_path = output_dir / REPORT_MD_FILENAME

        write_text(activation_json_path, activation_json_result.stdout)
        write_text(activation_md_path, activation_markdown_result.stdout)
        write_text(spec_lint_log_path, render_spec_lint_log(spec_lint_result))
        if snapshot_refresh_result is not None:
            write_text(
                snapshot_capture_log_path,
                render_command_log("capture_activation_snapshots command output", snapshot_refresh_result),
            )
        if open_blockers_refresh_result is not None:
            write_text(
                open_blockers_refresh_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    open_blockers_refresh_result,
                ),
            )

        summary = build_summary_payload(
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_path=open_blockers_path,
            output_dir=output_dir,
            spec_globs=tuple(args.spec_globs),
            snapshot_refresh_requested=bool(args.refresh_snapshots),
            snapshot_refresh_result=snapshot_refresh_result,
            open_blockers_refresh_requested=bool(args.refresh_open_blockers),
            open_blockers_refresh_result=open_blockers_refresh_result,
            open_blockers_refresh_root=open_blockers_refresh_root,
            open_blockers_refresh_generated_at_utc=args.open_blockers_generated_at_utc,
            open_blockers_refresh_source=args.open_blockers_source,
            issues_max_age_seconds=args.issues_max_age_seconds,
            milestones_max_age_seconds=args.milestones_max_age_seconds,
            snapshot_generated_at_utc=args.snapshot_generated_at_utc,
            activation_payload=activation_payload,
            activation_json_result=activation_json_result,
            activation_markdown_result=activation_markdown_result,
            spec_lint_result=spec_lint_result,
            errors=tuple(errors),
            final_exit_code=final_exit_code,
            final_status=final_status,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)

        write_text(summary_json_path, summary_json)
        write_text(report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist preflight artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "activation-preflight: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"summary={display_path(output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(output_dir / REPORT_MD_FILENAME)}"
    )
    return final_exit_code


if __name__ == "__main__":
    raise SystemExit(main())
