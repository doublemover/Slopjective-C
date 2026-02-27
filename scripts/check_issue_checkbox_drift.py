#!/usr/bin/env python3
"""Check markdown issue-checkbox state against GitHub open/closed issue state."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_GLOBS = (
    "spec/planning/active/**/*.md",
    "tests/conformance/workpacks/*.md",
)
SCRIPTS_DIR = Path(__file__).resolve().parent

if str(SCRIPTS_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_DIR))

from lib.gh_client import GhClient


VERDICT_PASS = "PASS"
VERDICT_DRIFT = "DRIFT"
VERDICT_FAIL = "FAIL"
VERDICT_SORT_ORDER = (VERDICT_PASS, VERDICT_DRIFT, VERDICT_FAIL)
VERDICT_ORDER_INDEX = {value: index for index, value in enumerate(VERDICT_SORT_ORDER)}


@dataclass(frozen=True)
class CheckboxRef:
    path: Path
    line: int
    checked: bool
    issue_number: int


def issue_state_sets_live() -> tuple[set[int], set[int]]:
    client = GhClient(root=ROOT)
    return client.issue_numbers(state="open"), client.issue_numbers(state="closed")


def parse_issue_number(raw: Any, context: str) -> int:
    if isinstance(raw, bool):
        raise RuntimeError(f"{context} must be an integer issue number")
    if isinstance(raw, int):
        return raw
    if isinstance(raw, str) and raw.isdigit():
        return int(raw)
    raise RuntimeError(f"{context} must be an integer issue number")


def parse_issue_number_set(raw: Any, context: str) -> set[int]:
    if not isinstance(raw, list):
        raise RuntimeError(f"{context} must be a JSON array")

    numbers: set[int] = set()
    for idx, entry in enumerate(raw):
        entry_context = f"{context}[{idx}]"
        number_raw: Any
        if isinstance(entry, dict):
            if "number" not in entry:
                raise RuntimeError(f"{entry_context} object must include 'number'")
            number_raw = entry["number"]
        else:
            number_raw = entry
        numbers.add(parse_issue_number(number_raw, entry_context))

    return numbers


def issue_state_sets_from_issue_array(payload: list[Any], source: Path) -> tuple[set[int], set[int]]:
    open_set: set[int] = set()
    closed_set: set[int] = set()

    for idx, entry in enumerate(payload):
        if not isinstance(entry, dict):
            raise RuntimeError(
                f"issues snapshot {source} entry {idx} must be an object with 'number' and 'state'"
            )

        number = parse_issue_number(
            entry.get("number"),
            f"issues snapshot {source} entry {idx} field 'number'",
        )
        state_raw = entry.get("state")
        if not isinstance(state_raw, str):
            raise RuntimeError(
                f"issues snapshot {source} entry {idx} field 'state' must be 'open' or 'closed'"
            )
        state = state_raw.strip().lower()
        if state == "open":
            open_set.add(number)
        elif state == "closed":
            closed_set.add(number)
        else:
            raise RuntimeError(
                f"issues snapshot {source} entry {idx} field 'state' must be 'open' or 'closed'"
            )

    return open_set, closed_set


def issue_state_sets_from_snapshot(path: Path) -> tuple[set[int], set[int]]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise RuntimeError(f"issues snapshot does not exist: {path}") from exc
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid JSON in issues snapshot {path}: {exc}") from exc

    if isinstance(payload, dict):
        if "open" not in payload or "closed" not in payload:
            raise RuntimeError(
                f"issues snapshot {path} must include both 'open' and 'closed' arrays"
            )
        open_set = parse_issue_number_set(
            payload["open"],
            f"issues snapshot {path} field 'open'",
        )
        closed_set = parse_issue_number_set(
            payload["closed"],
            f"issues snapshot {path} field 'closed'",
        )
        return open_set, closed_set

    if isinstance(payload, list):
        return issue_state_sets_from_issue_array(payload, path)

    raise RuntimeError(
        f"issues snapshot {path} must be either an object with 'open'/'closed' arrays "
        "or an array of issue objects with 'number'/'state'"
    )


def resolve_issue_state_sets(
    *,
    issues_json: Path | None,
    live_gh: bool,
) -> tuple[set[int], set[int]]:
    if issues_json is not None and live_gh:
        raise RuntimeError("cannot combine --issues-json with --live-gh")

    if issues_json is not None:
        return issue_state_sets_from_snapshot(issues_json)

    return issue_state_sets_live()


def iter_markdown_paths(patterns: Iterable[str]) -> list[Path]:
    paths: list[Path] = []
    seen: set[Path] = set()
    for pattern in patterns:
        for path in ROOT.glob(pattern):
            resolved = path.resolve()
            if not path.is_file() or resolved in seen:
                continue
            seen.add(resolved)
            paths.append(path)
    return sorted(paths)


def parse_checkbox_refs(path: Path) -> list[CheckboxRef]:
    refs: list[CheckboxRef] = []
    for idx, line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        if "issue-drift: ignore" in line:
            continue
        if "Issue #" not in line:
            continue
        stripped = line.lstrip()
        if not (stripped.startswith("- [ ]") or stripped.startswith("- [x]") or stripped.startswith("- [X]")):
            continue

        checked = stripped.startswith("- [x]") or stripped.startswith("- [X]")
        marker = "Issue #"
        marker_index = stripped.find(marker)
        if marker_index == -1:
            continue
        tail = stripped[marker_index + len(marker) :]
        digits = ""
        for ch in tail:
            if ch.isdigit():
                digits += ch
            else:
                break
        if not digits:
            continue

        refs.append(
            CheckboxRef(
                path=path,
                line=idx,
                checked=checked,
                issue_number=int(digits),
            )
        )

    return refs


def check_drift(
    paths: list[Path],
    *,
    open_set: set[int],
    closed_set: set[int],
) -> tuple[list[str], int, int]:
    diagnostics: list[str] = []
    checked_row_count = 0
    overlap_count = 0

    for path in paths:
        refs = parse_checkbox_refs(path)
        checked_row_count += sum(1 for ref in refs if ref.checked)
        for ref in refs:
            issue = ref.issue_number
            in_open = issue in open_set
            in_closed = issue in closed_set
            rel = ref.path.relative_to(ROOT).as_posix()
            if in_open and in_closed:
                overlap_count += 1
                diagnostics.append(
                    f"{rel}:{ref.line}: issue #{issue} appears in both open and closed sets"
                )
                continue
            if not in_open and not in_closed:
                diagnostics.append(
                    f"{rel}:{ref.line}: issue #{issue} not found in open/closed sets"
                )
                continue
            expected_checked = in_closed
            if ref.checked != expected_checked:
                expected = "checked" if expected_checked else "unchecked"
                state = "closed" if in_closed else "open"
                diagnostics.append(
                    f"{rel}:{ref.line}: issue #{issue} is {state} but checkbox is not {expected}"
                )

    return diagnostics, checked_row_count, overlap_count


def classify_verdict(
    *,
    mismatch_count: int,
    drift_threshold: int,
    overlap_count: int,
    max_overlap_count: int,
) -> str:
    if overlap_count > max_overlap_count:
        return VERDICT_FAIL
    if mismatch_count == 0:
        return VERDICT_PASS
    if mismatch_count <= drift_threshold:
        return VERDICT_DRIFT
    return VERDICT_FAIL


def verdict_severity(verdict: str) -> int:
    return VERDICT_ORDER_INDEX.get(verdict, len(VERDICT_ORDER_INDEX))


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Detect drift between issue-linked markdown checkboxes and live "
            "GitHub issue open/closed state."
        )
    )
    parser.add_argument(
        "--glob",
        action="append",
        default=[],
        help=(
            "Glob path(s) relative to repository root to scan for issue-linked "
            "checkboxes. Can be passed multiple times. If omitted, built-in "
            "planning/workpack defaults are used."
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        help=(
            "Read issue state from a local JSON snapshot instead of live GitHub. "
            "Accepts either {'open': [...], 'closed': [...]} or "
            "an array of {'number': N, 'state': 'open'|'closed'}."
        ),
    )
    parser.add_argument(
        "--live-gh",
        action="store_true",
        help=(
            "Force live GitHub issue state lookup. "
            "Live mode is also used when --issues-json is omitted."
        ),
    )
    parser.add_argument(
        "--drift-threshold",
        type=int,
        default=0,
        help=(
            "Number of mismatches still reported as DRIFT instead of FAIL. "
            "Default 0 keeps fail-closed behavior."
        ),
    )
    parser.add_argument(
        "--max-overlap-count",
        type=int,
        default=0,
        help=(
            "Maximum number of open/closed overlap conflicts allowed before "
            "verdict is forced to FAIL. Default 0 keeps fail-closed overlap behavior."
        ),
    )
    parser.add_argument(
        "--max-verdict",
        choices=("pass", "drift", "fail"),
        default="pass",
        help=(
            "Exit non-zero when computed verdict exceeds this threshold. "
            "Default 'pass' keeps fail-closed behavior."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    globs = args.glob if args.glob else list(DEFAULT_GLOBS)
    paths = iter_markdown_paths(globs)
    if not paths:
        print("issue-drift: no matching markdown files")
        return 0

    try:
        if args.drift_threshold < 0:
            raise RuntimeError("--drift-threshold must be >= 0")
        if args.max_overlap_count < 0:
            raise RuntimeError("--max-overlap-count must be >= 0")
        open_set, closed_set = resolve_issue_state_sets(
            issues_json=args.issues_json,
            live_gh=args.live_gh,
        )
        diagnostics, checked_rows, overlap_count = check_drift(
            paths,
            open_set=open_set,
            closed_set=closed_set,
        )
    except RuntimeError as exc:
        print(f"issue-drift: {exc}", file=sys.stderr)
        return 2

    mismatch_count = len(diagnostics)
    max_verdict = args.max_verdict.strip().upper()
    verdict = classify_verdict(
        mismatch_count=mismatch_count,
        drift_threshold=args.drift_threshold,
        overlap_count=overlap_count,
        max_overlap_count=args.max_overlap_count,
    )
    if diagnostics:
        blocked = verdict_severity(verdict) > verdict_severity(max_verdict)
        stream = sys.stderr if blocked else sys.stdout
        for diag in diagnostics:
            print(diag, file=stream)
        print(
            (
                f"\nissue-drift: {verdict} "
                f"(found {mismatch_count} mismatch(es); "
                f"drift threshold: {args.drift_threshold}; "
                f"overlap count: {overlap_count}; "
                f"max overlap count: {args.max_overlap_count}; "
                f"max verdict: {max_verdict}; "
                f"threshold gate: {'blocked' if blocked else 'allowed'}; "
                f"checked row count: {checked_rows})."
            ),
            file=stream,
        )
        return 1 if blocked else 0

    print(
        "issue-drift: OK "
        f"(verdict: {verdict}; mismatch count: 0; "
        f"drift threshold: {args.drift_threshold}; "
        f"overlap count: {overlap_count}; max overlap count: {args.max_overlap_count}; "
        f"max verdict: {max_verdict}; checked row count: {checked_rows})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
