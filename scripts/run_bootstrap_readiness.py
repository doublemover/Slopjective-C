#!/usr/bin/env python3
"""Run deterministic bootstrap readiness orchestration and persist evidence artifacts."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import textwrap
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG_JSON = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT / "spec" / "planning"
DEFAULT_OUTPUT_DIR = ROOT / "reports" / "bootstrap_readiness"
DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"

CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH = ROOT / "scripts" / "check_bootstrap_readiness.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"

BOOTSTRAP_JSON_FILENAME = "check_bootstrap_readiness.json"
BOOTSTRAP_MD_FILENAME = "check_bootstrap_readiness.md"
BOOTSTRAP_JSON_LOG_FILENAME = "check_bootstrap_readiness_json.log"
BOOTSTRAP_MD_LOG_FILENAME = "check_bootstrap_readiness_markdown.log"
OPEN_BLOCKERS_REFRESH_LOG_FILENAME = "extract_open_blockers.log"
SPEC_LINT_LOG_FILENAME = "spec_lint.log"
SUMMARY_JSON_FILENAME = "bootstrap_readiness_summary.json"
REPORT_MD_FILENAME = "bootstrap_readiness_report.md"

EXIT_BOOTSTRAPPABLE = 0
EXIT_BLOCKED = 1
EXIT_RUNNER_ERROR = 2


@dataclass(frozen=True)
class CommandSpec:
    name: str
    script_path: Path
    actual_args: tuple[str, ...]
    display_args: tuple[str, ...]


@dataclass(frozen=True)
class CommandResult:
    spec: CommandSpec
    exit_code: int
    stdout: str
    stderr: str


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


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH


def write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(normalize_newlines(content), encoding="utf-8", newline="\n")


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def run_command(spec: CommandSpec) -> CommandResult:
    command = [sys.executable, str(spec.script_path), *spec.actual_args]
    proc = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    return CommandResult(
        spec=spec,
        exit_code=int(proc.returncode),
        stdout=normalize_newlines(proc.stdout),
        stderr=normalize_newlines(proc.stderr),
    )


def render_command_log(title: str, result: CommandResult) -> str:
    lines = [
        f"# {title}",
        "",
        "## stdout",
        "",
        result.stdout.rstrip("\n") if result.stdout else "_empty_",
        "",
        "## stderr",
        "",
        result.stderr.rstrip("\n") if result.stderr else "_empty_",
        "",
    ]
    return "\n".join(lines)


def summarize_command(result: CommandResult) -> dict[str, Any]:
    return {
        "argv": [
            "python",
            display_path(result.spec.script_path),
            *list(result.spec.display_args),
        ],
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }


def parse_checker_payload(result: CommandResult) -> tuple[dict[str, Any] | None, str | None]:
    if result.exit_code not in (EXIT_BOOTSTRAPPABLE, EXIT_BLOCKED):
        return None, (
            "check_bootstrap_readiness(json) returned unexpected exit code "
            f"{result.exit_code}."
        )

    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        return (
            None,
            (
                "check_bootstrap_readiness(json) emitted invalid JSON: "
                f"{exc.msg} at {exc.lineno}:{exc.colno}."
            ),
        )

    if not isinstance(payload, dict):
        return None, "check_bootstrap_readiness(json) output root must be an object."

    def parse_count(field_name: str) -> tuple[int | None, str | None]:
        raw = payload.get(field_name)
        if isinstance(raw, bool) or not isinstance(raw, int) or raw < 0:
            return None, (
                "check_bootstrap_readiness(json) missing non-negative integer "
                f"{field_name!r}."
            )
        return raw, None

    issues_open_count, issues_error = parse_count("issues_open_count")
    if issues_error is not None:
        return None, issues_error
    milestones_open_count, milestones_error = parse_count("milestones_open_count")
    if milestones_error is not None:
        return None, milestones_error
    catalog_open_task_count, catalog_error = parse_count("catalog_open_task_count")
    if catalog_error is not None:
        return None, catalog_error
    blockers_open_count, blockers_error = parse_count("blockers_open_count")
    if blockers_error is not None:
        return None, blockers_error

    readiness_state = payload.get("readiness_state")
    if readiness_state not in ("bootstrappable", "blocked"):
        return None, (
            "check_bootstrap_readiness(json) missing deterministic 'readiness_state' "
            "in {'bootstrappable','blocked'}."
        )

    intake_recommendation = payload.get("intake_recommendation")
    if intake_recommendation not in ("go", "hold"):
        return None, (
            "check_bootstrap_readiness(json) missing deterministic "
            "'intake_recommendation' in {'go','hold'}."
        )

    blocking_dimensions = payload.get("blocking_dimensions")
    if not isinstance(blocking_dimensions, list) or not all(
        isinstance(item, str) and item for item in blocking_dimensions
    ):
        return None, (
            "check_bootstrap_readiness(json) missing non-empty string list "
            "'blocking_dimensions'."
        )
    if len(set(blocking_dimensions)) != len(blocking_dimensions):
        return None, (
            "check_bootstrap_readiness(json) has duplicate entries in "
            "'blocking_dimensions'."
        )

    expected_blocking_dimensions = [
        field_name
        for field_name, count in (
            ("issues_open_count", issues_open_count),
            ("milestones_open_count", milestones_open_count),
            ("catalog_open_task_count", catalog_open_task_count),
            ("blockers_open_count", blockers_open_count),
        )
        if count > 0
    ]
    if blocking_dimensions != expected_blocking_dimensions:
        return None, (
            "check_bootstrap_readiness(json) blocking_dimensions drift: "
            f"blocking_dimensions={blocking_dimensions!r} "
            f"expected={expected_blocking_dimensions!r}."
        )

    expected_readiness_state = (
        "bootstrappable" if not expected_blocking_dimensions else "blocked"
    )
    if readiness_state != expected_readiness_state:
        return None, (
            "check_bootstrap_readiness(json) readiness reduction mismatch: "
            f"readiness_state={readiness_state!r} expected={expected_readiness_state!r}."
        )

    expected_intake_recommendation = "go" if readiness_state == "bootstrappable" else "hold"
    if intake_recommendation != expected_intake_recommendation:
        return None, (
            "check_bootstrap_readiness(json) recommendation drift: "
            f"intake_recommendation={intake_recommendation!r} "
            f"expected={expected_intake_recommendation!r}."
        )

    expected_exit = (
        EXIT_BOOTSTRAPPABLE
        if readiness_state == "bootstrappable"
        else EXIT_BLOCKED
    )
    if result.exit_code != expected_exit:
        return None, (
            "check_bootstrap_readiness(json) readiness/exit mismatch: "
            f"readiness_state={readiness_state!r} exit={result.exit_code}."
        )

    return payload, None


def check_markdown_consistency(
    markdown_result: CommandResult,
    *,
    checker_payload: dict[str, Any],
) -> str | None:
    if markdown_result.exit_code not in (EXIT_BOOTSTRAPPABLE, EXIT_BLOCKED):
        return (
            "check_bootstrap_readiness(markdown) returned unexpected exit code "
            f"{markdown_result.exit_code}."
        )

    expected_exit = (
        EXIT_BOOTSTRAPPABLE
        if checker_payload["readiness_state"] == "bootstrappable"
        else EXIT_BLOCKED
    )
    if markdown_result.exit_code != expected_exit:
        return (
            "check_bootstrap_readiness(markdown) readiness/exit mismatch: "
            f"readiness_state={checker_payload['readiness_state']!r} "
            f"exit={markdown_result.exit_code}."
        )

    expected_lines = (
        "# Bootstrap Readiness",
        "| Metric | Value |",
        "| --- | --- |",
        f"| issues_open_count | `{checker_payload['issues_open_count']}` |",
        f"| milestones_open_count | `{checker_payload['milestones_open_count']}` |",
        f"| catalog_open_task_count | `{checker_payload['catalog_open_task_count']}` |",
        f"| blockers_open_count | `{checker_payload['blockers_open_count']}` |",
        f"| readiness_state | `{checker_payload['readiness_state']}` |",
        f"| intake_recommendation | `{checker_payload['intake_recommendation']}` |",
    )
    for expected_line in expected_lines:
        if expected_line in markdown_result.stdout:
            continue
        return (
            "check_bootstrap_readiness(markdown) missing deterministic line "
            f"{expected_line!r}."
        )
    return None


def determine_final_exit(
    *,
    checker_payload: dict[str, Any] | None,
    errors: Sequence[str],
) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"
    if checker_payload is None:
        return EXIT_RUNNER_ERROR, "runner-error"

    readiness_state = checker_payload.get("readiness_state")
    if readiness_state == "blocked":
        return EXIT_BLOCKED, "blocked-readiness"
    if readiness_state == "bootstrappable":
        return EXIT_BOOTSTRAPPABLE, "ok"
    return EXIT_RUNNER_ERROR, "runner-error"


def build_summary_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    output_dir: Path,
    refresh_open_blockers_requested: bool,
    refresh_open_blockers_result: CommandResult | None,
    refresh_open_blockers_root: Path | None,
    refresh_open_blockers_generated_at_utc: str | None,
    refresh_open_blockers_source: str | None,
    checker_json_result: CommandResult,
    checker_markdown_result: CommandResult,
    checker_payload: dict[str, Any] | None,
    run_spec_lint_requested: bool,
    spec_globs: Sequence[str],
    spec_lint_result: CommandResult | None,
    errors: Sequence[str],
    final_status: str,
    final_exit_code: int,
) -> dict[str, Any]:
    readiness_state = None
    intake_recommendation = None
    blocking_dimensions: list[str] = []
    issues_open_count = None
    milestones_open_count = None
    catalog_open_task_count = None
    blockers_open_count = None

    if checker_payload is not None:
        readiness_state = checker_payload.get("readiness_state")
        intake_recommendation = checker_payload.get("intake_recommendation")
        raw_blocking_dimensions = checker_payload.get("blocking_dimensions", [])
        if isinstance(raw_blocking_dimensions, list):
            blocking_dimensions = [str(item) for item in raw_blocking_dimensions]
        raw_issues = checker_payload.get("issues_open_count")
        if isinstance(raw_issues, int) and not isinstance(raw_issues, bool):
            issues_open_count = raw_issues
        raw_milestones = checker_payload.get("milestones_open_count")
        if isinstance(raw_milestones, int) and not isinstance(raw_milestones, bool):
            milestones_open_count = raw_milestones
        raw_catalog = checker_payload.get("catalog_open_task_count")
        if isinstance(raw_catalog, int) and not isinstance(raw_catalog, bool):
            catalog_open_task_count = raw_catalog
        raw_blockers = checker_payload.get("blockers_open_count")
        if isinstance(raw_blockers, int) and not isinstance(raw_blockers, bool):
            blockers_open_count = raw_blockers

    commands: dict[str, Any] = {}
    if refresh_open_blockers_result is not None:
        commands["extract_open_blockers_snapshot_json"] = summarize_command(
            refresh_open_blockers_result
        )
    commands["check_bootstrap_readiness_json"] = summarize_command(checker_json_result)
    commands["check_bootstrap_readiness_markdown"] = summarize_command(
        checker_markdown_result
    )
    if spec_lint_result is not None:
        commands["spec_lint"] = summarize_command(spec_lint_result)

    return {
        "runner": "bootstrap-readiness-runner/v0.1",
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
            "open_blockers_refresh": refresh_open_blockers_requested,
            "open_blockers_root": (
                display_path(refresh_open_blockers_root)
                if refresh_open_blockers_root is not None
                else None
            ),
            "open_blockers_generated_at_utc": refresh_open_blockers_generated_at_utc,
            "open_blockers_source": refresh_open_blockers_source,
            "run_spec_lint": run_spec_lint_requested,
            "spec_globs": list(spec_globs),
        },
        "artifacts": {
            "output_dir": display_path(output_dir),
            "checker_json": BOOTSTRAP_JSON_FILENAME,
            "checker_markdown": BOOTSTRAP_MD_FILENAME,
            "checker_json_log": BOOTSTRAP_JSON_LOG_FILENAME,
            "checker_markdown_log": BOOTSTRAP_MD_LOG_FILENAME,
            "open_blockers_refresh_log": (
                OPEN_BLOCKERS_REFRESH_LOG_FILENAME
                if refresh_open_blockers_result is not None
                else None
            ),
            "spec_lint_log": (
                SPEC_LINT_LOG_FILENAME if spec_lint_result is not None else None
            ),
            "summary_json": SUMMARY_JSON_FILENAME,
            "report_markdown": REPORT_MD_FILENAME,
        },
        "open_blockers_refresh": {
            "requested": refresh_open_blockers_requested,
            "attempted": refresh_open_blockers_result is not None,
            "exit_code": (
                refresh_open_blockers_result.exit_code
                if refresh_open_blockers_result is not None
                else None
            ),
            "root": (
                display_path(refresh_open_blockers_root)
                if refresh_open_blockers_root is not None
                else None
            ),
            "generated_at_utc": refresh_open_blockers_generated_at_utc,
            "source": refresh_open_blockers_source,
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
        },
        "readiness": {
            "readiness_state": readiness_state,
            "intake_recommendation": intake_recommendation,
            "issues_open_count": issues_open_count,
            "milestones_open_count": milestones_open_count,
            "catalog_open_task_count": catalog_open_task_count,
            "blockers_open_count": blockers_open_count,
            "blocking_dimensions": blocking_dimensions,
            "checker_exit_code": checker_json_result.exit_code,
        },
        "spec_lint": {
            "requested": run_spec_lint_requested,
            "attempted": spec_lint_result is not None,
            "exit_code": spec_lint_result.exit_code if spec_lint_result is not None else None,
            "ok": (
                spec_lint_result.exit_code == 0 if spec_lint_result is not None else None
            ),
        },
        "commands": commands,
        "errors": list(errors),
        "final_status": final_status,
        "final_exit_code": final_exit_code,
    }


def render_markdown_report(summary: dict[str, Any]) -> str:
    inputs = summary["inputs"]
    artifacts = summary["artifacts"]
    refresh = summary["open_blockers_refresh"]
    readiness = summary["readiness"]
    spec_lint = summary["spec_lint"]
    commands = summary["commands"]
    errors = summary["errors"]

    assert isinstance(inputs, dict)
    assert isinstance(artifacts, dict)
    assert isinstance(refresh, dict)
    assert isinstance(readiness, dict)
    assert isinstance(spec_lint, dict)
    assert isinstance(commands, dict)
    assert isinstance(errors, list)

    def optional_literal(value: Any) -> str:
        if value is None:
            return "_none_"
        return f"`{value}`"

    blocking_dimensions = readiness.get("blocking_dimensions")
    blocking_dimensions_text = "_none_"
    if isinstance(blocking_dimensions, list) and blocking_dimensions:
        blocking_dimensions_text = ", ".join(f"`{item}`" for item in blocking_dimensions)

    lines = [
        "# Bootstrap Readiness Orchestration",
        "",
        "## Inputs",
        "",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        "- Open blockers JSON: " + optional_literal(inputs["open_blockers_json"]),
        "- Open blockers refresh requested: "
        f"`{bool_text(bool(inputs['open_blockers_refresh']))}`",
        "- Open blockers refresh root: " + optional_literal(inputs["open_blockers_root"]),
        "- Open blockers generated_at_utc: "
        + optional_literal(inputs["open_blockers_generated_at_utc"]),
        "- Open blockers source: " + optional_literal(inputs["open_blockers_source"]),
        f"- Run spec_lint: `{bool_text(bool(inputs['run_spec_lint']))}`",
        (
            "- spec_lint globs: "
            + (
                ", ".join(f"`{glob}`" for glob in inputs["spec_globs"])
                if inputs["spec_globs"]
                else "_default spec_lint globs_"
            )
        ),
        "",
        "## Open Blockers Refresh",
        "",
        f"- Requested: `{bool_text(bool(refresh['requested']))}`",
        f"- Attempted: `{bool_text(bool(refresh['attempted']))}`",
        "- Exit code: " + optional_literal(refresh["exit_code"]),
        "- Root: " + optional_literal(refresh["root"]),
        "- generated_at_utc: " + optional_literal(refresh["generated_at_utc"]),
        "- source: " + optional_literal(refresh["source"]),
        "- Open blockers JSON path: " + optional_literal(refresh["open_blockers_json"]),
        "",
        "## Readiness",
        "",
        "- Readiness state: " + optional_literal(readiness["readiness_state"]),
        "- Intake recommendation: " + optional_literal(readiness["intake_recommendation"]),
        "- issues_open_count: " + optional_literal(readiness["issues_open_count"]),
        "- milestones_open_count: " + optional_literal(readiness["milestones_open_count"]),
        "- catalog_open_task_count: " + optional_literal(readiness["catalog_open_task_count"]),
        "- blockers_open_count: " + optional_literal(readiness["blockers_open_count"]),
        "- blocking_dimensions: " + blocking_dimensions_text,
        "- checker exit code: " + optional_literal(readiness["checker_exit_code"]),
        "",
        "## Spec Lint",
        "",
        f"- Requested: `{bool_text(bool(spec_lint['requested']))}`",
        f"- Attempted: `{bool_text(bool(spec_lint['attempted']))}`",
        "- Exit code: " + optional_literal(spec_lint["exit_code"]),
        "- OK: " + optional_literal(spec_lint["ok"]),
        "",
        "## Final Outcome",
        "",
        f"- Final status: `{summary['final_status']}`",
        f"- Final exit code: `{summary['final_exit_code']}`",
        "",
        "## Artifacts",
        "",
        f"- Output directory: `{artifacts['output_dir']}`",
        f"- Checker JSON: `{artifacts['checker_json']}`",
        f"- Checker markdown: `{artifacts['checker_markdown']}`",
        f"- Checker JSON log: `{artifacts['checker_json_log']}`",
        f"- Checker markdown log: `{artifacts['checker_markdown_log']}`",
        "- Open blockers refresh log: " + optional_literal(artifacts["open_blockers_refresh_log"]),
        "- spec_lint log: " + optional_literal(artifacts["spec_lint_log"]),
        f"- Summary JSON: `{artifacts['summary_json']}`",
        f"- Report markdown: `{artifacts['report_markdown']}`",
        "",
        "## Command Exit Codes",
        "",
        "| Command | Exit Code |",
        "| --- | --- |",
    ]

    for key, payload in commands.items():
        assert isinstance(payload, dict)
        lines.append(f"| `{key}` | `{payload['exit_code']}` |")

    lines.extend(["", "## Errors", ""])
    if errors:
        for entry in errors:
            lines.append(f"- {entry}")
    else:
        lines.append("- _none_")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_bootstrap_readiness.py",
        description=(
            "Run deterministic bootstrap readiness orchestration "
            "(check_bootstrap_readiness json+markdown with optional refresh/spec_lint) "
            "and persist evidence artifacts."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: bootstrappable readiness and no runner/contract errors.
              1: blocked readiness and no runner/contract errors.
              2: runner error, command contract mismatch, or optional command failure.

            Output artifact files (under --output-dir):
              - {BOOTSTRAP_JSON_FILENAME}
              - {BOOTSTRAP_MD_FILENAME}
              - {BOOTSTRAP_JSON_LOG_FILENAME}
              - {BOOTSTRAP_MD_LOG_FILENAME}
              - {OPEN_BLOCKERS_REFRESH_LOG_FILENAME} (when --refresh-open-blockers is used)
              - {SPEC_LINT_LOG_FILENAME} (when --run-spec-lint is used)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to open-issues snapshot JSON passed to check_bootstrap_readiness.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to open-milestones snapshot JSON passed to check_bootstrap_readiness.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON passed to check_bootstrap_readiness. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional open blockers JSON passed to check_bootstrap_readiness. When "
            "--refresh-open-blockers is set and this path is omitted, a refreshed snapshot "
            "is written under --output-dir/inputs/open_blockers.snapshot.json."
        ),
    )
    parser.add_argument(
        "--refresh-open-blockers",
        action="store_true",
        help=(
            "Refresh open blockers before readiness checks by invoking "
            "scripts/extract_open_blockers.py with --format snapshot-json."
        ),
    )
    parser.add_argument(
        "--open-blockers-root",
        type=Path,
        default=DEFAULT_OPEN_BLOCKERS_ROOT,
        help=(
            "Root directory scanned when --refresh-open-blockers is used. "
            f"Default: {display_path(DEFAULT_OPEN_BLOCKERS_ROOT)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-generated-at-utc",
        help=(
            "generated_at_utc metadata forwarded to extract_open_blockers snapshot-json "
            "(required when --refresh-open-blockers is used)."
        ),
    )
    parser.add_argument(
        "--open-blockers-source",
        help=(
            "source metadata forwarded to extract_open_blockers snapshot-json "
            "(required when --refresh-open-blockers is used)."
        ),
    )
    parser.add_argument(
        "--run-spec-lint",
        action="store_true",
        help="Run scripts/spec_lint.py after readiness checks.",
    )
    parser.add_argument(
        "--spec-glob",
        action="append",
        default=[],
        dest="spec_globs",
        help=(
            "Repeatable glob passed to spec_lint via --glob when --run-spec-lint is set. "
            "When omitted, spec_lint defaults are used."
        ),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=(
            "Directory where deterministic artifacts are written. "
            f"Default: {display_path(DEFAULT_OUTPUT_DIR)}."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    issues_path = resolve_repo_path(args.issues_json)
    milestones_path = resolve_repo_path(args.milestones_json)
    catalog_path = resolve_repo_path(args.catalog_json)
    output_dir = resolve_repo_path(args.output_dir)
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    refresh_open_blockers_root = (
        resolve_repo_path(args.open_blockers_root)
        if args.refresh_open_blockers
        else None
    )

    errors: list[str] = []
    refresh_open_blockers_result: CommandResult | None = None
    spec_lint_result: CommandResult | None = None

    if args.refresh_open_blockers:
        if open_blockers_path is None:
            open_blockers_path = default_open_blockers_output_path(output_dir)

        if args.open_blockers_generated_at_utc is None:
            errors.append(
                "--open-blockers-generated-at-utc is required when --refresh-open-blockers is set."
            )
        if args.open_blockers_source is None:
            errors.append(
                "--open-blockers-source is required when --refresh-open-blockers is set."
            )

        if (
            refresh_open_blockers_root is not None
            and args.open_blockers_generated_at_utc is not None
            and args.open_blockers_source is not None
        ):
            refresh_actual_args = [
                "--root",
                str(refresh_open_blockers_root),
                "--format",
                "snapshot-json",
                "--generated-at-utc",
                args.open_blockers_generated_at_utc,
                "--source",
                args.open_blockers_source,
            ]
            refresh_display_args = [
                "--root",
                display_path(refresh_open_blockers_root),
                "--format",
                "snapshot-json",
                "--generated-at-utc",
                args.open_blockers_generated_at_utc,
                "--source",
                args.open_blockers_source,
            ]
            refresh_spec = CommandSpec(
                name="extract_open_blockers_snapshot_json",
                script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
                actual_args=tuple(refresh_actual_args),
                display_args=tuple(refresh_display_args),
            )
            refresh_open_blockers_result = run_command(refresh_spec)
            if refresh_open_blockers_result.exit_code != 0:
                errors.append(
                    "extract_open_blockers(snapshot-json) returned unexpected exit code "
                    f"{refresh_open_blockers_result.exit_code}."
                )
            else:
                assert open_blockers_path is not None
                try:
                    write_text(open_blockers_path, refresh_open_blockers_result.stdout)
                except OSError as exc:
                    errors.append(
                        "unable to persist refreshed open blockers snapshot to "
                        f"{display_path(open_blockers_path)}: {exc}."
                    )

    checker_actual_args = [
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(catalog_path),
    ]
    checker_display_args = [
        "--issues-json",
        display_path(issues_path),
        "--milestones-json",
        display_path(milestones_path),
        "--catalog-json",
        display_path(catalog_path),
    ]
    if open_blockers_path is not None:
        checker_actual_args.extend(["--open-blockers-json", str(open_blockers_path)])
        checker_display_args.extend(["--open-blockers-json", display_path(open_blockers_path)])

    checker_json_spec = CommandSpec(
        name="check_bootstrap_readiness_json",
        script_path=CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
        actual_args=tuple([*checker_actual_args, "--format", "json"]),
        display_args=tuple([*checker_display_args, "--format", "json"]),
    )
    checker_markdown_spec = CommandSpec(
        name="check_bootstrap_readiness_markdown",
        script_path=CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH,
        actual_args=tuple([*checker_actual_args, "--format", "md"]),
        display_args=tuple([*checker_display_args, "--format", "md"]),
    )

    checker_json_result = run_command(checker_json_spec)
    checker_markdown_result = run_command(checker_markdown_spec)

    checker_payload, checker_error = parse_checker_payload(checker_json_result)
    if checker_error is not None:
        errors.append(checker_error)

    if checker_payload is not None:
        markdown_error = check_markdown_consistency(
            checker_markdown_result,
            checker_payload=checker_payload,
        )
        if markdown_error is not None:
            errors.append(markdown_error)

    if args.run_spec_lint:
        spec_lint_actual_args: list[str] = []
        spec_lint_display_args: list[str] = []
        for glob in args.spec_globs:
            spec_lint_actual_args.extend(["--glob", glob])
            spec_lint_display_args.extend(["--glob", glob])

        spec_lint_spec = CommandSpec(
            name="spec_lint",
            script_path=SPEC_LINT_SCRIPT_PATH,
            actual_args=tuple(spec_lint_actual_args),
            display_args=tuple(spec_lint_display_args),
        )
        spec_lint_result = run_command(spec_lint_spec)
        if spec_lint_result.exit_code != 0:
            errors.append(
                f"spec_lint returned unexpected exit code {spec_lint_result.exit_code}."
            )

    final_exit_code, final_status = determine_final_exit(
        checker_payload=checker_payload,
        errors=errors,
    )

    try:
        output_dir.mkdir(parents=True, exist_ok=True)

        checker_json_path = output_dir / BOOTSTRAP_JSON_FILENAME
        checker_md_path = output_dir / BOOTSTRAP_MD_FILENAME
        checker_json_log_path = output_dir / BOOTSTRAP_JSON_LOG_FILENAME
        checker_md_log_path = output_dir / BOOTSTRAP_MD_LOG_FILENAME
        refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
        spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
        summary_json_path = output_dir / SUMMARY_JSON_FILENAME
        report_md_path = output_dir / REPORT_MD_FILENAME

        write_text(checker_json_path, checker_json_result.stdout)
        write_text(checker_md_path, checker_markdown_result.stdout)
        write_text(
            checker_json_log_path,
            render_command_log(
                "check_bootstrap_readiness json command output",
                checker_json_result,
            ),
        )
        write_text(
            checker_md_log_path,
            render_command_log(
                "check_bootstrap_readiness markdown command output",
                checker_markdown_result,
            ),
        )
        if refresh_open_blockers_result is not None:
            write_text(
                refresh_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    refresh_open_blockers_result,
                ),
            )
        if spec_lint_result is not None:
            write_text(
                spec_lint_log_path,
                render_command_log("spec_lint command output", spec_lint_result),
            )

        summary = build_summary_payload(
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_path=open_blockers_path,
            output_dir=output_dir,
            refresh_open_blockers_requested=bool(args.refresh_open_blockers),
            refresh_open_blockers_result=refresh_open_blockers_result,
            refresh_open_blockers_root=refresh_open_blockers_root,
            refresh_open_blockers_generated_at_utc=args.open_blockers_generated_at_utc,
            refresh_open_blockers_source=args.open_blockers_source,
            checker_json_result=checker_json_result,
            checker_markdown_result=checker_markdown_result,
            checker_payload=checker_payload,
            run_spec_lint_requested=bool(args.run_spec_lint),
            spec_globs=tuple(args.spec_globs),
            spec_lint_result=spec_lint_result,
            errors=tuple(errors),
            final_status=final_status,
            final_exit_code=final_exit_code,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)

        write_text(summary_json_path, summary_json)
        write_text(report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist bootstrap readiness artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "bootstrap-readiness: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"summary={display_path(output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(output_dir / REPORT_MD_FILENAME)}"
    )
    return final_exit_code


if __name__ == "__main__":
    raise SystemExit(main())
