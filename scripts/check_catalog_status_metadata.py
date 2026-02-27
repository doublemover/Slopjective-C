#!/usr/bin/env python3
"""Validate execution-status metadata completeness in remaining_task_review_catalog.json."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"


def normalize_text(value: Any) -> str:
    if not isinstance(value, str):
        return ""
    return " ".join(value.strip().split())


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_catalog(path: Path) -> dict[str, Any]:
    if not path.exists():
        raise ValueError(f"catalog file does not exist: {display_path(path)}")
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {display_path(path)}: {exc}") from exc

    if not isinstance(payload, dict):
        raise ValueError("catalog JSON root must be an object")

    tasks = payload.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError("catalog JSON must contain a 'tasks' array")

    return payload


def parse_lane_filters(raw: Sequence[str] | None) -> set[str]:
    if not raw:
        return set()
    lanes: set[str] = set()
    for value in raw:
        lane = normalize_text(value).upper()
        if not lane:
            raise ValueError("lane filters must be non-empty")
        lanes.add(lane)
    return lanes


def task_label(raw_task: dict[str, Any], index: int) -> str:
    task_id = normalize_text(raw_task.get("task_id"))
    if task_id:
        return task_id
    return f"index:{index}"


def validate_metadata(
    payload: dict[str, Any],
    *,
    lane_filters: set[str],
) -> tuple[list[str], list[str]]:
    tasks = payload["tasks"]
    assert isinstance(tasks, list)

    missing_rationale: list[str] = []
    missing_evidence: list[str] = []

    for index, raw in enumerate(tasks):
        if not isinstance(raw, dict):
            missing_rationale.append(f"index:{index}")
            missing_evidence.append(f"index:{index}")
            continue

        lane = normalize_text(raw.get("lane")).upper()
        if lane_filters and lane not in lane_filters:
            continue

        label = task_label(raw, index)

        rationale = normalize_text(raw.get("execution_status_rationale"))
        if not rationale:
            missing_rationale.append(label)

        evidence_raw = raw.get("execution_status_evidence_refs")
        evidence_ok = False
        if isinstance(evidence_raw, list):
            evidence_ok = any(normalize_text(item) for item in evidence_raw)
        if not evidence_ok:
            missing_evidence.append(label)

    return missing_rationale, missing_evidence


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_catalog_status_metadata.py",
        description=(
            "Validate that each catalog row has execution-status rationale and "
            "evidence references."
        ),
    )
    parser.add_argument(
        "--catalog",
        type=Path,
        default=DEFAULT_CATALOG,
        help="Path to remaining_task_review_catalog.json.",
    )
    parser.add_argument(
        "--lane",
        action="append",
        default=None,
        help="Optional lane filter (repeatable).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    catalog_path = args.catalog if args.catalog.is_absolute() else ROOT / args.catalog

    try:
        payload = load_catalog(catalog_path)
        lane_filters = parse_lane_filters(args.lane)
        missing_rationale, missing_evidence = validate_metadata(
            payload,
            lane_filters=lane_filters,
        )
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    overlap = sorted(set(missing_rationale) & set(missing_evidence))
    print("catalog-status-metadata summary:")
    print(f"- catalog={display_path(catalog_path)}")
    if lane_filters:
        print(f"- lane_filter={sorted(lane_filters)}")
    else:
        print("- lane_filter=[]")
    print(f"- missing_rationale={len(missing_rationale)}")
    print(f"- missing_evidence_refs={len(missing_evidence)}")
    print(f"- missing_both={len(overlap)}")

    if missing_rationale:
        preview = ", ".join(missing_rationale[:20])
        suffix = "" if len(missing_rationale) <= 20 else f", ... (+{len(missing_rationale) - 20})"
        print(f"- rationale_preview={preview}{suffix}")
    if missing_evidence:
        preview = ", ".join(missing_evidence[:20])
        suffix = "" if len(missing_evidence) <= 20 else f", ... (+{len(missing_evidence) - 20})"
        print(f"- evidence_preview={preview}{suffix}")

    if missing_rationale or missing_evidence:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
