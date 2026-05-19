from __future__ import annotations

from typing import Any


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Parser Draft Syntax Surface",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        f"- Source truth avoids tmp: `{summary['no_tmp_source_truth']}`",
        "",
        "## Checks",
    ]
    for section, values in summary["checks"].items():
        lines.append(f"- `{section}`: `{'PASS' if all(values.values()) else 'FAIL'}`")
    lines.append("")
    lines.append("## Validation Commands")
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)
