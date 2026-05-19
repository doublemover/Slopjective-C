from __future__ import annotations

import difflib
import hashlib

try:
    from scripts.build_pages import render_markdown_source
except ModuleNotFoundError:  # pragma: no cover - script-path execution compatibility
    from build_pages import render_markdown_source

from .models import ContractConfig


def render_expected(config: ContractConfig) -> tuple[str, int]:
    text = config.front_matter + render_markdown_source(config.body_path)
    return text, 1


def format_diff(actual: str, expected: str) -> str:
    diff_lines = list(
        difflib.unified_diff(
            actual.splitlines(),
            expected.splitlines(),
            fromfile="site/index.md (actual)",
            tofile="site/index.md (expected)",
            lineterm="",
        )
    )
    if not diff_lines:
        return ""
    preview = diff_lines[:60]
    return "\n".join(preview)


def digest(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()[:16]
