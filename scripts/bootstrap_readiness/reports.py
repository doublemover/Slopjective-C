from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path
from objc3c_tooling.subprocesses import python_script_command

from .models import CommandResult

BOOTSTRAP_JSON_FILENAME = "check_bootstrap_readiness.json"
BOOTSTRAP_MD_FILENAME = "check_bootstrap_readiness.md"
BOOTSTRAP_JSON_LOG_FILENAME = "check_bootstrap_readiness_json.log"
BOOTSTRAP_MD_LOG_FILENAME = "check_bootstrap_readiness_markdown.log"
OPEN_BLOCKERS_REFRESH_LOG_FILENAME = "extract_open_blockers.log"
SPEC_LINT_LOG_FILENAME = "spec_lint.log"
SUMMARY_JSON_FILENAME = "bootstrap_readiness_summary.json"
REPORT_MD_FILENAME = "bootstrap_readiness_report.md"


def bool_text(value: bool) -> str:
    return "true" if value else "false"


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
        "argv": python_script_command(display_path(result.spec.script_path), *result.spec.display_args),
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }


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
