"""Open-blocker audit runner artifact rendering."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import display_path
from objc3c_tooling.subprocesses import command_text, python_script_command

from .audit_runner_io import bool_text
from .audit_runner_models import CommandResult


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
    command_line = command_text(
        python_script_command(
            display_path(result.spec.script_path),
            *result.spec.display_args,
        )
    )
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
            + (
                f"`{audit['extract_exit_code']}`"
                if audit["extract_exit_code"] is not None
                else "_none_"
            )
        ),
        (
            "- Open blocker count: "
            + (
                f"`{audit['open_blocker_count']}`"
                if audit["open_blocker_count"] is not None
                else "_none_"
            )
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
            + (
                f"`{artifacts['extract_log']}`"
                if artifacts["extract_log"] is not None
                else "_none_"
            )
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
