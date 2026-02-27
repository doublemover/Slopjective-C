#!/usr/bin/env python3
"""Deterministic placeholder lint for planning markdown documents."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SCAN_ROOT = ROOT / "spec" / "planning"
DEFAULT_ALLOWLIST_PATH = (
    ROOT / "spec" / "planning" / "planning_placeholder_allowlist.json"
)
ALLOWLIST_SCHEMA_VERSION = 1

PLACEHOLDER_PATTERNS: tuple[tuple[str, re.Pattern[str]], ...] = (
    ("<COMMIT_SHA>", re.compile(r"<COMMIT_SHA>")),
    ("<sha-*>", re.compile(r"<sha-[^>]+>")),
    ("<output>", re.compile(r"<output>")),
    ("<yes/no + note>", re.compile(r"<yes/no \+ note>")),
    ("<status>", re.compile(r"<status>")),
    ("<YYYY-MM-DD>", re.compile(r"<YYYY-MM-DD>")),
    ("<DATE>", re.compile(r"<DATE>")),
)
VALID_TOKENS = {token for token, _ in PLACEHOLDER_PATTERNS}


@dataclass(frozen=True)
class PlaceholderMatch:
    path: str
    line: int
    column: int
    token: str
    match: str


@dataclass(frozen=True)
class AllowlistEntry:
    path: str
    token: str
    reason: str


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def normalize_allowlist_path(raw_path: str, *, index: int) -> str:
    normalized = raw_path.strip().replace("\\", "/")
    if not normalized:
        raise ValueError(f"allowlist entry {index} field 'path' must be non-empty")
    if Path(normalized).is_absolute():
        raise ValueError(
            f"allowlist entry {index} field 'path' must be repository-relative"
        )
    return normalized


def load_allowlist(allowlist_path: Path) -> list[AllowlistEntry]:
    if not allowlist_path.exists():
        raise ValueError(
            f"allowlist JSON file does not exist: {display_path(allowlist_path)}"
        )

    try:
        payload = json.loads(allowlist_path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(
            f"unable to read allowlist JSON {display_path(allowlist_path)}: {exc}"
        ) from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"invalid JSON in allowlist file {display_path(allowlist_path)}: {exc}"
        ) from exc

    if not isinstance(payload, dict):
        raise ValueError("allowlist JSON root must be an object with an 'entries' array")

    raw_version = payload.get("version")
    if (
        isinstance(raw_version, bool)
        or not isinstance(raw_version, int)
        or raw_version != ALLOWLIST_SCHEMA_VERSION
    ):
        raise ValueError(
            "allowlist JSON field 'version' must be the integer "
            f"{ALLOWLIST_SCHEMA_VERSION}"
        )

    entries = payload.get("entries")
    if not isinstance(entries, list):
        raise ValueError("allowlist JSON field 'entries' must be an array")

    allowlist_entries: list[AllowlistEntry] = []
    seen: set[tuple[str, str]] = set()
    expected_keys = {"path", "token", "reason"}
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            raise ValueError(f"allowlist entry {index} must be an object")
        extra_keys = sorted(set(entry.keys()) - expected_keys)
        if extra_keys:
            extras = ", ".join(extra_keys)
            raise ValueError(
                f"allowlist entry {index} has unexpected field(s): {extras}"
            )

        raw_path = entry.get("path")
        token = entry.get("token")
        reason = entry.get("reason")
        if not isinstance(raw_path, str):
            raise ValueError(f"allowlist entry {index} field 'path' must be a string")
        if not isinstance(token, str):
            raise ValueError(f"allowlist entry {index} field 'token' must be a string")
        if token not in VALID_TOKENS:
            valid = ", ".join(sorted(VALID_TOKENS))
            raise ValueError(
                f"allowlist entry {index} field 'token' is invalid: {token!r}; "
                f"expected one of {valid}"
            )
        if not isinstance(reason, str) or not reason.strip():
            raise ValueError(
                f"allowlist entry {index} field 'reason' must be a non-empty string"
            )

        normalized_path = normalize_allowlist_path(raw_path, index=index)
        normalized_reason = reason.strip()
        key = (normalized_path, token)
        if key in seen:
            raise ValueError(
                "duplicate allowlist entry for "
                f"path='{normalized_path}' token='{token}'"
            )
        seen.add(key)
        allowlist_entries.append(
            AllowlistEntry(
                path=normalized_path,
                token=token,
                reason=normalized_reason,
            )
        )

    return allowlist_entries


def iter_markdown_files(scan_root: Path) -> list[Path]:
    if not scan_root.exists():
        raise ValueError(f"scan root does not exist: {display_path(scan_root)}")
    if not scan_root.is_dir():
        raise ValueError(f"scan root is not a directory: {display_path(scan_root)}")

    return sorted(
        (path for path in scan_root.rglob("*.md") if path.is_file()),
        key=lambda path: (display_path(path).casefold(), display_path(path)),
    )


def scan_placeholders(paths: Sequence[Path]) -> list[PlaceholderMatch]:
    matches: list[PlaceholderMatch] = []
    for path in paths:
        source_path = display_path(path)
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except OSError as exc:
            raise ValueError(f"unable to read markdown file {source_path}: {exc}") from exc

        for line_no, line in enumerate(lines, start=1):
            for token, pattern in PLACEHOLDER_PATTERNS:
                for raw_match in pattern.finditer(line):
                    matches.append(
                        PlaceholderMatch(
                            path=source_path,
                            line=line_no,
                            column=raw_match.start() + 1,
                            token=token,
                            match=raw_match.group(0),
                        )
                    )

    return sorted(
        matches,
        key=lambda row: (
            row.path.casefold(),
            row.path,
            row.line,
            row.column,
            row.token,
            row.match,
        ),
    )


def find_unused_allowlist_entries(
    *,
    allowlist_entries: Sequence[AllowlistEntry],
    matches: Sequence[PlaceholderMatch],
) -> list[AllowlistEntry]:
    used_keys = {(row.path, row.token) for row in matches}
    unused = [
        entry
        for entry in allowlist_entries
        if (entry.path, entry.token) not in used_keys
    ]
    return sorted(
        unused,
        key=lambda entry: (
            entry.path.casefold(),
            entry.path,
            entry.token,
            entry.reason,
        ),
    )


def build_payload(
    *,
    scan_root: Path,
    allowlist_path: Path,
    scanned_file_count: int,
    matches: Sequence[PlaceholderMatch],
    unresolved: Sequence[PlaceholderMatch],
    unused_allowlist_entries: Sequence[AllowlistEntry],
) -> dict[str, object]:
    return {
        "scan_root": display_path(scan_root),
        "allowlist_path": display_path(allowlist_path),
        "scanned_file_count": scanned_file_count,
        "placeholder_count": len(matches),
        "allowlisted_count": len(matches) - len(unresolved),
        "unresolved_count": len(unresolved),
        "unused_allowlist_entry_count": len(unused_allowlist_entries),
        "unused_allowlist_entries": [
            {
                "path": entry.path,
                "token": entry.token,
                "reason": entry.reason,
            }
            for entry in unused_allowlist_entries
        ],
        "unresolved": [
            {
                "path": row.path,
                "line": row.line,
                "column": row.column,
                "token": row.token,
                "match": row.match,
            }
            for row in unresolved
        ],
    }


def render_json(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def render_human(
    *,
    payload: dict[str, object],
    unresolved: Sequence[PlaceholderMatch],
    fail_on_unused_allowlist_entries: bool,
) -> str:
    unresolved_count = int(payload["unresolved_count"])
    unused_allowlist_entry_count = int(payload["unused_allowlist_entry_count"])
    fails_for_unused_allowlist_entries = (
        unused_allowlist_entry_count > 0 and fail_on_unused_allowlist_entries
    )
    if unresolved_count == 0 and not fails_for_unused_allowlist_entries:
        return (
            "planning-placeholder-lint: OK "
            f"(scanned_files={payload['scanned_file_count']}, "
            f"placeholders={payload['placeholder_count']}, "
            f"allowlisted={payload['allowlisted_count']}, "
            f"unused_allowlist_entries={payload['unused_allowlist_entry_count']})\n"
        )

    lines: list[str] = []
    has_unresolved = unresolved_count > 0
    has_unused_allowlist_entries = unused_allowlist_entry_count > 0
    for row in unresolved:
        if row.token == row.match:
            detail = f"{row.path}:{row.line}:{row.column}: unresolved placeholder {row.token}"
        else:
            detail = (
                f"{row.path}:{row.line}:{row.column}: unresolved placeholder {row.token} "
                f"(matched {row.match})"
            )
        lines.append(detail)

    raw_unused_entries = payload.get("unused_allowlist_entries", [])
    if isinstance(raw_unused_entries, list) and raw_unused_entries:
        if has_unresolved:
            lines.append("")
        for raw_entry in raw_unused_entries:
            if not isinstance(raw_entry, dict):
                continue
            path = raw_entry.get("path")
            token = raw_entry.get("token")
            if isinstance(path, str) and isinstance(token, str):
                lines.append(
                    f"allowlist: unused entry path='{path}' token='{token}'"
                )

    lines.append("")
    if has_unresolved:
        lines.append(
            f"Found {unresolved_count} unresolved placeholder(s) outside allowlist."
        )
    if has_unused_allowlist_entries:
        entry_label = (
            "entry" if unused_allowlist_entry_count == 1 else "entries"
        )
        if fail_on_unused_allowlist_entries:
            lines.append(
                "Found "
                f"{unused_allowlist_entry_count} unused allowlist {entry_label}."
            )
        else:
            lines.append(
                "Found "
                f"{unused_allowlist_entry_count} unused allowlist {entry_label} "
                "(ignored by --allow-unused-allowlist-entries)."
            )
    lines.append(
        "Summary: "
        f"scanned_files={payload['scanned_file_count']} "
        f"placeholders={payload['placeholder_count']} "
        f"allowlisted={payload['allowlisted_count']} "
        f"unresolved={payload['unresolved_count']} "
        f"unused_allowlist_entries={payload['unused_allowlist_entry_count']}"
    )
    return "\n".join(lines) + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_planning_placeholders.py",
        description=(
            "Scan spec/planning markdown files for unresolved placeholder tokens and "
            "enforce path+token allowlist exceptions."
        ),
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=DEFAULT_SCAN_ROOT,
        help=(
            "Root directory to scan recursively for markdown files "
            f"(default: {display_path(DEFAULT_SCAN_ROOT)})."
        ),
    )
    parser.add_argument(
        "--allowlist",
        type=Path,
        default=DEFAULT_ALLOWLIST_PATH,
        help=(
            "Path to JSON allowlist with entries[path, token, reason] "
            f"(default: {display_path(DEFAULT_ALLOWLIST_PATH)})."
        ),
    )
    parser.add_argument(
        "--format",
        choices=("human", "json"),
        default="human",
        help="Output format: human (default) or json.",
    )
    parser.add_argument(
        "--allow-unused-allowlist-entries",
        action="store_true",
        help=(
            "Temporarily suppress failures caused by unused allowlist entries "
            "during migrations."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    scan_root = resolve_input_path(args.root)
    allowlist_path = resolve_input_path(args.allowlist)

    try:
        allowlist_entries = load_allowlist(allowlist_path)
        allowlist = {(entry.path, entry.token) for entry in allowlist_entries}
        markdown_files = iter_markdown_files(scan_root)
        matches = scan_placeholders(markdown_files)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    unresolved = [
        row for row in matches if (row.path, row.token) not in allowlist
    ]
    unused_allowlist_entries = find_unused_allowlist_entries(
        allowlist_entries=allowlist_entries,
        matches=matches,
    )
    payload = build_payload(
        scan_root=scan_root,
        allowlist_path=allowlist_path,
        scanned_file_count=len(markdown_files),
        matches=matches,
        unresolved=unresolved,
        unused_allowlist_entries=unused_allowlist_entries,
    )

    if args.format == "json":
        write_stdout(render_json(payload))
    else:
        write_stdout(
            render_human(
                payload=payload,
                unresolved=unresolved,
                fail_on_unused_allowlist_entries=(
                    not args.allow_unused_allowlist_entries
                ),
            )
        )

    unresolved_count = len(unresolved)
    stale_allowlist_count = (
        len(unused_allowlist_entries)
        if not args.allow_unused_allowlist_entries
        else 0
    )
    return 1 if unresolved_count > 0 or stale_allowlist_count > 0 else 0


if __name__ == "__main__":
    raise SystemExit(main())
