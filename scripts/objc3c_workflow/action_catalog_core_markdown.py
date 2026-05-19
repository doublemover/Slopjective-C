"""Core markdown action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_MARKDOWN_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-markdown": ActionSpec("check-markdown", "check markdown formatting drift across checked-in docs", "npx prettier --check <checked-in-md-globs>"),
    "format-markdown": ActionSpec("format-markdown", "rewrite markdown formatting across checked-in docs", "npx prettier --write <checked-in-md-globs>"),
    "lint-markdown": ActionSpec("lint-markdown", "run markdownlint across checked-in docs", "npx markdownlint-cli2 <checked-in-md-globs>"),
}

__all__ = ["CORE_MARKDOWN_ACTION_SPECS"]
