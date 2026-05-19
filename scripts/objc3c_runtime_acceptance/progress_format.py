"""Progress formatting helpers for runtime acceptance."""

from __future__ import annotations

from pathlib import Path

from .paths import ROOT


def round_seconds(seconds: float) -> float:
    return round(seconds, 6)


def format_seconds(seconds: float) -> str:
    return f"{seconds:.3f}s"


def repo_display_path(path: Path) -> str:
    if str(path) == "":
        return ""
    try:
        return str(path.relative_to(ROOT)).replace("\\", "/")
    except ValueError:
        return str(path).replace("\\", "/")


def command_display(command: list[str]) -> str:
    display_parts: list[str] = []
    for token in command:
        token_path = Path(token)
        if token_path.is_absolute():
            display_parts.append(repo_display_path(token_path))
        else:
            display_parts.append(token)
    rendered = " ".join(display_parts)
    if len(rendered) > 240:
        return rendered[:237] + "..."
    return rendered


__all__ = [
    "command_display",
    "format_seconds",
    "repo_display_path",
    "round_seconds",
]
