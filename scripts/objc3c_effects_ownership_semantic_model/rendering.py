from __future__ import annotations

from typing import Any

from objc3c_tooling.reports import write_report_outputs

from objc3c_effects_ownership_semantic_model.paths import JSON_OUT
from objc3c_effects_ownership_semantic_model.paths import MD_OUT


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Effects Ownership Semantic Model",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Missing required slices fixture: `{summary['missing_required_slices_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Observed Positive Counts"])
    model = summary.get("effects_ownership_semantic_model") or {}
    for field in summary["positive_minimum_counts"]:
        lines.append(f"- `{field}`: `{model.get(field)}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict[str, Any]) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )
