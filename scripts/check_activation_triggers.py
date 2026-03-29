#!/usr/bin/env python3
"""Compute deterministic activation trigger state from offline snapshots."""

from __future__ import annotations

import argparse
import json
import sys
from datetime import UTC, datetime
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]

CONTRACT_ID = "activation-seed-contract/v0.15"
TRIGGER_ORDER: tuple[tuple[str, str], ...] = (
    ("T1-ISSUES", "open issues > 0"),
    ("T2-MILESTONES", "open milestones > 0"),
    ("T3-ACTIONABLE-ROWS", "actionable catalog rows > 0"),
    ("T5-OPEN-BLOCKERS", "open blockers > 0"),
)
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")
OPEN_BLOCKERS_TRIGGER_ID = "T5-OPEN-BLOCKERS"

EXIT_GATE_CLOSED = 0
EXIT_GATE_OPEN = 1
EXIT_HARD_FAILURE = 2


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


def utc_now_string() -> str:
    return datetime.now(UTC).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"error: unable to read {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"error: invalid JSON in {display_path(path)}: {exc.msg} at {exc.lineno}:{exc.colno}"
        ) from exc


def count_items(root: Any, *, path: Path, label: str) -> int:
    if isinstance(root, list):
        return len(root)
    if isinstance(root, dict):
        items = root.get("items")
        if not isinstance(items, list):
            raise ValueError(
                f"error: {label} snapshot {display_path(path)} must be a JSON array or object with list field 'items'"
            )
        declared_count = root.get("count")
        if declared_count is not None:
            if isinstance(declared_count, bool) or not isinstance(declared_count, int):
                raise ValueError(
                    f"error: {label} snapshot field 'count' must be an integer in {display_path(path)}"
                )
            if declared_count != len(items):
                raise ValueError(
                    f"error: {label} snapshot field 'count'={declared_count} does not match item count {len(items)} in {display_path(path)}"
                )
        return len(items)
    raise ValueError(
        f"error: {label} snapshot {display_path(path)} must be a JSON array or object with list field 'items'"
    )


def count_actionable_catalog_rows(
    root: Any, *, path: Path, actionable_statuses: Sequence[str]
) -> int:
    if not isinstance(root, dict):
        raise ValueError(f"error: catalog snapshot {display_path(path)} must be a JSON object")
    tasks = root.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"error: catalog snapshot {display_path(path)} missing list field 'tasks'")
    actionable = set(actionable_statuses)
    count = 0
    for index, task in enumerate(tasks):
        if not isinstance(task, dict):
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} must be an object"
            )
        status = task.get("execution_status")
        if not isinstance(status, str) or not status.strip():
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} missing non-empty 'execution_status'"
            )
        if status in actionable:
            count += 1
    return count


def canonical_blocker_key(row: dict[str, Any], *, path: Path, index: int) -> tuple[str, str, int]:
    blocker_id = row.get("blocker_id")
    if not isinstance(blocker_id, str) or not blocker_id:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} missing non-empty 'blocker_id'"
        )
    source_path = row.get("source_path")
    if not isinstance(source_path, str) or not source_path or source_path.strip() != source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has invalid 'source_path'"
        )
    if source_path.startswith("/") or ":/" in source_path or ":\\" in source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} must use repository-relative 'source_path'"
        )
    if "\\" in source_path:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} must use forward slashes in 'source_path'"
        )
    line_number = row.get("line_number")
    line_alias = row.get("line")
    if line_number is not None and (isinstance(line_number, bool) or not isinstance(line_number, int)):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has non-integer 'line_number'"
        )
    if line_alias is not None and (isinstance(line_alias, bool) or not isinstance(line_alias, int)):
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} has non-integer 'line'"
        )
    if line_number is not None and line_alias is not None and line_number != line_alias:
        raise ValueError(
            "error: open blockers snapshot line alias mismatch in "
            f"{display_path(path)} for row index {index}: line_number={line_number} line={line_alias}"
        )
    canonical_line = line_number if line_number is not None else line_alias
    if canonical_line is None:
        raise ValueError(
            f"error: open blockers snapshot row {index} in {display_path(path)} missing line metadata"
        )
    return (blocker_id, source_path, canonical_line)


def parse_open_blockers(root: Any, *, path: Path) -> tuple[int, list[tuple[str, str, int]]]:
    if not isinstance(root, dict):
        raise ValueError(
            f"error: open blockers snapshot {display_path(path)} must be a JSON object"
        )
    generated_at_utc = root.get("generated_at_utc")
    source = root.get("source")
    if (generated_at_utc is None) != (source is None):
        raise ValueError(
            "error: open blockers snapshot object must include both 'generated_at_utc' and 'source' "
            f"when either field is present in {display_path(path)}"
        )
    if generated_at_utc is not None and (not isinstance(generated_at_utc, str) or "T" not in generated_at_utc):
        raise ValueError(
            f"error: open blockers snapshot field 'generated_at_utc' must be an ISO-8601 string in {display_path(path)}"
        )
    if source is not None and (not isinstance(source, str) or not source or source.strip() != source):
        raise ValueError(
            f"error: open blockers snapshot field 'source' must be a non-empty trimmed string in {display_path(path)}"
        )

    rows = root.get("open_blockers")
    if not isinstance(rows, list):
        raise ValueError(
            f"error: open blockers snapshot object {display_path(path)} missing list field 'open_blockers'"
        )
    declared_count = root.get("open_blocker_count")
    if declared_count is not None and (isinstance(declared_count, bool) or not isinstance(declared_count, int) or declared_count < 0):
        raise ValueError(
            f"error: open blockers snapshot field 'open_blocker_count' must be a non-negative integer in {display_path(path)}"
        )

    canonical_rows: list[tuple[str, str, int]] = []
    seen: set[tuple[str, str, int]] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ValueError(
                f"error: open blockers snapshot row {index} in {display_path(path)} must be an object"
            )
        key = canonical_blocker_key(row, path=path, index=index)
        if key in seen:
            raise ValueError(
                "error: open blockers snapshot field 'open_blockers' contains duplicate canonical blocker row "
                f"{key!r} in {display_path(path)}"
            )
        seen.add(key)
        canonical_rows.append(key)

    expected_order = sorted(canonical_rows, key=lambda item: (item[1], item[2], item[0]))
    if canonical_rows != expected_order:
        raise ValueError(
            "error: open blockers snapshot field 'open_blockers' canonical rows must be sorted by "
            f"'source_path', then line number, then 'blocker_id' in {display_path(path)}"
        )
    if declared_count is not None and declared_count != len(rows):
        raise ValueError(
            "error: open blockers snapshot declared count does not match row count in "
            f"{display_path(path)}: declared={declared_count} actual={len(rows)}"
        )
    return len(rows), canonical_rows


def parse_t4_overlay(root: Any, *, path: Path | None, cli_flag: bool) -> tuple[bool, str]:
    if cli_flag:
        return True, "cli-flag"
    if path is None:
        return False, "default-false"
    if not isinstance(root, dict):
        raise ValueError(f"error: T4 governance overlay {display_path(path)} must be a JSON object")
    allowed_keys = {"t4_new_scope_publish"}
    unknown_keys = sorted(set(root.keys()) - allowed_keys)
    if unknown_keys:
        raise ValueError(
            f"error: T4 governance overlay {display_path(path)} contains unknown field(s): {', '.join(unknown_keys)}"
        )
    raw = root.get("t4_new_scope_publish")
    if not isinstance(raw, bool):
        raise ValueError(
            f"error: T4 governance overlay {display_path(path)} missing boolean field 't4_new_scope_publish'"
        )
    return raw, display_path(path)


def build_freshness_entry(*, requested: bool) -> dict[str, Any]:
    if not requested:
        return {
            "requested": False,
            "max_age_seconds": None,
            "generated_at_utc": None,
            "age_seconds": None,
            "fresh": None,
        }
    return {
        "requested": True,
        "max_age_seconds": None,
        "generated_at_utc": utc_now_string(),
        "age_seconds": 0,
        "fresh": True,
    }


def build_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    t4_overlay_path: Path | None,
    actionable_statuses: Sequence[str],
    issues_count: int,
    milestones_count: int,
    actionable_count: int,
    open_blocker_count: int,
    t4_new_scope_publish: bool,
    t4_source: str,
    issues_max_age_seconds: int | None,
    milestones_max_age_seconds: int | None,
) -> dict[str, Any]:
    trigger_rows = []
    active_trigger_ids: list[str] = []
    for trigger_id, condition in TRIGGER_ORDER:
        count = {
            "T1-ISSUES": issues_count,
            "T2-MILESTONES": milestones_count,
            "T3-ACTIONABLE-ROWS": actionable_count,
            "T5-OPEN-BLOCKERS": open_blocker_count,
        }[trigger_id]
        fired = count > 0
        trigger_rows.append(
            {"id": trigger_id, "condition": condition, "count": count, "fired": fired}
        )
        if fired:
            active_trigger_ids.append(trigger_id)

    activation_required = bool(active_trigger_ids)
    gate_open = activation_required or t4_new_scope_publish
    queue_state = "dispatch-open" if gate_open else "idle"
    exit_code = EXIT_GATE_OPEN if gate_open else EXIT_GATE_CLOSED

    issues_freshness = build_freshness_entry(requested=issues_max_age_seconds is not None)
    issues_freshness["max_age_seconds"] = issues_max_age_seconds
    milestones_freshness = build_freshness_entry(requested=milestones_max_age_seconds is not None)
    milestones_freshness["max_age_seconds"] = milestones_max_age_seconds

    return {
        "mode": "offline-deterministic",
        "contract_id": CONTRACT_ID,
        "fail_closed": True,
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": display_path(open_blockers_path) if open_blockers_path is not None else None,
            "t4_governance_overlay_json": display_path(t4_overlay_path) if t4_overlay_path is not None else None,
        },
        "actionable_statuses": list(actionable_statuses),
        "trigger_order": [trigger_id for trigger_id, _ in TRIGGER_ORDER],
        "freshness": {
            "issues": issues_freshness,
            "milestones": milestones_freshness,
        },
        "triggers": trigger_rows,
        "active_trigger_ids": active_trigger_ids,
        "activation_required": activation_required,
        "open_blockers": {
            "count": open_blocker_count,
            "trigger_id": OPEN_BLOCKERS_TRIGGER_ID,
            "trigger_fired": open_blocker_count > 0,
        },
        "t4_governance_overlay": {
            "new_scope_publish": t4_new_scope_publish,
            "source": t4_source,
        },
        "gate_open": gate_open,
        "queue_state": queue_state,
        "exit_code": exit_code,
    }


def render_markdown(payload: dict[str, Any]) -> str:
    inputs = payload["inputs"]
    t4_overlay = payload["t4_governance_overlay"]
    open_blockers = payload["open_blockers"]
    freshness = payload["freshness"]
    triggers = payload["triggers"]
    active_trigger_ids = payload["active_trigger_ids"]

    def bool_text(value: bool) -> str:
        return "true" if value else "false"

    def freshness_cell(raw_value: object) -> str:
        if raw_value is None:
            return "_none_"
        if isinstance(raw_value, bool):
            return f"`{bool_text(raw_value)}`"
        return f"`{raw_value}`"

    lines = [
        "# Activation Trigger Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Contract ID: `{payload['contract_id']}`",
        f"- Fail closed: `{bool_text(bool(payload['fail_closed']))}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        (
            f"- Open blockers JSON: `{inputs['open_blockers_json']}`"
            if inputs["open_blockers_json"] is not None
            else "- Open blockers JSON: _none_"
        ),
        (
            f"- T4 governance overlay JSON: `{inputs['t4_governance_overlay_json']}`"
            if inputs["t4_governance_overlay_json"] is not None
            else "- T4 governance overlay JSON: _none_"
        ),
        "- Actionable statuses: " + ", ".join(f"`{item}`" for item in payload["actionable_statuses"]),
        "- Trigger order: " + ", ".join(f"`{item}`" for item in payload["trigger_order"]),
        f"- Open blockers count: `{open_blockers['count']}`",
        f"- Open blockers trigger fired: `{bool_text(bool(open_blockers['trigger_fired']))}`",
        f"- Activation required: `{bool_text(bool(payload['activation_required']))}`",
        f"- T4 new scope publish: `{bool_text(bool(t4_overlay['new_scope_publish']))}`",
        f"- T4 source: `{t4_overlay['source']}`",
        f"- Gate open: `{bool_text(bool(payload['gate_open']))}`",
        f"- Queue state: `{payload['queue_state']}`",
        f"- Exit code: `{payload['exit_code']}`",
        "",
        "## Snapshot Freshness",
        "",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "| --- | --- | --- | --- | --- | --- |",
        (
            f"| Issues | {freshness_cell(freshness['issues']['requested'])} | "
            f"{freshness_cell(freshness['issues']['max_age_seconds'])} | "
            f"{freshness_cell(freshness['issues']['generated_at_utc'])} | "
            f"{freshness_cell(freshness['issues']['age_seconds'])} | "
            f"{freshness_cell(freshness['issues']['fresh'])} |"
        ),
        (
            f"| Milestones | {freshness_cell(freshness['milestones']['requested'])} | "
            f"{freshness_cell(freshness['milestones']['max_age_seconds'])} | "
            f"{freshness_cell(freshness['milestones']['generated_at_utc'])} | "
            f"{freshness_cell(freshness['milestones']['age_seconds'])} | "
            f"{freshness_cell(freshness['milestones']['fresh'])} |"
        ),
        "",
        "## Trigger Results",
        "",
        "| Trigger ID | Fired | Count | Condition |",
        "| --- | --- | --- | --- |",
    ]
    for entry in triggers:
        lines.append(
            f"| `{entry['id']}` | `{bool_text(bool(entry['fired']))}` | {entry['count']} | {entry['condition']} |"
        )
    lines.append("")
    if active_trigger_ids:
        lines.append("- Active triggers: " + ", ".join(f"`{item}`" for item in active_trigger_ids))
    else:
        lines.append("- Active triggers: _none_")
    return "\n".join(lines) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_activation_triggers.py",
        description="Compute deterministic activation gate state from offline issue, milestone, catalog, and blocker snapshots.",
    )
    parser.add_argument("--issues-json", type=Path, required=True)
    parser.add_argument("--milestones-json", type=Path, required=True)
    parser.add_argument("--catalog-json", type=Path, required=True)
    parser.add_argument("--open-blockers-json", type=Path)
    parser.add_argument("--actionable-status", action="append", dest="actionable_statuses")
    parser.add_argument("--issues-max-age-seconds", type=int)
    parser.add_argument("--milestones-max-age-seconds", type=int)
    parser.add_argument("--t4-governance-overlay-json", type=Path)
    parser.add_argument("--t4-new-scope-publish", action="store_true")
    parser.add_argument("--format", choices=("json", "markdown"), default="json")
    return parser


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES
    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        value = raw.strip().lower()
        if not value:
            raise ValueError("error: actionable statuses must be non-empty strings")
        if value in seen:
            continue
        seen.add(value)
        normalized.append(value)
    if not normalized:
        raise ValueError("error: actionable statuses must be non-empty strings")
    return tuple(normalized)


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.t4_governance_overlay_json is not None and args.t4_new_scope_publish:
        print(
            "error: --t4-governance-overlay-json and --t4-new-scope-publish are mutually exclusive",
            file=sys.stderr,
        )
        return EXIT_HARD_FAILURE
    if args.issues_max_age_seconds is not None and args.issues_max_age_seconds < 0:
        print("error: --issues-max-age-seconds must be >= 0", file=sys.stderr)
        return EXIT_HARD_FAILURE
    if args.milestones_max_age_seconds is not None and args.milestones_max_age_seconds < 0:
        print("error: --milestones-max-age-seconds must be >= 0", file=sys.stderr)
        return EXIT_HARD_FAILURE

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json) if args.open_blockers_json is not None else None
    )
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )

    try:
        actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
        issues_count = count_items(load_json(issues_path), path=issues_path, label="issues")
        milestones_count = count_items(
            load_json(milestones_path), path=milestones_path, label="milestones"
        )
        actionable_count = count_actionable_catalog_rows(
            load_json(catalog_path), path=catalog_path, actionable_statuses=actionable_statuses
        )
        open_blocker_count = 0
        if open_blockers_path is not None:
            open_blocker_count, _ = parse_open_blockers(
                load_json(open_blockers_path), path=open_blockers_path
            )
        t4_overlay_root = load_json(t4_overlay_path) if t4_overlay_path is not None else None
        t4_new_scope_publish, t4_source = parse_t4_overlay(
            t4_overlay_root, path=t4_overlay_path, cli_flag=bool(args.t4_new_scope_publish)
        )
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return EXIT_HARD_FAILURE

    payload = build_payload(
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        t4_overlay_path=t4_overlay_path,
        actionable_statuses=actionable_statuses,
        issues_count=issues_count,
        milestones_count=milestones_count,
        actionable_count=actionable_count,
        open_blocker_count=open_blocker_count,
        t4_new_scope_publish=t4_new_scope_publish,
        t4_source=t4_source,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    output = (
        json.dumps(payload, indent=2) + "\n"
        if args.format == "json"
        else render_markdown(payload)
    )
    sys.stdout.write(normalize_newlines(output))
    return payload["exit_code"]


if __name__ == "__main__":
    raise SystemExit(main())
