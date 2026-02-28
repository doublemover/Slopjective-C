#!/usr/bin/env python3
"""Lint unchecked planning checkboxes against explicit policy exceptions."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_GLOBS: tuple[str, ...] = ("spec/planning/**/*.md",)
DEFAULT_EXCLUDE_GLOBS: tuple[str, ...] = ()
DEFAULT_POLICY_PATH = Path("spec/planning/planning_checkbox_policy.json")

HEADING_RE = re.compile(r"^(#{1,6})\s+(.+?)\s*$")
HEADING_ANCHOR_SUFFIX_RE = re.compile(r"\s*\{#[^}]+\}\s*$")
UNCHECKED_ROW_RE = re.compile(r"^\s*-\s+\[ \]\s+(.+?)\s*$")


@dataclass(frozen=True)
class PolicyException:
    path: str
    section: str
    reason: str


@dataclass(frozen=True)
class UncheckedRow:
    path: str
    line: int
    section: str
    text: str


@dataclass(frozen=True)
class EvaluatedRow:
    path: str
    line: int
    section: str
    text: str
    allowed: bool
    reason: str | None


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def normalize_path_string(value: str) -> str:
    normalized = normalize_space(value).replace("\\", "/")
    return normalized.strip("/")


def normalize_glob(value: str) -> str:
    return value.replace("\\", "/")


def normalize_heading_text(raw_heading: str) -> str:
    without_anchor = HEADING_ANCHOR_SUFFIX_RE.sub("", raw_heading).strip()
    return normalize_space(without_anchor)


def display_path(path: Path, *, root: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(root.resolve()).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_root_path(raw_root: Path) -> Path:
    if raw_root.is_absolute():
        return raw_root
    return ROOT / raw_root


def resolve_policy_path(*, root: Path, raw_policy: Path) -> Path:
    if raw_policy.is_absolute():
        return raw_policy
    return root / raw_policy


def iter_markdown_paths(*, root: Path, globs: Iterable[str]) -> list[Path]:
    deduped: dict[str, Path] = {}
    for raw_glob in globs:
        normalized_glob = raw_glob.replace("\\", "/")
        for path in root.glob(normalized_glob):
            if not path.is_file():
                continue
            rel = display_path(path, root=root)
            deduped[rel] = path.resolve()

    return [deduped[key] for key in sorted(deduped.keys(), key=lambda item: (item.casefold(), item))]


def load_policy(*, path: Path, root: Path) -> list[PolicyException]:
    if not path.exists():
        raise ValueError(f"policy file does not exist: {display_path(path, root=root)}")

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"unable to read policy file {display_path(path, root=root)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in policy file {display_path(path, root=root)}: {exc}") from exc

    if not isinstance(payload, dict):
        raise ValueError("policy JSON root must be an object")

    raw_version = payload.get("version")
    if isinstance(raw_version, bool) or not isinstance(raw_version, int) or raw_version <= 0:
        raise ValueError("policy field 'version' must be a positive integer")

    raw_entries = payload.get("allowed_unchecked_sections")
    if not isinstance(raw_entries, list):
        raise ValueError("policy field 'allowed_unchecked_sections' must be an array")

    entries: list[PolicyException] = []
    seen: set[tuple[str, str]] = set()
    for index, raw in enumerate(raw_entries):
        context = f"allowed_unchecked_sections[{index}]"
        if not isinstance(raw, dict):
            raise ValueError(f"policy entry {context} must be an object")

        raw_path = raw.get("path")
        raw_section = raw.get("section")
        raw_reason = raw.get("reason")
        if not isinstance(raw_path, str) or not normalize_space(raw_path):
            raise ValueError(f"policy entry {context}.path must be a non-empty string")
        if not isinstance(raw_section, str) or not normalize_space(raw_section):
            raise ValueError(f"policy entry {context}.section must be a non-empty string")
        if not isinstance(raw_reason, str) or not normalize_space(raw_reason):
            raise ValueError(f"policy entry {context}.reason must be a non-empty string")

        policy_path = normalize_path_string(raw_path)
        section = normalize_space(raw_section)
        reason = normalize_space(raw_reason)
        key = (policy_path, section)
        if key in seen:
            raise ValueError(
                "duplicate policy exception for "
                f"path='{policy_path}' section='{section}'"
            )
        seen.add(key)
        entries.append(
            PolicyException(
                path=policy_path,
                section=section,
                reason=reason,
            )
        )

    return entries


def parse_unchecked_rows(path: Path, *, root: Path) -> list[UncheckedRow]:
    rel = display_path(path, root=root)
    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except OSError as exc:
        raise ValueError(f"unable to read markdown file {rel}: {exc}") from exc

    rows: list[UncheckedRow] = []
    section_stack: list[str] = []
    in_fence = False

    for line_number, line in enumerate(lines, start=1):
        stripped = line.strip()
        if stripped.startswith("```") or stripped.startswith("~~~"):
            in_fence = not in_fence
            continue
        if in_fence:
            continue

        heading_match = HEADING_RE.match(line)
        if heading_match:
            level = len(heading_match.group(1))
            heading_text = normalize_heading_text(heading_match.group(2))
            if heading_text:
                section_stack = section_stack[: level - 1]
                section_stack.append(heading_text)
            continue

        row_match = UNCHECKED_ROW_RE.match(line)
        if row_match is None:
            continue

        text = normalize_space(row_match.group(1))
        section = " > ".join(section_stack) if section_stack else "(no heading)"
        rows.append(
            UncheckedRow(
                path=rel,
                line=line_number,
                section=section,
                text=text,
            )
        )

    return rows


def evaluate_rows(
    rows: Sequence[UncheckedRow],
    *,
    policy_entries: Sequence[PolicyException],
) -> list[EvaluatedRow]:
    policy_map = {(entry.path, entry.section): entry.reason for entry in policy_entries}

    evaluated: list[EvaluatedRow] = []
    for row in rows:
        reason = policy_map.get((row.path, row.section))
        evaluated.append(
            EvaluatedRow(
                path=row.path,
                line=row.line,
                section=row.section,
                text=row.text,
                allowed=reason is not None,
                reason=reason,
            )
        )
    return evaluated


def collect_unused_policy_entries(
    *,
    evaluated_rows: Sequence[EvaluatedRow],
    policy_entries: Sequence[PolicyException],
) -> list[PolicyException]:
    used_policy_keys = {(row.path, row.section) for row in evaluated_rows if row.allowed}
    return [
        entry
        for entry in policy_entries
        if (entry.path, entry.section) not in used_policy_keys
    ]


def build_payload(
    *,
    root: Path,
    policy_path: Path,
    scanned_paths: Sequence[Path],
    evaluated_rows: Sequence[EvaluatedRow],
    unused_policy_entries: Sequence[PolicyException],
) -> dict[str, Any]:
    allowed_rows = [row for row in evaluated_rows if row.allowed]
    disallowed_rows = [row for row in evaluated_rows if not row.allowed]

    def row_to_dict(row: EvaluatedRow) -> dict[str, Any]:
        payload: dict[str, Any] = {
            "path": row.path,
            "line": row.line,
            "section": row.section,
            "text": row.text,
        }
        if row.reason is not None:
            payload["reason"] = row.reason
        return payload

    def policy_entry_to_dict(entry: PolicyException) -> dict[str, Any]:
        return {
            "path": entry.path,
            "section": entry.section,
            "reason": entry.reason,
        }

    return {
        "root": root.resolve().as_posix(),
        "policy_path": display_path(policy_path, root=root),
        "summary": {
            "scanned_file_count": len(scanned_paths),
            "unchecked_row_count": len(evaluated_rows),
            "allowed_count": len(allowed_rows),
            "disallowed_count": len(disallowed_rows),
            "unused_policy_entry_count": len(unused_policy_entries),
        },
        "allowed": [row_to_dict(row) for row in allowed_rows],
        "disallowed": [row_to_dict(row) for row in disallowed_rows],
        "unused_policy_entries": [policy_entry_to_dict(entry) for entry in unused_policy_entries],
    }


def render_human(payload: dict[str, Any], *, allow_unused_policy_entries: bool) -> str:
    summary = payload["summary"]
    assert isinstance(summary, dict)
    disallowed_rows = payload["disallowed"]
    assert isinstance(disallowed_rows, list)
    unused_policy_entries = payload["unused_policy_entries"]
    assert isinstance(unused_policy_entries, list)

    lines = [
        "planning-unchecked-checkboxes summary:",
        f"- root={payload['root']}",
        f"- policy_path={payload['policy_path']}",
        f"- scanned_file_count={summary['scanned_file_count']}",
        f"- unchecked_row_count={summary['unchecked_row_count']}",
        f"- allowed_count={summary['allowed_count']}",
        f"- disallowed_count={summary['disallowed_count']}",
        f"- unused_policy_entry_count={summary['unused_policy_entry_count']}",
    ]

    if disallowed_rows or (unused_policy_entries and not allow_unused_policy_entries):
        lines.append("planning-unchecked-checkboxes: FAIL")
    else:
        lines.append("planning-unchecked-checkboxes: OK")

    if disallowed_rows:
        lines.append("disallowed unchecked checkbox rows:")
        for row in disallowed_rows:
            assert isinstance(row, dict)
            lines.append(
                f"{row['path']}:{row['line']}: section='{row['section']}' text='{row['text']}'"
            )

    if unused_policy_entries:
        if allow_unused_policy_entries:
            lines.append("unused policy entries (ignored due to --allow-unused-policy-entries):")
        else:
            lines.append("unused policy entries:")
        for entry in unused_policy_entries:
            assert isinstance(entry, dict)
            lines.append(
                f"path='{entry['path']}' section='{entry['section']}' reason='{entry['reason']}'"
            )

    return "\n".join(lines) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_planning_unchecked_checkboxes.py",
        description=(
            "Fail when planning markdown contains unchecked checkbox rows that do "
            "not match explicit policy exceptions."
        ),
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=ROOT,
        help="Root directory used to resolve markdown globs and policy path.",
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=[],
        help=(
            "Glob (relative to --root) to scan for markdown files. Repeatable. "
            "Defaults to spec/planning/**/*.md."
        ),
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=[],
        help=(
            "Glob (relative to --root) to exclude markdown files from scan scope. "
            "Repeatable."
        ),
    )
    parser.add_argument(
        "--policy",
        type=Path,
        default=DEFAULT_POLICY_PATH,
        help=(
            "Policy JSON path. Relative paths are resolved from --root. "
            "Must contain allowed_unchecked_sections entries with path/section/reason."
        ),
    )
    parser.add_argument(
        "--format",
        choices=("human", "json"),
        default="human",
        help="Output format.",
    )
    parser.add_argument(
        "--allow-unused-policy-entries",
        action="store_true",
        help=(
            "Allow policy exceptions that match no unchecked rows. "
            "Intended only as a temporary migration bypass."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    root = resolve_root_path(args.root)
    globs = [normalize_glob(raw_glob) for raw_glob in (args.glob if args.glob else list(DEFAULT_GLOBS))]
    exclude_globs = [normalize_glob(raw_glob) for raw_glob in (args.exclude if args.exclude else list(DEFAULT_EXCLUDE_GLOBS))]
    policy_path = resolve_policy_path(root=root, raw_policy=args.policy)

    try:
        if not root.exists():
            raise ValueError(f"root directory does not exist: {root.as_posix()}")
        if not root.is_dir():
            raise ValueError(f"root path is not a directory: {root.as_posix()}")

        scanned_paths = iter_markdown_paths(root=root, globs=globs)
        if exclude_globs:
            excluded_paths: set[Path] = set()
            for exclude_glob in exclude_globs:
                for path in root.glob(exclude_glob):
                    if path.is_file():
                        excluded_paths.add(path.resolve())
            scanned_paths = [path for path in scanned_paths if path.resolve() not in excluded_paths]
        if not scanned_paths:
            raise ValueError("no markdown files matched the requested scope")

        policy_entries = load_policy(path=policy_path, root=root)
        unchecked_rows: list[UncheckedRow] = []
        for path in scanned_paths:
            unchecked_rows.extend(parse_unchecked_rows(path, root=root))
        evaluated_rows = evaluate_rows(unchecked_rows, policy_entries=policy_entries)
        unused_policy_entries = collect_unused_policy_entries(
            evaluated_rows=evaluated_rows,
            policy_entries=policy_entries,
        )
    except ValueError as exc:
        print(f"planning-unchecked-checkboxes: {exc}", file=sys.stderr)
        return 2

    payload = build_payload(
        root=root,
        policy_path=policy_path,
        scanned_paths=scanned_paths,
        evaluated_rows=evaluated_rows,
        unused_policy_entries=unused_policy_entries,
    )

    if args.format == "json":
        write_stdout(json.dumps(payload, indent=2) + "\n")
    else:
        write_stdout(render_human(payload, allow_unused_policy_entries=args.allow_unused_policy_entries))

    summary = payload["summary"]
    assert isinstance(summary, dict)
    disallowed_count = summary["disallowed_count"]
    assert isinstance(disallowed_count, int)
    unused_policy_entry_count = summary["unused_policy_entry_count"]
    assert isinstance(unused_policy_entry_count, int)
    if disallowed_count > 0:
        return 1
    if unused_policy_entry_count > 0 and not args.allow_unused_policy_entries:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
