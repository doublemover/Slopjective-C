#!/usr/bin/env python3
"""Generate deterministic compiler milestone dispatch plans from GH snapshots."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ISSUES_JSON = ROOT / "tmp" / "gh_open_issues_pages.json"
LANE_ORDER: tuple[str, ...] = ("A", "B", "C", "D", "E")
TASK_ID_RE = re.compile(r"\[M(?P<milestone>\d+)-(?P<lane>[A-E])(?P<seq>\d{3})\]")


@dataclass(frozen=True)
class IssueRow:
    number: int
    title: str
    milestone_number: int
    milestone_title: str
    lane_labels: tuple[str, ...]


@dataclass(frozen=True)
class TaskRef:
    issue_number: int
    task_id: str
    sequence: int
    title: str


def normalize_path(path: Path) -> Path:
    if path.is_absolute():
        return path
    return ROOT / path


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def flatten_json_pages(payload: Any) -> list[dict[str, Any]]:
    if isinstance(payload, list):
        flattened: list[dict[str, Any]] = []
        if payload and all(isinstance(item, list) for item in payload):
            for page in payload:
                for row in page:
                    if isinstance(row, dict):
                        flattened.append(row)
            return flattened

        for row in payload:
            if isinstance(row, dict):
                flattened.append(row)
        return flattened
    raise ValueError("issues JSON must be an array or slurped page-array")


def parse_lane_labels(raw_labels: Any) -> tuple[str, ...]:
    if not isinstance(raw_labels, list):
        return ()
    lanes: set[str] = set()
    for raw in raw_labels:
        if not isinstance(raw, dict):
            continue
        name = raw.get("name")
        if not isinstance(name, str):
            continue
        if name.startswith("lane:"):
            lanes.add(name)
    return tuple(sorted(lanes))


def parse_issue_rows(path: Path) -> list[IssueRow]:
    try:
        raw_payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"issues JSON file does not exist: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read issues JSON file {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {display_path(path)}: {exc}") from exc

    rows: list[IssueRow] = []
    for raw in flatten_json_pages(raw_payload):
        if "pull_request" in raw:
            continue

        milestone = raw.get("milestone")
        if not isinstance(milestone, dict):
            continue
        milestone_number = milestone.get("number")
        milestone_title = milestone.get("title")
        number = raw.get("number")
        title = raw.get("title")
        if not isinstance(milestone_number, int):
            continue
        if not isinstance(milestone_title, str):
            continue
        if not isinstance(number, int):
            continue
        if not isinstance(title, str):
            continue

        rows.append(
            IssueRow(
                number=number,
                title=normalize_space(title),
                milestone_number=milestone_number,
                milestone_title=normalize_space(milestone_title),
                lane_labels=parse_lane_labels(raw.get("labels")),
            )
        )

    if not rows:
        raise ValueError("no open issue rows with milestones found in issues snapshot")
    return rows


def pick_target_milestone(rows: Sequence[IssueRow], explicit_number: int | None) -> int:
    if explicit_number is not None:
        matches = {row.milestone_number for row in rows if row.milestone_number == explicit_number}
        if not matches:
            raise ValueError(
                f"milestone {explicit_number} not present in provided open issue snapshot"
            )
        return explicit_number

    return min(row.milestone_number for row in rows)


def parse_task_ref(row: IssueRow, lane: str) -> TaskRef | None:
    match = TASK_ID_RE.search(row.title)
    if not match:
        return None
    parsed_lane = match.group("lane")
    if parsed_lane != lane:
        return None
    task_id = f"M{int(match.group('milestone')):02d}-{parsed_lane}{match.group('seq')}"
    return TaskRef(
        issue_number=row.number,
        task_id=task_id,
        sequence=int(match.group("seq")),
        title=row.title,
    )


def build_payload(rows: Sequence[IssueRow], *, milestone_number: int, top_n: int) -> dict[str, Any]:
    target_rows = [row for row in rows if row.milestone_number == milestone_number]
    if not target_rows:
        raise ValueError(f"no rows available for milestone {milestone_number}")

    milestone_title = sorted({row.milestone_title for row in target_rows}, key=str.casefold)[0]
    lane_summaries: list[dict[str, Any]] = []
    for lane in LANE_ORDER:
        lane_rows = [row for row in target_rows if f"lane:{lane}" in row.lane_labels]
        tasks: list[TaskRef] = []
        for row in lane_rows:
            parsed = parse_task_ref(row, lane)
            if parsed is not None:
                tasks.append(parsed)
        tasks.sort(key=lambda entry: (entry.sequence, entry.issue_number, entry.task_id))

        lane_summaries.append(
            {
                "lane": lane,
                "open_issue_count": len(lane_rows),
                "next_tasks": [
                    {
                        "issue_number": task.issue_number,
                        "task_id": task.task_id,
                        "sequence": task.sequence,
                        "title": task.title,
                    }
                    for task in tasks[:top_n]
                ],
            }
        )

    int_rows = [row for row in target_rows if "lane:INT" in row.lane_labels]
    int_rows.sort(key=lambda row: (row.number, row.title.casefold(), row.title))
    parallel_lanes = [
        lane_summary["lane"]
        for lane_summary in lane_summaries
        if lane_summary["open_issue_count"] > 0
    ]
    return {
        "source": {
            "issues_json": display_path(DEFAULT_ISSUES_JSON),
        },
        "milestone": {
            "number": milestone_number,
            "title": milestone_title,
            "open_issue_count": len(target_rows),
        },
        "parallelization": {
            "parallel_lanes": parallel_lanes,
            "regroup_dependency": [row.number for row in int_rows],
            "regroup_dependency_note": (
                "INT regroup is lane-gated and should execute only after parallel lanes complete."
            ),
        },
        "lanes": lane_summaries,
    }


def render_markdown(payload: dict[str, Any]) -> str:
    milestone = payload["milestone"]
    parallelization = payload["parallelization"]
    lanes = payload["lanes"]

    lines: list[str] = [
        "# Compiler Dispatch Plan",
        "",
        "## Milestone",
        "",
        f"- Number: **{milestone['number']}**",
        f"- Title: **{milestone['title']}**",
        f"- Open issues in milestone: **{milestone['open_issue_count']}**",
        "",
        "## Parallelization",
        "",
        (
            "- Parallel lanes: "
            + ", ".join(f"`{lane}`" for lane in parallelization["parallel_lanes"])
        ),
        (
            "- Regroup dependency issues: "
            + (
                ", ".join(f"`#{number}`" for number in parallelization["regroup_dependency"])
                if parallelization["regroup_dependency"]
                else "_none_"
            )
        ),
        f"- Note: {parallelization['regroup_dependency_note']}",
        "",
        "## Next Tasks",
        "",
    ]

    for lane_summary in lanes:
        lane = lane_summary["lane"]
        lines.append(
            f"### Lane `{lane}` ({lane_summary['open_issue_count']} open issues)"
        )
        next_tasks = lane_summary["next_tasks"]
        if next_tasks:
            for task in next_tasks:
                lines.append(
                    f"- `#{task['issue_number']}` `{task['task_id']}`: {task['title']}"
                )
        else:
            lines.append("- _No parseable lane task IDs found._")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def render_json(payload: dict[str, Any]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Generate deterministic lane-parallel compiler dispatch summaries."
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        default=DEFAULT_ISSUES_JSON,
        help=f"Path to gh open-issues snapshot (default: {display_path(DEFAULT_ISSUES_JSON)}).",
    )
    parser.add_argument(
        "--milestone-number",
        type=int,
        help="Optional explicit milestone number; defaults to the lowest open milestone in snapshot.",
    )
    parser.add_argument(
        "--top-n",
        type=int,
        default=3,
        help="Number of next tasks to emit per lane (default: 3).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format (default: json).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.top_n <= 0:
        print("error: --top-n must be > 0", file=sys.stderr)
        return 2

    issues_json_path = normalize_path(args.issues_json)
    try:
        rows = parse_issue_rows(issues_json_path)
        milestone_number = pick_target_milestone(rows, args.milestone_number)
        payload = build_payload(rows, milestone_number=milestone_number, top_n=args.top_n)
        payload["source"]["issues_json"] = display_path(issues_json_path)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.format == "markdown":
        sys.stdout.write(render_markdown(payload))
    else:
        sys.stdout.write(render_json(payload))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
