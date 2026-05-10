"""Runner support for deterministic open-blocker audit orchestration."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import write_text_file
from objc3c_tooling.paths import display_path
from objc3c_tooling.public_workflow_output import normalize_newlines
from objc3c_tooling.subprocesses import command_text, python_script_command, run_timed

DEFAULT_SNAPSHOT_RELATIVE_PATH = Path("inputs") / "open_blockers.snapshot.json"
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


def run_command(spec: CommandSpec, *, root: Path) -> CommandResult:
    command = python_script_command(spec.script_path, *spec.actual_args)
    execution = run_timed(command, cwd=root, timeout=DEFAULT_COMMAND_TIMEOUT_SECONDS)
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
