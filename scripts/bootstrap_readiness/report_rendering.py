from __future__ import annotations

from typing import Any

from .models import CommandResult
from .report_models import SummaryPayload


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


def render_markdown_report(summary: SummaryPayload) -> str:
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
