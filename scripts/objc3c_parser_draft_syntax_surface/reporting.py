from __future__ import annotations

from typing import Any

from objc3c_parser_draft_syntax_surface.paths import JSON_OUT
from objc3c_parser_draft_syntax_surface.paths import MD_OUT
from objc3c_parser_draft_syntax_surface.rendering import render_markdown
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs


def expected_reports(summary: dict[str, Any]) -> tuple[str, str]:
    return expected_json_report(summary), render_markdown(summary)


def write_outputs(summary: dict[str, Any]) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )
