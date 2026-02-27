#!/usr/bin/env python3
"""Close SPT issues when their source checkbox rows are checked.

This helper reconciles `spec/planning/remaining_task_review_catalog.json` against
current markdown checkbox state in the workspace and can optionally close matching
open SPT issues on GitHub.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from copy import deepcopy
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CLOSE_CONFIG_SECTION = "close_spt_issues_from_checkboxes"
DEFAULT_CLOSE_CONFIG: dict[str, Any] = {
    "catalog_default": "spec/planning/remaining_task_review_catalog.json",
    "task_id_prefix_default": "SPT-",
    "task_id_pattern": r"^\[(SPT-\d{4})\]",
}


@dataclass(frozen=True)
class CatalogTask:
    task_id: str
    path: Path
    line: int
    title: str
    task_key: str | None = None
    source_line_hash: str | None = None


@dataclass(frozen=True)
class IssueRef:
    number: int
    title: str
    url: str


@dataclass(frozen=True)
class CloseToolingConfig:
    catalog_default: Path
    task_id_prefix_default: str
    task_id_pattern: re.Pattern[str]


class ConfigError(RuntimeError):
    pass


@dataclass(frozen=True)
class SourceLineResult:
    raw_line: str | None
    checked: bool
    display_line: str
    stale_reason: str | None


@dataclass(frozen=True)
class PlannedAction:
    task_id: str
    task_key: str | None
    issue_number: int
    issue_url: str
    source_path: str
    source_line: int
    source_checkbox_line: str


@dataclass(frozen=True)
class LoadedPlan:
    catalog_digest: str
    lane_filter: str | None
    task_id_prefix: str
    commit_sha: str | None
    actions: list[PlannedAction]


def _deep_merge_dict(base: dict[str, Any], override: dict[str, Any]) -> dict[str, Any]:
    merged: dict[str, Any] = deepcopy(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = _deep_merge_dict(merged[key], value)  # type: ignore[arg-type]
            continue
        merged[key] = value
    return merged


def _read_json_config(path: Path, section: str) -> dict[str, Any]:
    if not path.exists():
        raise ConfigError(f"config file not found: {path.as_posix()}")

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ConfigError(f"unable to read config file '{path.as_posix()}': {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ConfigError(f"invalid JSON in config file '{path.as_posix()}': {exc}") from exc

    if not isinstance(payload, dict):
        raise ConfigError(
            f"invalid config file '{path.as_posix()}': top-level JSON must be an object"
        )

    section_payload = payload.get(section)
    if not isinstance(section_payload, dict):
        raise ConfigError(
            f"invalid config file '{path.as_posix()}': missing object section '{section}'"
        )

    return section_payload


def _expect_str(payload: dict[str, Any], key: str, context: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ConfigError(f"{context} must define non-empty string key '{key}'")
    return value


def _resolve_root_path(raw_path: str) -> Path:
    path = Path(raw_path)
    if path.is_absolute():
        return path
    return ROOT / path


def load_close_tooling_config(config_path: Path | None) -> CloseToolingConfig:
    merged = deepcopy(DEFAULT_CLOSE_CONFIG)
    if config_path is not None:
        resolved_config = config_path if config_path.is_absolute() else ROOT / config_path
        override = _read_json_config(resolved_config, CLOSE_CONFIG_SECTION)
        merged = _deep_merge_dict(merged, override)

    context = f"configuration section '{CLOSE_CONFIG_SECTION}'"
    catalog_default = _resolve_root_path(_expect_str(merged, "catalog_default", context))
    task_id_prefix_default = _expect_str(merged, "task_id_prefix_default", context)
    task_id_pattern_raw = _expect_str(merged, "task_id_pattern", context)

    try:
        task_id_pattern = re.compile(task_id_pattern_raw)
    except re.error as exc:
        raise ConfigError(f"{context} key 'task_id_pattern' is not a valid regex: {exc}") from exc

    if task_id_pattern.groups < 1:
        raise ConfigError(
            f"{context} key 'task_id_pattern' must include a capture group for task ID extraction"
        )

    return CloseToolingConfig(
        catalog_default=catalog_default,
        task_id_prefix_default=task_id_prefix_default,
        task_id_pattern=task_id_pattern,
    )


def run_cmd(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str]) -> Any:
    proc = run_cmd(["gh", *args])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"gh {' '.join(args)} failed: {detail}")

    try:
        return json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"gh {' '.join(args)} returned invalid JSON: {exc}") from exc


def load_catalog(path: Path) -> list[CatalogTask]:
    if not path.exists():
        raise RuntimeError(f"catalog file not found: {path}")

    payload = json.loads(path.read_text(encoding="utf-8"))
    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise RuntimeError("catalog JSON missing 'tasks' array")

    tasks: list[CatalogTask] = []
    for raw in raw_tasks:
        if not isinstance(raw, dict):
            continue
        task_id = raw.get("task_id")
        raw_path = raw.get("path")
        raw_line = raw.get("line")
        title = raw.get("title")
        task_key = raw.get("task_key")
        source_line_hash = raw.get("source_line_hash")
        if not isinstance(task_id, str):
            continue
        if not isinstance(raw_path, str):
            continue
        if not isinstance(raw_line, int):
            continue
        if not isinstance(title, str):
            title = task_id
        if task_key is not None:
            if not isinstance(task_key, str) or not task_key:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid task_key; expected non-empty string"
                )
        if source_line_hash is not None:
            if not isinstance(source_line_hash, str) or not source_line_hash:
                raise RuntimeError(
                    f"catalog task '{task_id}' has invalid source_line_hash; expected non-empty string"
                )
        tasks.append(
            CatalogTask(
                task_id=task_id,
                path=ROOT / Path(raw_path),
                line=raw_line,
                title=title,
                task_key=task_key,
                source_line_hash=source_line_hash,
            )
        )

    return tasks


def fetch_open_spt_issues(task_id_pattern: re.Pattern[str]) -> dict[str, IssueRef]:
    payload = run_gh_json(
        ["issue", "list", "--state", "open", "--limit", "2000", "--json", "number,title,url"]
    )
    if not isinstance(payload, list):
        raise RuntimeError("unexpected gh issue list payload shape")

    mapping: dict[str, IssueRef] = {}
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        number = item.get("number")
        url = item.get("url")
        if not isinstance(title, str) or not isinstance(number, int) or not isinstance(url, str):
            continue
        match = task_id_pattern.match(title)
        if not match:
            continue
        task_id = match.group(1)
        mapping[task_id] = IssueRef(number=number, title=title, url=url)

    return mapping


def checkbox_state(path: Path, line: int) -> tuple[bool, str]:
    result = source_line_result(path, line, expected_hash=None)
    return result.checked, result.display_line


def compute_source_line_hash(raw_line: str) -> str:
    digest = hashlib.sha256(raw_line.encode("utf-8")).hexdigest()
    return f"sha256:{digest}"


def compute_file_digest(path: Path) -> str:
    digest = hashlib.sha256(path.read_bytes()).hexdigest()
    return f"sha256:{digest}"


def _display_path(path: Path) -> str:
    if path.is_absolute():
        try:
            return path.relative_to(ROOT).as_posix()
        except ValueError:
            return path.as_posix()
    return path.as_posix()


def _normalize_source_line_hash(value: str) -> str:
    candidate = value.strip()
    if candidate.lower().startswith("sha256:"):
        return candidate.split(":", maxsplit=1)[1].lower()
    return candidate.lower()


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _canonical_json(payload: dict[str, Any]) -> str:
    return json.dumps(payload, sort_keys=True, separators=(",", ":"), ensure_ascii=False)


def compute_plan_digest(payload: dict[str, Any]) -> str:
    digest = hashlib.sha256(_canonical_json(payload).encode("utf-8")).hexdigest()
    return f"sha256:{digest}"


def build_planned_actions(candidates: list[tuple[CatalogTask, IssueRef, str]]) -> list[PlannedAction]:
    return [
        PlannedAction(
            task_id=task.task_id,
            task_key=task.task_key,
            issue_number=issue.number,
            issue_url=issue.url,
            source_path=_display_path(task.path),
            source_line=task.line,
            source_checkbox_line=raw_line,
        )
        for task, issue, raw_line in candidates
    ]


def build_plan_payload(
    *,
    catalog_path: Path,
    catalog_digest: str,
    lane_filter: str | None,
    task_id_prefix: str,
    commit_sha: str | None,
    actions: list[PlannedAction],
) -> dict[str, Any]:
    payload: dict[str, Any] = {
        "schema_version": 1,
        "generated_at": _utc_timestamp(),
        "catalog_path": _display_path(catalog_path),
        "catalog_digest": catalog_digest,
        "lane_filter": lane_filter,
        "task_id_prefix": task_id_prefix,
        "commit_sha": commit_sha,
        "candidate_task_ids": [action.task_id for action in actions],
        "candidate_issue_ids": [action.issue_number for action in actions],
        "candidates": [
            {
                "task_id": action.task_id,
                "task_key": action.task_key,
                "issue_number": action.issue_number,
                "issue_url": action.issue_url,
                "source_path": action.source_path,
                "source_line": action.source_line,
                "source_checkbox_line": action.source_checkbox_line,
            }
            for action in actions
        ],
    }
    payload["plan_digest"] = compute_plan_digest(payload)
    return payload


def write_plan(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def _expect_plan_str(payload: dict[str, Any], key: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"plan JSON missing non-empty string key '{key}'")
    return value


def _expect_plan_optional_str(payload: dict[str, Any], key: str) -> str | None:
    value = payload.get(key)
    if value is None:
        return None
    if not isinstance(value, str):
        raise RuntimeError(f"plan JSON key '{key}' must be a string or null")
    if value == "":
        return None
    return value


def _expect_plan_int(payload: dict[str, Any], key: str) -> int:
    value = payload.get(key)
    if not isinstance(value, int):
        raise RuntimeError(f"plan JSON missing integer key '{key}'")
    return value


def load_plan(path: Path) -> LoadedPlan:
    if not path.exists():
        raise RuntimeError(f"plan file not found: {path.as_posix()}")

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise RuntimeError(f"unable to read plan file '{path.as_posix()}': {exc}") from exc
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid JSON in plan file '{path.as_posix()}': {exc}") from exc

    if not isinstance(payload, dict):
        raise RuntimeError("plan JSON must be an object")

    plan_digest = _expect_plan_str(payload, "plan_digest")
    payload_without_digest = dict(payload)
    payload_without_digest.pop("plan_digest", None)
    actual_plan_digest = compute_plan_digest(payload_without_digest)
    if _normalize_source_line_hash(plan_digest) != _normalize_source_line_hash(actual_plan_digest):
        raise RuntimeError(
            f"plan digest mismatch expected={plan_digest} actual={actual_plan_digest}"
        )

    catalog_digest = _expect_plan_str(payload, "catalog_digest")
    lane_filter = _expect_plan_optional_str(payload, "lane_filter")
    if lane_filter is not None and lane_filter not in {"A", "B", "C", "D"}:
        raise RuntimeError("plan JSON key 'lane_filter' must be one of A/B/C/D or null")
    task_id_prefix = _expect_plan_str(payload, "task_id_prefix")
    commit_sha = _expect_plan_optional_str(payload, "commit_sha")

    raw_candidates = payload.get("candidates")
    if not isinstance(raw_candidates, list):
        raise RuntimeError("plan JSON missing 'candidates' array")

    actions: list[PlannedAction] = []
    for raw in raw_candidates:
        if not isinstance(raw, dict):
            raise RuntimeError("plan JSON candidates must be objects")
        task_id = _expect_plan_str(raw, "task_id")
        issue_number = _expect_plan_int(raw, "issue_number")
        issue_url = _expect_plan_str(raw, "issue_url")
        source_path = _expect_plan_str(raw, "source_path")
        source_line = _expect_plan_int(raw, "source_line")
        source_checkbox_line = _expect_plan_str(raw, "source_checkbox_line")
        task_key_raw = raw.get("task_key")
        if task_key_raw is None:
            task_key = None
        elif isinstance(task_key_raw, str) and task_key_raw:
            task_key = task_key_raw
        else:
            raise RuntimeError(
                f"plan candidate '{task_id}' key 'task_key' must be non-empty string when present"
            )

        actions.append(
            PlannedAction(
                task_id=task_id,
                task_key=task_key,
                issue_number=issue_number,
                issue_url=issue_url,
                source_path=source_path,
                source_line=source_line,
                source_checkbox_line=source_checkbox_line,
            )
        )

    candidate_task_ids = payload.get("candidate_task_ids")
    if not isinstance(candidate_task_ids, list) or not all(
        isinstance(item, str) for item in candidate_task_ids
    ):
        raise RuntimeError("plan JSON key 'candidate_task_ids' must be an array of strings")
    candidate_issue_ids = payload.get("candidate_issue_ids")
    if not isinstance(candidate_issue_ids, list) or not all(
        isinstance(item, int) for item in candidate_issue_ids
    ):
        raise RuntimeError("plan JSON key 'candidate_issue_ids' must be an array of integers")

    derived_task_ids = [action.task_id for action in actions]
    derived_issue_ids = [action.issue_number for action in actions]
    if candidate_task_ids != derived_task_ids:
        raise RuntimeError("plan candidate_task_ids does not match candidates payload")
    if candidate_issue_ids != derived_issue_ids:
        raise RuntimeError("plan candidate_issue_ids does not match candidates payload")

    return LoadedPlan(
        catalog_digest=catalog_digest,
        lane_filter=lane_filter,
        task_id_prefix=task_id_prefix,
        commit_sha=commit_sha,
        actions=actions,
    )


def source_line_result(path: Path, line: int, expected_hash: str | None) -> SourceLineResult:
    if not path.exists():
        return SourceLineResult(
            raw_line=None,
            checked=False,
            display_line="<missing file>",
            stale_reason="missing-file",
        )

    lines = path.read_text(encoding="utf-8").splitlines()
    if line <= 0 or line > len(lines):
        return SourceLineResult(
            raw_line=None,
            checked=False,
            display_line="<line out of range>",
            stale_reason="line-out-of-range",
        )

    raw_line = lines[line - 1]
    display_line = raw_line.strip()
    checked = display_line.startswith("- [x]") or display_line.startswith("- [X]")
    if expected_hash is None:
        return SourceLineResult(
            raw_line=raw_line,
            checked=checked,
            display_line=display_line,
            stale_reason=None,
        )

    actual_hash = _normalize_source_line_hash(compute_source_line_hash(raw_line))
    expected_hash_norm = _normalize_source_line_hash(expected_hash)
    if actual_hash != expected_hash_norm:
        return SourceLineResult(
            raw_line=raw_line,
            checked=checked,
            display_line=display_line,
            stale_reason=f"hash-mismatch expected={expected_hash} actual=sha256:{actual_hash}",
        )

    return SourceLineResult(
        raw_line=raw_line,
        checked=checked,
        display_line=display_line,
        stale_reason=None,
    )


def close_issue(number: int, comment: str) -> None:
    proc = run_cmd(["gh", "issue", "close", str(number), "--comment", comment])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"failed to close issue #{number}: {detail}")


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="close_spt_issues_from_checkboxes.py",
        description=(
            "Detect checked checklist rows from the 510-task catalog and optionally "
            "close matching open SPT issues."
        ),
    )
    parser.add_argument(
        "--config",
        type=Path,
        default=None,
        help=(
            "Optional path to tooling config JSON. When omitted, built-in defaults are used; "
            "built-in defaults match pre-HB-06 behavior."
        ),
    )
    parser.add_argument(
        "--catalog",
        type=Path,
        default=None,
        help=(
            "Path to remaining_task_review_catalog.json. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--task-id-prefix",
        default=None,
        help=(
            "Only consider task IDs with this prefix. "
            "Default resolves from config/built-in value."
        ),
    )
    parser.add_argument(
        "--lane",
        choices=["A", "B", "C", "D"],
        default=None,
        help="Optional lane filter based on issue title tag '[Lane X]'.",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually close matched open issues. Default is dry-run.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Optional maximum number of issues to close/apply.",
    )
    parser.add_argument(
        "--commit-sha",
        default="",
        help="Optional commit SHA to include in closeout comment.",
    )
    parser.add_argument(
        "--plan-out",
        type=Path,
        default=None,
        help="Write immutable action-plan JSON and exit without closing issues.",
    )
    parser.add_argument(
        "--plan-in",
        type=Path,
        default=None,
        help="Read immutable action-plan JSON; required when --apply is set.",
    )
    parser.add_argument(
        "--fail-on-stale-source",
        action="store_true",
        help=(
            "Exit with code 2 if catalog source references are stale (missing file, line out of "
            "range, or source_line_hash mismatch)."
        ),
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    if args.apply and args.plan_in is None:
        print("error: --apply requires --plan-in <path>", file=sys.stderr)
        return 2
    if args.apply and args.plan_out is not None:
        print("error: --plan-out cannot be used with --apply", file=sys.stderr)
        return 2

    config = load_close_tooling_config(args.config)

    catalog = (
        config.catalog_default
        if args.catalog is None
        else (args.catalog if args.catalog.is_absolute() else ROOT / args.catalog)
    )
    plan_out_path = (
        None
        if args.plan_out is None
        else (args.plan_out if args.plan_out.is_absolute() else ROOT / args.plan_out)
    )
    plan_in_path = (
        None
        if args.plan_in is None
        else (args.plan_in if args.plan_in.is_absolute() else ROOT / args.plan_in)
    )

    loaded_plan: LoadedPlan | None = None
    effective_lane = args.lane
    effective_task_id_prefix = (
        args.task_id_prefix if args.task_id_prefix is not None else config.task_id_prefix_default
    )
    effective_commit_sha = args.commit_sha or None

    if args.apply:
        if plan_in_path is None:
            print("error: --plan-in path resolution failed", file=sys.stderr)
            return 2
        try:
            loaded_plan = load_plan(plan_in_path)
        except RuntimeError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 2
        if args.lane is not None and args.lane != loaded_plan.lane_filter:
            print(
                f"error: lane filter mismatch between CLI ({args.lane}) and plan ({loaded_plan.lane_filter})",
                file=sys.stderr,
            )
            return 2
        if args.task_id_prefix is not None and args.task_id_prefix != loaded_plan.task_id_prefix:
            print(
                "error: task-id-prefix mismatch between CLI and plan",
                file=sys.stderr,
            )
            return 2
        if args.commit_sha and args.commit_sha != (loaded_plan.commit_sha or ""):
            print(
                "error: commit-sha mismatch between CLI and plan",
                file=sys.stderr,
            )
            return 2
        effective_lane = loaded_plan.lane_filter
        effective_task_id_prefix = loaded_plan.task_id_prefix
        effective_commit_sha = loaded_plan.commit_sha

    catalog_tasks = load_catalog(catalog)
    live_catalog_digest = compute_file_digest(catalog)
    if loaded_plan is not None:
        if _normalize_source_line_hash(loaded_plan.catalog_digest) != _normalize_source_line_hash(
            live_catalog_digest
        ):
            print(
                "error: plan/catalog digest mismatch; plan is stale against current catalog",
                file=sys.stderr,
            )
            print(
                f"plan_catalog_digest={loaded_plan.catalog_digest} live_catalog_digest={live_catalog_digest}",
                file=sys.stderr,
            )
            return 2

    if effective_lane:
        filtered = [
            task
            for task in catalog_tasks
            if f"[Lane {effective_lane}]" in task.title
        ]
    else:
        filtered = catalog_tasks

    filtered = [
        task for task in filtered if task.task_id.startswith(effective_task_id_prefix)
    ]

    stale_entries: list[tuple[CatalogTask, str]] = []
    source_state_by_task_id: dict[str, SourceLineResult] = {}
    task_by_task_id: dict[str, CatalogTask] = {}
    for task in filtered:
        source_state = source_line_result(task.path, task.line, task.source_line_hash)
        source_state_by_task_id[task.task_id] = source_state
        task_by_task_id[task.task_id] = task
        if source_state.stale_reason is not None:
            stale_entries.append((task, source_state.stale_reason))
            continue

    if args.fail_on_stale_source and stale_entries:
        for task, reason in stale_entries:
            rel = _display_path(task.path)
            key_info = f" task_key={task.task_key}" if task.task_key else ""
            hash_info = f" source_line_hash={task.source_line_hash}" if task.source_line_hash else ""
            print(
                f"stale_source {task.task_id}{key_info} source={rel}:{task.line}{hash_info} reason={reason}",
                file=sys.stderr,
            )
        print(
            f"error: detected {len(stale_entries)} stale source entries; "
            "rerun catalog generation or update source references.",
            file=sys.stderr,
        )
        return 2

    planned_actions: list[PlannedAction]
    if loaded_plan is not None:
        invalid_plan_actions: list[str] = []
        for action in loaded_plan.actions:
            task = task_by_task_id.get(action.task_id)
            state = source_state_by_task_id.get(action.task_id)
            if task is None or state is None:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} not found in filtered catalog scope"
                )
                continue
            if state.stale_reason is not None:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} stale_source={state.stale_reason}"
                )
                continue
            if not state.checked:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} source checkbox is no longer checked"
                )
                continue
            if action.source_path != _display_path(task.path) or action.source_line != task.line:
                invalid_plan_actions.append(
                    f"plan_action {action.task_id} source does not match current catalog entry"
                )
                continue

        if invalid_plan_actions:
            for entry in invalid_plan_actions:
                print(entry, file=sys.stderr)
            print(
                f"error: detected {len(invalid_plan_actions)} invalid plan action(s); aborting apply",
                file=sys.stderr,
            )
            return 2

        planned_actions = loaded_plan.actions
    else:
        open_map = fetch_open_spt_issues(config.task_id_pattern)
        checked_candidates: list[tuple[CatalogTask, IssueRef, str]] = []
        for task in filtered:
            state = source_state_by_task_id[task.task_id]
            if state.stale_reason is not None:
                continue
            issue = open_map.get(task.task_id)
            if issue is None:
                continue
            if not state.checked:
                continue
            checked_candidates.append((task, issue, state.display_line))

        actions = checked_candidates
        if args.limit is not None:
            actions = actions[: max(0, args.limit)]
        planned_actions = build_planned_actions(actions)

    print(
        f"catalog_tasks={len(catalog_tasks)} filtered={len(filtered)} "
        f"checked_open_matches={len(planned_actions)} stale_source={len(stale_entries)}"
    )

    for action in planned_actions:
        print(
            f"match {action.task_id} -> #{action.issue_number} ({action.issue_url}) "
            f"source={action.source_path}:{action.source_line}"
        )

    if plan_out_path is not None:
        payload = build_plan_payload(
            catalog_path=catalog,
            catalog_digest=live_catalog_digest,
            lane_filter=effective_lane,
            task_id_prefix=effective_task_id_prefix,
            commit_sha=effective_commit_sha,
            actions=planned_actions,
        )
        write_plan(plan_out_path, payload)
        print(
            f"plan_written path={_display_path(plan_out_path)} actions={len(planned_actions)} "
            f"digest={payload['plan_digest']}"
        )
        print("dry-run only; pass --apply --plan-in <path> to close issues")
        return 0

    if not args.apply:
        print("dry-run only; pass --plan-out <path> to freeze close actions")
        return 0

    closed = 0
    for action in planned_actions:
        comment_lines = [
            "## Automated Closeout",
            f"- Task ID: `{action.task_id}`",
            f"- Source checkbox confirmed checked: `{action.source_path}:{action.source_line}`",
            f"- Source line: `{action.source_checkbox_line}`",
            "- Closeout mode: checklist-to-issue reconciliation",
        ]
        if effective_commit_sha:
            comment_lines.append(f"- Commit evidence: `{effective_commit_sha}`")

        comment = "\n".join(comment_lines)
        close_issue(action.issue_number, comment)
        closed += 1
        print(f"closed #{action.issue_number} ({action.task_id})")

    print(f"closed_total={closed}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(2)
