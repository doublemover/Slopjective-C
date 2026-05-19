from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_cross_module_semantic_contracts_diagnostics.paths import JSON_OUT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import MD_OUT
from objc3c_tooling.reports import expected_json_report
from objc3c_tooling.reports import write_report_outputs


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Cross-Module Semantic Contracts Diagnostics",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Observed Positive Counts"])
    model = summary.get("cross_module_semantic_contracts_diagnostics_model") or {}
    for field in summary["positive_minimum_counts"]:
        lines.append(f"- `{field}`: `{model.get(field)}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def expected_report_outputs(summary: dict[str, Any]) -> tuple[str, str]:
    return expected_json_report(summary), render_markdown(summary)


def write_outputs(summary: dict[str, Any], *, json_path: Path = JSON_OUT, markdown_path: Path = MD_OUT) -> None:
    write_report_outputs(
        summary=summary,
        json_path=json_path,
        markdown_path=markdown_path,
        markdown=render_markdown(summary),
    )
