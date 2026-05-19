from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from .digests import (
    compute_file_digest_bytes,
    compute_plan_digest,
    compute_source_line_hash,
    normalize_source_line_hash,
)
from .models import CatalogTask, IssueRef, LoadedPlan, PlannedAction
from .paths import display_path


def compute_file_digest(path: Path) -> str:
    return compute_file_digest_bytes(path.read_bytes())


def utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def build_planned_actions(candidates: list[tuple[CatalogTask, IssueRef, str]]) -> list[PlannedAction]:
    return [
        PlannedAction(
            task_id=task.task_id,
            task_key=task.task_key,
            issue_number=issue.number,
            issue_url=issue.url,
            source_path=display_path(task.path),
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
        "generated_at": utc_timestamp(),
        "catalog_path": display_path(catalog_path),
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


def expect_plan_str(payload: dict[str, Any], key: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"plan JSON missing non-empty string key '{key}'")
    return value


def expect_plan_optional_str(payload: dict[str, Any], key: str) -> str | None:
    value = payload.get(key)
    if value is None:
        return None
    if not isinstance(value, str):
        raise RuntimeError(f"plan JSON key '{key}' must be a string or null")
    if value == "":
        return None
    return value


def expect_plan_int(payload: dict[str, Any], key: str) -> int:
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

    plan_digest = expect_plan_str(payload, "plan_digest")
    payload_without_digest = dict(payload)
    payload_without_digest.pop("plan_digest", None)
    actual_plan_digest = compute_plan_digest(payload_without_digest)
    if normalize_source_line_hash(plan_digest) != normalize_source_line_hash(actual_plan_digest):
        raise RuntimeError(
            f"plan digest mismatch expected={plan_digest} actual={actual_plan_digest}"
        )

    catalog_digest = expect_plan_str(payload, "catalog_digest")
    lane_filter = expect_plan_optional_str(payload, "lane_filter")
    if lane_filter is not None and lane_filter not in {"A", "B", "C", "D"}:
        raise RuntimeError("plan JSON key 'lane_filter' must be one of A/B/C/D or null")
    task_id_prefix = expect_plan_str(payload, "task_id_prefix")
    commit_sha = expect_plan_optional_str(payload, "commit_sha")

    raw_candidates = payload.get("candidates")
    if not isinstance(raw_candidates, list):
        raise RuntimeError("plan JSON missing 'candidates' array")

    actions: list[PlannedAction] = []
    for raw in raw_candidates:
        if not isinstance(raw, dict):
            raise RuntimeError("plan JSON candidates must be objects")
        task_id = expect_plan_str(raw, "task_id")
        issue_number = expect_plan_int(raw, "issue_number")
        issue_url = expect_plan_str(raw, "issue_url")
        source_path = expect_plan_str(raw, "source_path")
        source_line = expect_plan_int(raw, "source_line")
        source_checkbox_line = expect_plan_str(raw, "source_checkbox_line")
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
