#!/usr/bin/env python3
"""Run deterministic repo-root open blocker audit orchestration."""

from __future__ import annotations

import argparse
import json
import sys
import textwrap
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence
from open_blocker_extraction.audit_contract import (
    validate_contract_check_result as validate_contract_check_output,
)
from open_blocker_extraction.audit_payload import validate_extract_snapshot_payload
from open_blocker_extraction.audit_scope import (
    DEFAULT_EXCLUDE_PATHS,
    build_extractor_exclude_paths,
    normalize_exclude_paths,
    normalize_include_globs,
    resolve_effective_audit_root,
    resolve_markdown_scope,
    validate_generated_at_utc,
    validate_snapshot_source,
)
from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.paths import display_path, resolve_repo_path
from objc3c_tooling.subprocesses import command_text, python_script_command, run_timed
from objc3c_tooling.public_workflow_output import normalize_newlines

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_AUDIT_ROOT = ROOT
DEFAULT_OUTPUT_DIR = ROOT / "tmp" / "reports" / "open_blocker_audit"
DEFAULT_SNAPSHOT_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"

EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH = ROOT / "scripts" / "extract_open_blockers.py"
CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH = (
    ROOT / "scripts" / "check_open_blocker_audit_contract.py"
)
EXTRACT_LOG_FILENAME = "extract_open_blockers.log"
SUMMARY_JSON_FILENAME = "open_blocker_audit_summary.json"
REPORT_MD_FILENAME = "open_blocker_audit_report.md"
CONTRACT_CHECK_TRANSCRIPT_FILENAME = "open_blocker_audit_contract_check_transcript.txt"
CONTRACT_CHECK_STDERR_FILENAME = "open_blocker_audit_contract_check.stderr.txt"
RUNNER_CONTRACT_ID = "open-blocker-audit-runner"
RUNNER_CONTRACT_VERSION = "v0.1"
RUNNER_ID = f"{RUNNER_CONTRACT_ID}/{RUNNER_CONTRACT_VERSION}"
CHECKER_MODE = "open-blocker-audit-contract-v1"
DEFAULT_COMMAND_TIMEOUT_SECONDS = 600

EXIT_OK = 0
EXIT_OPEN_BLOCKERS = 1
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






def write_text(path: Path, content: str) -> None:
    write_text_file(path, normalize_newlines(content))


def bool_text(value: bool) -> str:
    return "true" if value else "false"


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


def render_contract_check_transcript(result: CommandResult) -> str:
    command_line = command_text(python_script_command(display_path(result.spec.script_path), *result.spec.display_args))
    lines = [
        "# open_blocker_audit contract check transcript",
        "",
        "## command",
        "",
        command_line,
        "",
        "## stdout",
        "",
        result.stdout.rstrip("\n") if result.stdout else "_empty_",
        "",
        "## stderr",
        "",
        result.stderr.rstrip("\n") if result.stderr else "_empty_",
        "",
        f"Exit code: {result.exit_code}",
        "",
    ]
    return "\n".join(lines)


def summarize_command(result: CommandResult) -> dict[str, Any]:
    return {
        "argv": python_script_command(display_path(result.spec.script_path), *result.spec.display_args),
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }


def build_runner_snapshot_payload(
    extract_payload: dict[str, object],
) -> dict[str, object]:
    return {
        "contract_id": RUNNER_CONTRACT_ID,
        "contract_version": RUNNER_CONTRACT_VERSION,
        "generated_at_utc": extract_payload["generated_at_utc"],
        "source": extract_payload["source"],
        "open_blocker_count": extract_payload["open_blocker_count"],
        "open_blockers": extract_payload["open_blockers"],
    }


def determine_final_exit(*, errors: Sequence[str], blocker_count: int | None) -> tuple[int, str]:
    if errors:
        return EXIT_RUNNER_ERROR, "runner-error"
    if blocker_count is None:
        return EXIT_RUNNER_ERROR, "runner-error"
    if blocker_count > 0:
        return EXIT_OPEN_BLOCKERS, "open-blockers"
    return EXIT_OK, "ok"


def build_summary_payload(
    *,
    audit_root: Path,
    effective_audit_root: Path,
    include_globs: Sequence[str],
    exclude_paths: Sequence[str],
    extractor_exclude_paths: Sequence[str],
    generated_at_utc: str | None,
    source: str | None,
    output_dir: Path,
    snapshot_json_path: Path,
    included_markdown_paths: Sequence[str],
    excluded_markdown_paths: Sequence[str],
    extract_result: CommandResult | None,
    blocker_count: int | None,
    errors: Sequence[str],
    final_status: str,
    final_exit_code: int,
) -> dict[str, object]:
    commands: dict[str, object] = {}
    if extract_result is not None:
        commands["extract_open_blockers_snapshot_json"] = summarize_command(extract_result)

    return {
        "runner": RUNNER_ID,
        "contract_id": RUNNER_CONTRACT_ID,
        "contract_version": RUNNER_CONTRACT_VERSION,
        "inputs": {
            "audit_root": display_path(audit_root),
            "effective_audit_root": display_path(effective_audit_root),
            "include_globs": list(include_globs),
            "exclude_paths": list(exclude_paths),
            "extractor_exclude_paths": list(extractor_exclude_paths),
            "generated_at_utc": generated_at_utc,
            "source": source,
        },
        "scope": {
            "included_markdown_count": len(included_markdown_paths),
            "excluded_markdown_count": len(excluded_markdown_paths),
        },
        "artifacts": {
            "output_dir": display_path(output_dir),
            "snapshot_json": display_path(snapshot_json_path),
            "extract_log": EXTRACT_LOG_FILENAME if extract_result is not None else None,
            "summary_json": SUMMARY_JSON_FILENAME,
            "report_markdown": REPORT_MD_FILENAME,
        },
        "audit": {
            "extract_attempted": extract_result is not None,
            "extract_exit_code": extract_result.exit_code if extract_result is not None else None,
            "open_blocker_count": blocker_count,
        },
        "commands": commands,
        "errors": list(errors),
        "final_status": final_status,
        "final_exit_code": final_exit_code,
    }


def render_markdown_report(summary: dict[str, object]) -> str:
    inputs = summary["inputs"]
    scope = summary["scope"]
    artifacts = summary["artifacts"]
    audit = summary["audit"]
    commands = summary["commands"]
    errors = summary["errors"]

    assert isinstance(inputs, dict)
    assert isinstance(scope, dict)
    assert isinstance(artifacts, dict)
    assert isinstance(audit, dict)
    assert isinstance(commands, dict)
    assert isinstance(errors, list)

    exclude_paths = inputs["exclude_paths"]
    extractor_exclude_paths = inputs["extractor_exclude_paths"]
    include_globs = inputs["include_globs"]
    assert isinstance(include_globs, list)
    assert isinstance(exclude_paths, list)
    assert isinstance(extractor_exclude_paths, list)

    lines = [
        "# Open Blocker Audit Orchestration",
        "",
        "## Inputs",
        "",
        f"- Audit root: `{inputs['audit_root']}`",
        f"- Effective audit root: `{inputs['effective_audit_root']}`",
        (
            "- Include globs: " + ", ".join(f"`{value}`" for value in include_globs)
            if include_globs
            else "- Include globs: _none_"
        ),
        (
            "- Exclude paths: " + ", ".join(f"`{value}`" for value in exclude_paths)
            if exclude_paths
            else "- Exclude paths: _none_"
        ),
        (
            "- Extractor exclude paths: "
            + ", ".join(f"`{value}`" for value in extractor_exclude_paths)
            if extractor_exclude_paths
            else "- Extractor exclude paths: _none_"
        ),
        (
            "- generated_at_utc: "
            + (f"`{inputs['generated_at_utc']}`" if inputs["generated_at_utc"] else "_none_")
        ),
        "- source: " + (f"`{inputs['source']}`" if inputs["source"] else "_none_"),
        "",
        "## Scope",
        "",
        f"- Included markdown files: `{scope['included_markdown_count']}`",
        f"- Excluded markdown files: `{scope['excluded_markdown_count']}`",
        "",
        "## Audit",
        "",
        f"- Extract attempted: `{bool_text(bool(audit['extract_attempted']))}`",
        (
            "- Extract exit code: "
            + (f"`{audit['extract_exit_code']}`" if audit["extract_exit_code"] is not None else "_none_")
        ),
        (
            "- Open blocker count: "
            + (f"`{audit['open_blocker_count']}`" if audit["open_blocker_count"] is not None else "_none_")
        ),
        "",
        "## Final Outcome",
        "",
        f"- Contract ID: `{summary['contract_id']}`",
        f"- Contract version: `{summary['contract_version']}`",
        f"- Final status: `{summary['final_status']}`",
        f"- Final exit code: `{summary['final_exit_code']}`",
        "",
        "## Artifacts",
        "",
        f"- Output directory: `{artifacts['output_dir']}`",
        f"- Snapshot JSON: `{artifacts['snapshot_json']}`",
        (
            "- Extract log: "
            + (f"`{artifacts['extract_log']}`" if artifacts["extract_log"] is not None else "_none_")
        ),
        f"- Summary JSON: `{artifacts['summary_json']}`",
        f"- Report markdown: `{artifacts['report_markdown']}`",
        "",
        "## Command Exit Codes",
        "",
        "| Command | Exit Code |",
        "| --- | --- |",
    ]

    for command_name, command_payload in commands.items():
        assert isinstance(command_payload, dict)
        lines.append(f"| `{command_name}` | `{command_payload['exit_code']}` |")

    lines.extend(["", "## Errors", ""])
    if errors:
        for error in errors:
            lines.append(f"- {error}")
    else:
        lines.append("- _none_")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_open_blocker_audit.py",
        description=(
            "Run deterministic repo-root open blocker audit orchestration by invoking "
            "extract_open_blockers snapshot-json mode with explicit metadata, exclusions, "
            "schema checks, and fail-closed artifact persistence."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent(
            f"""\
            Exit semantics:
              0: no open blockers and no runner/contract errors.
              1: one or more open blockers discovered.
              2: runner error, extractor contract mismatch, or schema/provenance drift.

            Output artifact files (under --output-dir):
              - {DEFAULT_SNAPSHOT_RELATIVE_PATH.as_posix()}
              - {EXTRACT_LOG_FILENAME} (when extract_open_blockers is attempted)
              - {SUMMARY_JSON_FILENAME}
              - {REPORT_MD_FILENAME}
              - {CONTRACT_CHECK_TRANSCRIPT_FILENAME}
              - {CONTRACT_CHECK_STDERR_FILENAME}
            """
        ),
    )
    parser.add_argument(
        "--audit-root",
        type=Path,
        default=DEFAULT_AUDIT_ROOT,
        help=(
            "Root directory scanned for markdown blockers. "
            f"Default: {display_path(DEFAULT_AUDIT_ROOT)}."
        ),
    )
    parser.add_argument(
        "--generated-at-utc",
        help=(
            "Required strict UTC timestamp metadata for snapshot-json mode "
            "(YYYY-MM-DDTHH:MM:SSZ)."
        ),
    )
    parser.add_argument(
        "--source",
        help=(
            "Required canonical non-empty source metadata for snapshot-json mode "
            "(no leading/trailing or repeated internal whitespace)."
        ),
    )
    parser.add_argument(
        "--include-glob",
        action="append",
        default=[],
        dest="include_globs",
        help=(
            "Optional repository-relative markdown include glob. Repeatable. "
            "Patterns must share one static directory prefix "
            "(for example: docs/reference/**/*.md)."
        ),
    )
    parser.add_argument(
        "--exclude-path",
        action="append",
        default=[],
        dest="exclude_paths",
        help=(
            "Repeatable exclusion glob forwarded to extract_open_blockers --exclude-path. "
            "Defaults are applied unless --no-default-exclude is set."
        ),
    )
    parser.add_argument(
        "--no-default-exclude",
        action="store_true",
        help="Disable default exclusion globs and use only --exclude-path entries.",
    )
    parser.add_argument(
        "--snapshot-json",
        type=Path,
        help=(
            "Optional output path for normalized blocker snapshot JSON. "
            "Defaults to --output-dir/inputs/open_blockers.snapshot.json."
        ),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=DEFAULT_OUTPUT_DIR,
        help=(
            "Directory where deterministic audit artifacts are written. "
            f"Default: {display_path(DEFAULT_OUTPUT_DIR)}."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    audit_root = resolve_repo_path(args.audit_root)
    effective_audit_root = audit_root
    output_dir = resolve_repo_path(args.output_dir)
    snapshot_json_path = (
        resolve_repo_path(args.snapshot_json)
        if args.snapshot_json is not None
        else output_dir / DEFAULT_SNAPSHOT_RELATIVE_PATH
    )

    errors: list[str] = []
    extract_result: CommandResult | None = None
    normalized_snapshot_payload: dict[str, object] | None = None
    blocker_count: int | None = None
    included_markdown_paths: list[str] = []
    excluded_markdown_paths: list[str] = []
    extractor_exclude_paths: tuple[str, ...] = ()

    try:
        exclude_paths = normalize_exclude_paths(
            args.exclude_paths,
            include_defaults=not args.no_default_exclude,
        )
    except ValueError as exc:
        exclude_paths = ()
        errors.append(str(exc))

    try:
        include_globs = normalize_include_globs(args.include_globs)
    except ValueError as exc:
        include_globs = ()
        errors.append(str(exc))

    if not errors:
        try:
            effective_audit_root = resolve_effective_audit_root(
                audit_root=audit_root,
                include_globs=include_globs,
            )
        except ValueError as exc:
            errors.append(str(exc))

    generated_at_utc: str | None = None
    source: str | None = None

    if args.generated_at_utc is None:
        errors.append("--generated-at-utc is required.")
    else:
        try:
            generated_at_utc = validate_generated_at_utc(args.generated_at_utc)
        except ValueError as exc:
            errors.append(str(exc))

    if args.source is None:
        errors.append("--source is required.")
    else:
        try:
            source = validate_snapshot_source(args.source)
        except ValueError as exc:
            errors.append(str(exc))

    if not errors:
        try:
            included_markdown_paths, excluded_markdown_paths = resolve_markdown_scope(
                audit_root=effective_audit_root,
                exclude_paths=exclude_paths,
            )
        except ValueError as exc:
            errors.append(str(exc))

    if not errors and not included_markdown_paths:
        errors.append(
            "no markdown files matched audit scope after exclusions under "
            f"{display_path(effective_audit_root)}."
        )

    if not errors:
        extractor_exclude_paths = build_extractor_exclude_paths(
            exclude_paths=exclude_paths,
            markdown_paths=[*included_markdown_paths, *excluded_markdown_paths],
        )

    if not errors:
        assert generated_at_utc is not None
        assert source is not None

        actual_args: list[str] = [
            "--root",
            str(effective_audit_root),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            generated_at_utc,
            "--source",
            source,
        ]
        display_args: list[str] = [
            "--root",
            display_path(effective_audit_root),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            generated_at_utc,
            "--source",
            source,
        ]
        for exclude_path in extractor_exclude_paths:
            actual_args.extend(["--exclude-path", exclude_path])
            display_args.extend(["--exclude-path", exclude_path])

        extract_spec = CommandSpec(
            name="extract_open_blockers_snapshot_json",
            script_path=EXTRACT_OPEN_BLOCKERS_SCRIPT_PATH,
            actual_args=tuple(actual_args),
            display_args=tuple(display_args),
        )
        extract_result = run_command(extract_spec)

        if extract_result.exit_code != 0:
            errors.append(
                "extract_open_blockers(snapshot-json) returned unexpected exit code "
                f"{extract_result.exit_code}."
            )
        else:
            try:
                raw_payload = json.loads(extract_result.stdout)
            except json.JSONDecodeError as exc:
                errors.append(
                    "extract_open_blockers(snapshot-json) emitted invalid JSON: "
                    f"{exc.msg} at {exc.lineno}:{exc.colno}."
                )
            else:
                if not isinstance(raw_payload, dict):
                    errors.append(
                        "extract_open_blockers(snapshot-json) output root must be an object."
                    )
                else:
                    try:
                        normalized_extract_payload = validate_extract_snapshot_payload(
                            raw_payload,
                            expected_generated_at_utc=generated_at_utc,
                            expected_source=source,
                        )
                        normalized_snapshot_payload = build_runner_snapshot_payload(
                            normalized_extract_payload
                        )
                    except ValueError as exc:
                        errors.append(str(exc))

    if normalized_snapshot_payload is not None:
        raw_count = normalized_snapshot_payload.get("open_blocker_count")
        if isinstance(raw_count, int) and not isinstance(raw_count, bool):
            blocker_count = raw_count

    final_exit_code, final_status = determine_final_exit(
        errors=errors,
        blocker_count=blocker_count,
    )

    extract_log_path = output_dir / EXTRACT_LOG_FILENAME
    summary_json_path = output_dir / SUMMARY_JSON_FILENAME
    report_md_path = output_dir / REPORT_MD_FILENAME
    contract_check_transcript_path = output_dir / CONTRACT_CHECK_TRANSCRIPT_FILENAME
    contract_check_stderr_path = output_dir / CONTRACT_CHECK_STDERR_FILENAME

    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        if extract_result is not None:
            write_text(
                extract_log_path,
                render_command_log(
                    "extract_open_blockers snapshot-json command output",
                    extract_result,
                ),
            )

        if normalized_snapshot_payload is not None:
            write_text(
                snapshot_json_path,
                json.dumps(normalized_snapshot_payload, indent=2) + "\n",
            )

        summary = build_summary_payload(
            audit_root=audit_root,
            effective_audit_root=effective_audit_root,
            include_globs=include_globs,
            exclude_paths=exclude_paths,
            extractor_exclude_paths=extractor_exclude_paths,
            generated_at_utc=generated_at_utc,
            source=source,
            output_dir=output_dir,
            snapshot_json_path=snapshot_json_path,
            included_markdown_paths=included_markdown_paths,
            excluded_markdown_paths=excluded_markdown_paths,
            extract_result=extract_result,
            blocker_count=blocker_count,
            errors=tuple(errors),
            final_status=final_status,
            final_exit_code=final_exit_code,
        )
        summary_json = json.dumps(summary, indent=2) + "\n"
        report_markdown = render_markdown_report(summary)
        write_text(summary_json_path, summary_json)
        write_text(report_md_path, report_markdown)

        contract_check_actual_args = [
            "--summary",
            str(summary_json_path),
            "--snapshot",
            str(snapshot_json_path),
            "--extract-log",
            str(extract_log_path),
            "--contract-id",
            RUNNER_CONTRACT_ID,
            "--contract-version",
            RUNNER_CONTRACT_VERSION,
        ]
        contract_check_display_args = [
            "--summary",
            display_path(summary_json_path),
            "--snapshot",
            display_path(snapshot_json_path),
            "--extract-log",
            display_path(extract_log_path),
            "--contract-id",
            RUNNER_CONTRACT_ID,
            "--contract-version",
            RUNNER_CONTRACT_VERSION,
        ]
        contract_check_spec = CommandSpec(
            name="check_open_blocker_audit_contract",
            script_path=CHECK_OPEN_BLOCKER_AUDIT_CONTRACT_SCRIPT_PATH,
            actual_args=tuple(contract_check_actual_args),
            display_args=tuple(contract_check_display_args),
        )
        contract_check_result = run_command(contract_check_spec)

        write_text(
            contract_check_transcript_path,
            render_contract_check_transcript(contract_check_result),
        )
        write_text(contract_check_stderr_path, contract_check_result.stderr)

        contract_check_errors = validate_contract_check_output(
            contract_check_result,
            summary_json_path=summary_json_path,
            snapshot_json_path=snapshot_json_path,
            extract_log_path=extract_log_path,
            checker_mode=CHECKER_MODE,
            runner_contract_id=RUNNER_CONTRACT_ID,
            runner_contract_version=RUNNER_CONTRACT_VERSION,
            exit_ok=EXIT_OK,
        )
        if contract_check_errors:
            errors.extend(contract_check_errors)
            final_exit_code, final_status = determine_final_exit(
                errors=errors,
                blocker_count=blocker_count,
            )
            summary = build_summary_payload(
                audit_root=audit_root,
                effective_audit_root=effective_audit_root,
                include_globs=include_globs,
                exclude_paths=exclude_paths,
                extractor_exclude_paths=extractor_exclude_paths,
                generated_at_utc=generated_at_utc,
                source=source,
                output_dir=output_dir,
                snapshot_json_path=snapshot_json_path,
                included_markdown_paths=included_markdown_paths,
                excluded_markdown_paths=excluded_markdown_paths,
                extract_result=extract_result,
                blocker_count=blocker_count,
                errors=tuple(errors),
                final_status=final_status,
                final_exit_code=final_exit_code,
            )
            summary_json = json.dumps(summary, indent=2) + "\n"
            report_markdown = render_markdown_report(summary)
            write_text(summary_json_path, summary_json)
            write_text(report_md_path, report_markdown)
    except OSError as exc:
        print(f"error: unable to persist open blocker audit artifacts: {exc}", file=sys.stderr)
        return EXIT_RUNNER_ERROR

    print(
        "open-blocker-audit: "
        f"status={final_status} "
        f"exit_code={final_exit_code} "
        f"snapshot={display_path(snapshot_json_path)} "
        f"summary={display_path(summary_json_path)} "
        f"report={display_path(report_md_path)} "
        f"contract_check_transcript={display_path(contract_check_transcript_path)} "
        f"contract_check_stderr={display_path(contract_check_stderr_path)}"
    )
    return final_exit_code


if __name__ == "__main__":
    raise SystemExit(main())
