#!/usr/bin/env python3
"""Run deterministic bootstrap readiness orchestration and persist evidence artifacts."""

from __future__ import annotations

import argparse
import json
import sys
import textwrap
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.paths import display_path, resolve_repo_path
from objc3c_tooling.subprocesses import python_script_command, run_timed
from objc3c_tooling.public_workflow_output import normalize_newlines

if __package__:
    from .bootstrap_readiness.models import CommandResult, CommandSpec
    from .bootstrap_readiness.reports import (
        BOOTSTRAP_JSON_FILENAME,
        BOOTSTRAP_JSON_LOG_FILENAME,
        BOOTSTRAP_MD_FILENAME,
        BOOTSTRAP_MD_LOG_FILENAME,
        OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
        REPORT_MD_FILENAME,
        SPEC_LINT_LOG_FILENAME,
        SUMMARY_JSON_FILENAME,
        build_summary_payload,
        render_command_log,
        render_markdown_report,
    )
else:
    from bootstrap_readiness.models import CommandResult, CommandSpec
    from bootstrap_readiness.reports import (
        BOOTSTRAP_JSON_FILENAME,
        BOOTSTRAP_JSON_LOG_FILENAME,
        BOOTSTRAP_MD_FILENAME,
        BOOTSTRAP_MD_LOG_FILENAME,
        OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
        REPORT_MD_FILENAME,
        SPEC_LINT_LOG_FILENAME,
        SUMMARY_JSON_FILENAME,
        build_summary_payload,
        render_command_log,
        render_markdown_report,
    )

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG_JSON = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "bootstrap_readiness"
DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
DEFAULT_COMMAND_TIMEOUT_SECONDS = 600

CHECK_BOOTSTRAP_READINESS_SCRIPT_PATH = ROOT / "scripts" / "check_bootstrap_readiness.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"

EXIT_BOOTSTRAPPABLE = 0
EXIT_BLOCKED = 1
EXIT_RUNNER_ERROR = 2


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / DEFAULT_REFRESH_OPEN_BLOCKERS_RELATIVE_PATH


def write_text(path: Path, content: str) -> None:
    write_text_file(path, normalize_newlines(content))


def run_command(spec: CommandSpec) -> CommandResult:
    command = python_script_command(spec.script_path, *spec.actual_args)
    execution = run_timed(command, cwd=ROOT, timeout=DEFAULT_COMMAND_TIMEOUT_SECONDS)
    exit_code = EXIT_RUNNER_ERROR if execution.timeout_seconds is not None else int(execution.returncode)
    return CommandResult(
        spec=spec,
        exit_code=exit_code,
        stdout=normalize_newlines(execution.stdout),
        stderr=normalize_newlines(execution.stderr),
    )


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
