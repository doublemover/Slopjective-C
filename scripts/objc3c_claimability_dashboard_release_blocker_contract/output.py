from __future__ import annotations

from pathlib import Path
from typing import Any

from .rendering import render_markdown
from .tooling import write_report_outputs


def write_outputs(summary: dict[str, Any], json_out: Path, md_out: Path) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_out,
        markdown_path=md_out,
        markdown=render_markdown(summary),
        sort_keys=False,
    )
