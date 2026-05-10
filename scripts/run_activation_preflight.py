#!/usr/bin/env python3
"""Run deterministic activation preflight orchestration and persist evidence artifacts."""

from __future__ import annotations

import argparse
import json
import sys
import textwrap
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.activation_preflight.command_runner import (
    DEFAULT_COMMAND_TIMEOUT_SECONDS,
    run_command,
)
from scripts.activation_preflight.command_specs import (
    activation_check_specs,
    open_blockers_refresh_spec,
    snapshot_refresh_spec,
    spec_lint_spec,
)
from scripts.activation_preflight.contracts import CommandResult, CommandSpec
from scripts.activation_preflight.payload_validation import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
    EXIT_RUNNER_ERROR,
    check_markdown_gate_consistency,
    parse_activation_payload,
)
from scripts.activation_preflight.reports import (
    ACTIVATION_JSON_FILENAME,
    ACTIVATION_MD_FILENAME,
    OPEN_BLOCKERS_REFRESH_LOG_FILENAME,
    REPORT_MD_FILENAME,
    SNAPSHOT_CAPTURE_LOG_FILENAME,
    SPEC_LINT_LOG_FILENAME,
    SUMMARY_JSON_FILENAME,
    build_summary_payload,
    render_command_log,
    render_markdown_report,
    render_spec_lint_log,
)
from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.paths import display_path, resolve_repo_path
from objc3c_tooling.public_workflow_output import normalize_newlines

DEFAULT_CATALOG_JSON = ROOT / "tmp" / "reports" / "remaining_task_review_catalog.json"
DEFAULT_OPEN_BLOCKERS_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "activation_preflight"

ACTIVATION_CHECK_SCRIPT_PATH = ROOT / "scripts" / "check_activation_triggers.py"
SPEC_LINT_SCRIPT_PATH = ROOT / "scripts" / "spec_lint.py"
CAPTURE_SNAPSHOTS_SCRIPT_PATH = ROOT / "scripts" / "capture_activation_snapshots.py"
EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"

OPEN_BLOCKERS_REFRESH_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
DEFAULT_ACTIONABLE_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")


def default_open_blockers_output_path(output_dir: Path) -> Path:
    return output_dir / OPEN_BLOCKERS_REFRESH_RELATIVE_PATH


def write_text(path: Path, content: str) -> None:
    write_text_file(path, normalize_newlines(content))


def parse_non_negative_int(raw: str) -> int:
    try:
        value = int(raw)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("must be an integer") from exc
    if value < 0:
        raise argparse.ArgumentTypeError("must be >= 0")
    return value


def normalize_actionable_statuses(raw_values: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_values:
        return DEFAULT_ACTIONABLE_STATUSES

    normalized: list[str] = []
    seen: set[str] = set()
    for raw in raw_values:
        status = raw.strip().lower()
        if not status:
            raise ValueError("actionable status filters must be non-empty")
        if status in seen:
            continue
        seen.add(status)
        normalized.append(status)
    if not normalized:
        raise ValueError("no actionable statuses were provided")
    return tuple(normalized)


def determine_final_exit(
    *,
    activation_payload: dict[str, Any] | None,
    spec_lint_exit_code: int,
    errors: Sequence[str],
) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"

    assert activation_payload is not None
    gate_open = bool(activation_payload["gate_open"])
    if gate_open:
        return EXIT_GATE_OPEN, "activation-open"

    if spec_lint_exit_code == 0:
        return EXIT_GATE_CLOSED, "ok"

    return EXIT_RUNNER_ERROR, "spec-lint-failed"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_activation_preflight.py",
        description=(
            "Run deterministic activation preflight orchestration "
            "(check_activation_triggers json+markdown + spec_lint) and persist evidence artifacts."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: gate closed and spec_lint succeeded.
              1: gate open (activation-open signal is preserved deterministically).
              2: orchestration error, malformed activation output, or spec_lint failure while gate is closed.

            Output artifact files (under --output-dir):
              - {ACTIVATION_JSON_FILENAME}
              - {ACTIVATION_MD_FILENAME}
              - {SPEC_LINT_LOG_FILENAME}
              - {SNAPSHOT_CAPTURE_LOG_FILENAME} (when --refresh-snapshots is used)
              - {OPEN_BLOCKERS_REFRESH_LOG_FILENAME} (when --refresh-open-blockers is used)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        required=True,
        help="Path to offline issues snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--milestones-json",
        type=Path,
        required=True,
        help="Path to offline milestones snapshot JSON passed to check_activation_triggers.",
    )
    parser.add_argument(
        "--refresh-snapshots",
        action="store_true",
        help=(
            "Refresh issues/milestones snapshots before running activation checks by invoking "
            "scripts/capture_activation_snapshots.py."
        ),
    )
    parser.add_argument(
        "--snapshot-generated-at-utc",
        help=(
            "Optional generated_at_utc timestamp forwarded to capture_activation_snapshots "
            "when --refresh-snapshots is set."
        ),
    )
    parser.add_argument(
        "--issues-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for issues.",
    )
    parser.add_argument(
        "--milestones-max-age-seconds",
        type=parse_non_negative_int,
        help="Optional freshness max-age in seconds forwarded to check_activation_triggers for milestones.",
    )
    parser.add_argument(
        "--catalog-json",
        type=Path,
        default=DEFAULT_CATALOG_JSON,
        help=(
            "Path to remaining-task catalog JSON passed to check_activation_triggers. "
            f"Default: {display_path(DEFAULT_CATALOG_JSON)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-json",
        type=Path,
        help=(
            "Optional open blockers JSON forwarded to check_activation_triggers "
            "for deterministic blocker-trigger gating. When --refresh-open-blockers "
            "is set and this path is omitted, the refreshed snapshot is written to "
            f"{OPEN_BLOCKERS_REFRESH_RELATIVE_PATH.as_posix()} under --output-dir."
        ),
    )
    parser.add_argument(
        "--refresh-open-blockers",
        action="store_true",
        help=(
            "Refresh open-blockers snapshot before activation checks by invoking "
            "scripts/extract_open_blockers.py with --format snapshot-json."
        ),
    )
    parser.add_argument(
        "--open-blockers-root",
        type=Path,
        default=DEFAULT_OPEN_BLOCKERS_ROOT,
        help=(
            "Root directory scanned for blocker rows when --refresh-open-blockers is set. "
            f"Default: {display_path(DEFAULT_OPEN_BLOCKERS_ROOT)}."
        ),
    )
    parser.add_argument(
        "--open-blockers-generated-at-utc",
        help=(
            "generated_at_utc metadata forwarded to extract_open_blockers "
            "snapshot-json mode (required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--open-blockers-source",
        help=(
            "source metadata forwarded to extract_open_blockers snapshot-json mode "
            "(required by snapshot-json format)."
        ),
    )
    parser.add_argument(
        "--actionable-status",
        action="append",
        dest="actionable_statuses",
        help=(
            "Repeatable actionable status forwarded to check_activation_triggers. "
            "Defaults are inherited when omitted."
        ),
    )
    t4_group = parser.add_mutually_exclusive_group()
    t4_group.add_argument(
        "--t4-governance-overlay-json",
        type=Path,
        help="Optional governance overlay JSON forwarded to check_activation_triggers.",
    )
    t4_group.add_argument(
        "--t4-new-scope-publish",
        action="store_true",
        help="Optional T4 override flag forwarded to check_activation_triggers.",
    )
    parser.add_argument(
        "--spec-glob",
        action="append",
        default=[],
        dest="spec_globs",
        help=(
            "Repeatable glob passed to spec_lint via --glob. "
            "If omitted, spec_lint defaults are used."
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
    open_blockers_path = (
        resolve_repo_path(args.open_blockers_json)
        if args.open_blockers_json is not None
        else None
    )
    output_dir = resolve_repo_path(args.output_dir)
    t4_overlay_path = (
        resolve_repo_path(args.t4_governance_overlay_json)
        if args.t4_governance_overlay_json is not None
        else None
    )
    try:
        expected_actionable_statuses = normalize_actionable_statuses(args.actionable_statuses)
    except ValueError as exc:
        expected_actionable_statuses = tuple(
            status for status in (args.actionable_statuses or []) if status
        )
        if not expected_actionable_statuses:
            expected_actionable_statuses = DEFAULT_ACTIONABLE_STATUSES
        errors = [f"invalid actionable-status input: {exc}."]
    else:
        errors: list[str] = []

    snapshot_refresh_result: CommandResult | None = None
    open_blockers_refresh_result: CommandResult | None = None
    open_blockers_refresh_root = (
        resolve_repo_path(args.open_blockers_root) if args.refresh_open_blockers else None
    )

    if args.refresh_snapshots:
        capture_spec = snapshot_refresh_spec(
            script_path=CAPTURE_SNAPSHOTS_SCRIPT_PATH,
            issues_path=issues_path,
            milestones_path=milestones_path,
            generated_at_utc=args.snapshot_generated_at_utc,
        )
        snapshot_refresh_result = run_command(capture_spec)
        if snapshot_refresh_result.exit_code != 0:
            errors.append(
                "capture_activation_snapshots returned unexpected exit code "
                f"{snapshot_refresh_result.exit_code}."
            )

    if args.refresh_open_blockers:
        if open_blockers_path is None:
            open_blockers_path = default_open_blockers_output_path(output_dir)

        assert open_blockers_refresh_root is not None
        extract_spec = open_blockers_refresh_spec(
            script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
            root=open_blockers_refresh_root,
            generated_at_utc=args.open_blockers_generated_at_utc,
            source=args.open_blockers_source,
        )
        open_blockers_refresh_result = run_command(extract_spec)
        if open_blockers_refresh_result.exit_code != 0:
            errors.append(
                "extract_open_blockers(snapshot-json) returned unexpected exit code "
                f"{open_blockers_refresh_result.exit_code}."
            )
        else:
            try:
                write_text(open_blockers_path, open_blockers_refresh_result.stdout)
            except OSError as exc:
                errors.append(
                    "unable to persist refreshed open blockers snapshot to "
                    f"{display_path(open_blockers_path)}: {exc}."
                )

    activation_json_spec, activation_markdown_spec = activation_check_specs(
        script_path=ACTIVATION_CHECK_SCRIPT_PATH,
        issues_path=issues_path,
        milestones_path=milestones_path,
        catalog_path=catalog_path,
        open_blockers_path=open_blockers_path,
        actionable_statuses=args.actionable_statuses,
        issues_max_age_seconds=args.issues_max_age_seconds,
        milestones_max_age_seconds=args.milestones_max_age_seconds,
        t4_overlay_path=t4_overlay_path,
        t4_new_scope_publish=bool(args.t4_new_scope_publish),
    )
    spec_lint_command_spec = spec_lint_spec(
        script_path=SPEC_LINT_SCRIPT_PATH,
        spec_globs=args.spec_globs,
    )

    activation_json_result = run_command(activation_json_spec)
    activation_markdown_result = run_command(activation_markdown_spec)
    spec_lint_result = run_command(spec_lint_command_spec)

    activation_payload, activation_error = parse_activation_payload(
        activation_json_result,
        expected_issues_path=issues_path,
        expected_milestones_path=milestones_path,
        expected_catalog_path=catalog_path,
        expected_open_blockers_path=open_blockers_path,
        expected_t4_overlay_path=t4_overlay_path,
        expected_actionable_statuses=expected_actionable_statuses,
        expected_issues_max_age_seconds=args.issues_max_age_seconds,
        expected_milestones_max_age_seconds=args.milestones_max_age_seconds,
    )
    if activation_error is not None:
        errors.append(activation_error)

    if activation_payload is not None:
        markdown_error = check_markdown_gate_consistency(
            activation_markdown_result,
            activation_payload=activation_payload,
        )
        if markdown_error is not None:
            errors.append(markdown_error)

    final_exit_code, final_status = determine_final_exit(
        activation_payload=activation_payload,
        spec_lint_exit_code=spec_lint_result.exit_code,
        errors=errors,
    )

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        activation_json_path = output_dir / ACTIVATION_JSON_FILENAME
        activation_md_path = output_dir / ACTIVATION_MD_FILENAME
        spec_lint_log_path = output_dir / SPEC_LINT_LOG_FILENAME
        snapshot_capture_log_path = output_dir / SNAPSHOT_CAPTURE_LOG_FILENAME
        open_blockers_refresh_log_path = output_dir / OPEN_BLOCKERS_REFRESH_LOG_FILENAME
        summary_json_path = output_dir / SUMMARY_JSON_FILENAME
        report_md_path = output_dir / REPORT_MD_FILENAME

        write_text(activation_json_path, activation_json_result.stdout)
        write_text(activation_md_path, activation_markdown_result.stdout)
        write_text(spec_lint_log_path, render_spec_lint_log(spec_lint_result))
        if snapshot_refresh_result is not None:
            write_text(
                snapshot_capture_log_path,
                render_command_log("capture_activation_snapshots command output", snapshot_refresh_result),
            )
        if open_blockers_refresh_result is not None:
            write_text(
                open_blockers_refresh_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    open_blockers_refresh_result,
                ),
            )

        summary = build_summary_payload(
            issues_path=issues_path,
            milestones_path=milestones_path,
            catalog_path=catalog_path,
            open_blockers_path=open_blockers_path,
            output_dir=output_dir,
            spec_globs=tuple(args.spec_globs),
            snapshot_refresh_requested=bool(args.refresh_snapshots),
            snapshot_refresh_result=snapshot_refresh_result,
            open_blockers_refresh_requested=bool(args.refresh_open_blockers),
            open_blockers_refresh_result=open_blockers_refresh_result,
            open_blockers_refresh_root=open_blockers_refresh_root,
            open_blockers_refresh_generated_at_utc=args.open_blockers_generated_at_utc,
            open_blockers_refresh_source=args.open_blockers_source,
            issues_max_age_seconds=args.issues_max_age_seconds,
            milestones_max_age_seconds=args.milestones_max_age_seconds,
            snapshot_generated_at_utc=args.snapshot_generated_at_utc,
            activation_payload=activation_payload,
            activation_json_result=activation_json_result,
            activation_markdown_result=activation_markdown_result,
            spec_lint_result=spec_lint_result,
            errors=tuple(errors),
            final_exit_code=final_exit_code,
            final_status=final_status,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)

        write_text(summary_json_path, summary_json)
        write_text(report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist preflight artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "activation-preflight: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"summary={display_path(output_dir / SUMMARY_JSON_FILENAME)} "
        f"report={display_path(output_dir / REPORT_MD_FILENAME)}"
    )
    return final_exit_code


if __name__ == "__main__":
    raise SystemExit(main())
