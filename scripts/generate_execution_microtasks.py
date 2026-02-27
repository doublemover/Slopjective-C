#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
import sys
from dataclasses import dataclass
from datetime import date, datetime, timezone
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG_JSON = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"


@dataclass(frozen=True)
class Issue:
    number: int
    title: str
    labels: tuple[str, ...]


def parse_non_negative_int(raw: str) -> int:
    try:
        value = int(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer value: {raw!r}") from exc

    if value < 0:
        raise argparse.ArgumentTypeError("--closed-count must be >= 0")
    return value


def parse_generated_on(raw: str) -> date:
    try:
        parsed = date.fromisoformat(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "--generated-on must be in YYYY-MM-DD format"
        ) from exc
    return parsed


def source_date_epoch_to_date(value: str) -> date:
    try:
        epoch = int(value)
    except ValueError as exc:
        raise ValueError(f"SOURCE_DATE_EPOCH must be an integer; got {value!r}") from exc

    if epoch < 0:
        raise ValueError("SOURCE_DATE_EPOCH must be >= 0")

    try:
        return datetime.fromtimestamp(epoch, tz=timezone.utc).date()
    except (OverflowError, OSError, ValueError) as exc:
        raise ValueError(
            f"SOURCE_DATE_EPOCH is out of range for UTC conversion: {value!r}"
        ) from exc


def resolve_generated_on(*, generated_on: date | None, snapshot_date: date | None) -> date:
    if generated_on is not None:
        return generated_on

    if snapshot_date is not None:
        return snapshot_date

    source_date_epoch = os.getenv("SOURCE_DATE_EPOCH")
    if source_date_epoch:
        return source_date_epoch_to_date(source_date_epoch)

    raise ValueError(
        "must provide --generated-on (or legacy --snapshot-date) when SOURCE_DATE_EPOCH is not set"
    )


def normalize_line_endings(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def normalize_inline_text(value: str) -> str:
    normalized = normalize_line_endings(value).strip()
    if "\n" not in normalized:
        return normalized
    parts = [part.strip() for part in normalized.split("\n") if part.strip()]
    return " ".join(parts).strip()


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def parse_issue_number(raw: Any, index: int) -> int:
    if isinstance(raw, int):
        return raw

    if isinstance(raw, str) and raw.isdigit():
        return int(raw)

    raise ValueError(
        f"issue at index {index} has invalid 'number' value {raw!r}; expected int"
    )


def parse_issue_title(raw: Any, index: int) -> str:
    if not isinstance(raw, str):
        raise ValueError(
            f"issue at index {index} has invalid 'title' value {raw!r}; expected str"
        )
    return normalize_inline_text(raw)


def parse_labels(raw: Any) -> tuple[str, ...]:
    if raw is None:
        return ()
    if not isinstance(raw, list):
        return ()

    names: set[str] = set()
    for item in raw:
        if isinstance(item, str):
            label_name = normalize_inline_text(item)
            if label_name:
                names.add(label_name)
            continue
        if isinstance(item, dict):
            candidate = item.get("name")
            if isinstance(candidate, str):
                label_name = normalize_inline_text(candidate)
                if label_name:
                    names.add(label_name)
    return tuple(sorted(names, key=lambda label: (label.casefold(), label)))


def parse_catalog_task_identifier(raw: Any, index: int) -> str:
    if isinstance(raw, str):
        task_id = normalize_inline_text(raw)
        if task_id:
            return task_id
    return f"index:{index}"


def validate_catalog_status_integrity(path: Path, *, allow_missing_status: bool) -> None:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(
            f"catalog JSON file does not exist: {display_path(path)}"
        ) from exc
    except OSError as exc:
        raise ValueError(
            f"unable to read catalog JSON {display_path(path)}: {exc}"
        ) from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"invalid JSON in {display_path(path)}: {exc}"
        ) from exc

    if not isinstance(payload, dict):
        raise ValueError("catalog JSON root must be an object")

    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise ValueError("catalog JSON missing 'tasks' array")

    missing_status_task_ids: list[str] = []
    for index, raw_task in enumerate(raw_tasks):
        if not isinstance(raw_task, dict):
            raise ValueError(
                f"task at index {index} must be an object"
            )

        raw_status = raw_task.get("execution_status")
        if isinstance(raw_status, str) and raw_status.strip():
            continue

        missing_status_task_ids.append(
            parse_catalog_task_identifier(raw_task.get("task_id"), index)
        )

    if not missing_status_task_ids or allow_missing_status:
        return

    preview = ", ".join(missing_status_task_ids[:5])
    if len(missing_status_task_ids) > 5:
        preview += ", ..."

    raise ValueError(
        "catalog status integrity check failed: "
        f"{len(missing_status_task_ids)} task(s) are missing required "
        f"'execution_status' (examples: {preview}); pass "
        "--allow-missing-status to continue"
    )


def load_issues(path: Path) -> list[Issue]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"issues JSON file does not exist: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(payload, list):
        raise ValueError("issues JSON root must be an array")

    issues: list[Issue] = []
    for index, raw_issue in enumerate(payload):
        if not isinstance(raw_issue, dict):
            raise ValueError(
                f"issue at index {index} must be an object, got {type(raw_issue).__name__}"
            )

        number = parse_issue_number(raw_issue.get("number"), index)
        title = parse_issue_title(raw_issue.get("title"), index)
        labels = parse_labels(raw_issue.get("labels"))
        issues.append(Issue(number=number, title=title, labels=labels))

    return sorted(issues, key=lambda issue: (issue.number, issue.title.casefold(), issue.title))


def render_markdown(*, issues: list[Issue], closed_count: int, generated_on: date) -> str:
    open_count = len(issues)
    minimum_target = closed_count * 2
    planned_microtasks = open_count * 4

    lines: list[str] = [
        "# Execution Microtask Backlog (2x+ Expansion)",
        "",
        f"_Generated on {generated_on.isoformat()} from GitHub issue snapshot._",
        "",
        "## Baseline Metrics",
        "",
        f"- Snapshot date: **{generated_on.isoformat()}**",
        f"- Closed issues (completed tasks): **{closed_count}**",
        f"- Open issues (remaining tasks): **{open_count}**",
        f"- Minimum future-task target (2x completed): **{minimum_target}**",
        f"- Planned microtasks in this backlog: **{planned_microtasks}** (4 per open issue)",
        "",
        "## Microtasks By Open Issue",
        "",
    ]

    if not issues:
        lines.append("No open issues currently.")
        lines.append("")
    else:
        for issue in issues:
            lines.append(f"### Issue #{issue.number}: {issue.title}")
            if issue.labels:
                lines.append(f"_Labels: {', '.join(issue.labels)}_")
            lines.append("")
            lines.append(
                f"1. **Implementation**: Implement issue #{issue.number} requirements and "
                "update code/docs as needed."
            )
            lines.append(
                f"2. **Verification**: Add or update automated/manual checks for issue "
                f"#{issue.number} and capture evidence."
            )
            lines.append(
                f"3. **Integration**: Validate dependent workflows and cross-component "
                f"behavior for issue #{issue.number}."
            )
            lines.append(
                "4. **Closeout sync**: Post commit and test evidence on issue "
                f"#{issue.number}, then synchronize related epic/workpack status."
            )
            lines.append("")

    lines.extend(
        [
            "## Totals Summary",
            "",
            f"- Issues represented: **{open_count}**",
            f"- Total generated microtasks: **{planned_microtasks}**",
            f"- Implementation microtasks: **{open_count}**",
            f"- Verification microtasks: **{open_count}**",
            f"- Integration microtasks: **{open_count}**",
            f"- Closeout sync microtasks: **{open_count}**",
        ]
    )

    return normalize_line_endings("\n".join(lines) + "\n")


def write_stdout(markdown: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(markdown.encode("utf-8"))
        return
    sys.stdout.write(markdown)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_execution_microtasks.py",
        description=(
            "Generate markdown for spec/EXECUTION_MICROTASK_BACKLOG.md from "
            "a GitHub issues JSON snapshot. When run without generation "
            "arguments, validates catalog execution_status integrity."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Expected JSON input comes from:\n"
            "  gh issue list --state open --json number,title,labels\n\n"
            "Example:\n"
            "  python scripts/generate_execution_microtasks.py \\\n"
            "    --issues-json tmp/open_issues.json \\\n"
            "    --closed-count 110 \\\n"
            "    --generated-on 2026-02-23 > spec/EXECUTION_MICROTASK_BACKLOG.md\n\n"
            "  SOURCE_DATE_EPOCH=1771804800 python scripts/generate_execution_microtasks.py \\\n"
            "    --issues-json tmp/open_issues.json \\\n"
            "    --closed-count 110 > spec/EXECUTION_MICROTASK_BACKLOG.md\n\n"
            "Status-integrity check mode:\n"
            "  python scripts/generate_execution_microtasks.py\n"
            "  python scripts/generate_execution_microtasks.py --allow-missing-status"
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        default=None,
        help=(
            "Path to a JSON array produced by "
            "`gh issue list --state open --json number,title,labels`."
        ),
    )
    parser.add_argument(
        "--closed-count",
        type=parse_non_negative_int,
        default=None,
        help="Count of closed issues at snapshot time.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=None,
        help=(
            "Path to remaining_task_review_catalog.json used for status-integrity "
            "checks. Defaults to spec/planning/remaining_task_review_catalog.json "
            "in check mode."
        ),
    )
    parser.add_argument(
        "--allow-missing-status",
        action="store_true",
        help=(
            "Allow missing/blank execution_status values during catalog integrity "
            "checks."
        ),
    )
    parser.add_argument(
        "--generated-on",
        type=parse_generated_on,
        default=None,
        help=(
            "Date stamp embedded in output (YYYY-MM-DD). "
            "If omitted, SOURCE_DATE_EPOCH is used when set."
        ),
    )
    parser.add_argument(
        "--snapshot-date",
        type=parse_generated_on,
        default=None,
        help=argparse.SUPPRESS,
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    generation_mode = args.issues_json is not None or args.closed_count is not None
    catalog_path = args.catalog_json if args.catalog_json is not None else DEFAULT_CATALOG_JSON

    try:
        if not generation_mode or args.catalog_json is not None:
            validate_catalog_status_integrity(
                catalog_path,
                allow_missing_status=args.allow_missing_status,
            )

        if not generation_mode:
            return 0

        if args.issues_json is None or args.closed_count is None:
            parser.error(
                "must provide both --issues-json and --closed-count when generating microtasks"
            )

        issues = load_issues(args.issues_json)
        generated_on = resolve_generated_on(
            generated_on=args.generated_on,
            snapshot_date=args.snapshot_date,
        )
    except ValueError as exc:
        parser.error(str(exc))

    markdown = render_markdown(
        issues=issues,
        closed_count=args.closed_count,
        generated_on=generated_on,
    )
    write_stdout(markdown)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
