"""Activation preflight command log rendering."""

from __future__ import annotations

from scripts.activation_preflight.contracts import CommandResult


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


def render_spec_lint_log(result: CommandResult) -> str:
    return render_command_log("spec_lint command output", result)


__all__ = [
    "render_command_log",
    "render_spec_lint_log",
]
