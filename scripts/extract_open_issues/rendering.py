"""Report rendering for open-issue extraction."""

from __future__ import annotations

import json
from collections.abc import Mapping, Sequence

from .config import OutputFormat


def render_json_report(records: Sequence[Mapping[str, object]]) -> str:
    return json.dumps(
        list(records),
        indent=2,
        sort_keys=False,
        ensure_ascii=True,
        allow_nan=False,
    ) + "\n"


def render_markdown(records: Sequence[Mapping[str, object]]) -> str:
    lines = ["# Open issues", ""]
    if not records:
        lines.append("_No open-issues sections found._")
        return "\n".join(lines) + "\n"

    for record in records:
        lines.append(
            f"## {record['file']} - {record['heading']} (line {record['line']})"
        )
        items = record["items"]
        if not isinstance(items, list):
            raise TypeError("open issue record items must be a list")
        if items:
            for item in items:
                lines.append(f"- {item}")
        else:
            lines.append("- None")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def render_report(records: Sequence[Mapping[str, object]], output_format: OutputFormat) -> str:
    if output_format == "markdown":
        return render_markdown(records)
    if output_format == "json":
        return render_json_report(records)
    raise ValueError(f"unsupported open-issues output format: {output_format}")


__all__ = (
    "render_json_report",
    "render_markdown",
    "render_report",
)
