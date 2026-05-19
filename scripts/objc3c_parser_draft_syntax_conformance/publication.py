from __future__ import annotations

from typing import Any

from objc3c_parser_draft_syntax_conformance.paths import JSON_OUT
from objc3c_parser_draft_syntax_conformance.paths import MD_OUT
from objc3c_parser_draft_syntax_conformance.rendering import render_markdown
from objc3c_tooling.reports import write_report_outputs


def write_outputs(summary: dict[str, Any]) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )
