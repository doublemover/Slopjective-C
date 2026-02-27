#!/usr/bin/env python3
"""Apply lane audit status overrides to remaining_task_review_catalog.json."""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ALLOWED_STATUSES = {"open", "open-blocked", "blocked", "complete"}


@dataclass(frozen=True)
class OverrideEntry:
    task_id: str
    recommended_status: str
    evidence_refs: tuple[str, ...]
    rationale: str
    source_path: Path


def normalize_text(value: str) -> str:
    return " ".join(value.strip().split())


def normalize_status(value: Any) -> str:
    if not isinstance(value, str):
        return ""
    return normalize_text(value).lower()


def task_row_label(raw_task: dict[str, Any], index: int) -> str:
    raw_task_id = raw_task.get("task_id")
    if isinstance(raw_task_id, str):
        task_id = normalize_text(raw_task_id)
        if task_id:
            return f"task_id '{task_id}'"
    return f"task at index {index}"


def read_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"JSON file not found: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {path}: {exc}") from exc


def parse_override_entry(raw: Any, source_path: Path, index: int) -> OverrideEntry:
    if not isinstance(raw, dict):
        raise ValueError(
            f"{source_path}: override entry at index {index} must be an object"
        )

    task_id_raw = raw.get("task_id")
    if not isinstance(task_id_raw, str) or not normalize_text(task_id_raw):
        raise ValueError(
            f"{source_path}: override entry {index} has invalid 'task_id'"
        )
    task_id = normalize_text(task_id_raw)

    status_raw = raw.get("recommended_status")
    if not isinstance(status_raw, str):
        raise ValueError(
            f"{source_path}: override entry {index} has invalid 'recommended_status'"
        )
    recommended_status = normalize_text(status_raw)
    if recommended_status not in ALLOWED_STATUSES:
        allowed = ", ".join(sorted(ALLOWED_STATUSES))
        raise ValueError(
            f"{source_path}: override entry {index} status '{recommended_status}' "
            f"is not allowed (allowed: {allowed})"
        )

    evidence_refs_raw = raw.get("evidence_refs", [])
    evidence_refs: list[str] = []
    if evidence_refs_raw is not None:
        if not isinstance(evidence_refs_raw, list):
            raise ValueError(
                f"{source_path}: override entry {index} has invalid 'evidence_refs'"
            )
        for item in evidence_refs_raw:
            if not isinstance(item, str):
                raise ValueError(
                    f"{source_path}: override entry {index} has non-string evidence ref"
                )
            cleaned = normalize_text(item)
            if cleaned:
                evidence_refs.append(cleaned)

    rationale_raw = raw.get("rationale", "")
    if not isinstance(rationale_raw, str):
        raise ValueError(
            f"{source_path}: override entry {index} has invalid 'rationale'"
        )
    rationale = normalize_text(rationale_raw)

    return OverrideEntry(
        task_id=task_id,
        recommended_status=recommended_status,
        evidence_refs=tuple(evidence_refs),
        rationale=rationale,
        source_path=source_path,
    )


def load_overrides(paths: Sequence[Path]) -> dict[str, OverrideEntry]:
    mapping: dict[str, OverrideEntry] = {}
    for path in paths:
        payload = read_json(path)
        raw_entries: list[Any]
        if isinstance(payload, list):
            raw_entries = payload
        elif isinstance(payload, dict):
            overrides_raw = payload.get("overrides")
            if not isinstance(overrides_raw, list):
                raise ValueError(
                    f"{path}: override JSON object must contain an 'overrides' array"
                )
            raw_entries = overrides_raw
        else:
            raise ValueError(f"{path}: override JSON root must be an array or object")

        for index, raw in enumerate(raw_entries):
            entry = parse_override_entry(raw, path, index)
            previous = mapping.get(entry.task_id)
            if previous is not None:
                raise ValueError(
                    f"duplicate override for task_id '{entry.task_id}' in "
                    f"{previous.source_path} and {entry.source_path}"
                )
            mapping[entry.task_id] = entry
    return mapping


def load_catalog(path: Path) -> dict[str, Any]:
    payload = read_json(path)
    if not isinstance(payload, dict):
        raise ValueError(f"{path}: catalog JSON root must be an object")
    tasks = payload.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"{path}: catalog JSON must contain a 'tasks' array")
    return payload


def validate_catalog_status_invariants(
    catalog: dict[str, Any],
    *,
    source: str,
    allow_missing_task_ids: set[str] | None = None,
) -> None:
    tasks = catalog.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"{source}: catalog JSON must contain a 'tasks' array")

    allowed = ", ".join(sorted(ALLOWED_STATUSES))
    allowed_missing = allow_missing_task_ids or set()
    problems: list[str] = []

    for index, raw_task in enumerate(tasks):
        if not isinstance(raw_task, dict):
            problems.append(f"{source}: task at index {index} must be an object")
            continue

        label = task_row_label(raw_task, index)
        raw_task_id = raw_task.get("task_id")
        normalized_task_id = ""
        if isinstance(raw_task_id, str):
            normalized_task_id = normalize_text(raw_task_id)
            if normalized_task_id:
                raw_task["task_id"] = normalized_task_id
        normalized_status = normalize_status(raw_task.get("execution_status"))
        if not normalized_status:
            if normalized_task_id and normalized_task_id in allowed_missing:
                continue
            problems.append(
                f"{source}: {label} is missing required 'execution_status' "
                f"(allowed: {allowed})"
            )
            continue
        if normalized_status not in ALLOWED_STATUSES:
            problems.append(
                f"{source}: {label} has invalid 'execution_status' value "
                f"'{normalized_status}' (allowed: {allowed})"
            )
            continue
        raw_task["execution_status"] = normalized_status

    if not problems:
        return

    preview = "\n".join(f"- {problem}" for problem in problems[:10])
    if len(problems) > 10:
        preview = f"{preview}\n- ... plus {len(problems) - 10} more issue(s)"
    raise ValueError(f"catalog status invariants failed:\n{preview}")


def apply_overrides(
    catalog: dict[str, Any],
    overrides: dict[str, OverrideEntry],
) -> tuple[list[dict[str, Any]], list[str]]:
    validate_catalog_status_invariants(
        catalog,
        source="catalog before overrides",
        allow_missing_task_ids=set(overrides.keys()),
    )

    tasks = catalog["tasks"]
    assert isinstance(tasks, list)

    touched_task_ids: set[str] = set()
    changes: list[dict[str, Any]] = []

    for raw_task in tasks:
        if not isinstance(raw_task, dict):
            continue
        task_id = raw_task.get("task_id")
        if not isinstance(task_id, str):
            continue

        entry = overrides.get(task_id)
        if entry is None:
            continue

        touched_task_ids.add(task_id)
        before_status_text = normalize_status(raw_task.get("execution_status"))
        raw_task["execution_status"] = entry.recommended_status
        raw_task["execution_status_rationale"] = entry.rationale
        raw_task["execution_status_evidence_refs"] = list(entry.evidence_refs)
        raw_task["execution_status_override_source"] = entry.source_path.as_posix()

        changes.append(
            {
                "task_id": task_id,
                "before_status": before_status_text or "(unset)",
                "after_status": entry.recommended_status,
                "override_source": entry.source_path.as_posix(),
            }
        )

    validate_catalog_status_invariants(catalog, source="catalog after overrides")

    missing = sorted(task_id for task_id in overrides if task_id not in touched_task_ids)
    return sorted(changes, key=lambda row: row["task_id"]), missing


def write_catalog(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def render_summary(changes: list[dict[str, Any]], missing: list[str]) -> str:
    after_counts = Counter(change["after_status"] for change in changes)

    lines = [
        "override-apply summary:",
        f"- changed_rows={len(changes)}",
        f"- changed_status_counts={dict(sorted(after_counts.items()))}",
    ]
    if missing:
        lines.append(f"- missing_task_ids={missing}")
    else:
        lines.append("- missing_task_ids=[]")
    return "\n".join(lines)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="apply_remaining_task_status_overrides.py",
        description=(
            "Apply per-lane status override JSON files to "
            "spec/planning/remaining_task_review_catalog.json."
        ),
    )
    parser.add_argument(
        "--catalog",
        type=Path,
        default=Path("spec/planning/remaining_task_review_catalog.json"),
        help="Path to remaining_task_review_catalog.json.",
    )
    parser.add_argument(
        "--overrides",
        type=Path,
        nargs="+",
        required=True,
        help="One or more override JSON files.",
    )
    parser.add_argument(
        "--write",
        action="store_true",
        help="Write changes back to --catalog. Without this flag, dry-run only.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        overrides = load_overrides(args.overrides)
        catalog = load_catalog(args.catalog)
        changes, missing = apply_overrides(catalog, overrides)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(render_summary(changes, missing))

    if missing:
        return 2

    if args.write:
        write_catalog(args.catalog, catalog)
        print(f"- catalog_updated={args.catalog.as_posix()}")
    else:
        print("- dry_run=true")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
