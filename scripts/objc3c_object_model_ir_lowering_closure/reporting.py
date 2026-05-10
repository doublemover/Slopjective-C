from __future__ import annotations

from objc3c_object_model_ir_lowering_closure.paths import JSON_OUT
from objc3c_object_model_ir_lowering_closure.paths import MD_OUT
from objc3c_tooling.reports import write_report_outputs


def render_markdown(summary: dict) -> str:
    lines = [
        "# Object Model IR Lowering Closure",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        f"- Negative fixture: `{summary['negative_fixture']}`",
        f"- Source truth avoids tmp: `{summary['no_tmp_source_truth']}`",
        "",
        "## Layout Offsets",
    ]
    for property_name, offset in summary["ir_offsets_by_property"].items():
        lines.append(f"- `{property_name}`: `{offset}`")
    lines.extend(["", "## Checks"])
    for name, values in summary["emission_summary_checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if values else 'FAIL'}`")
    for section, values in summary["source_surfaces"].items():
        lines.append(f"- `{section}`: `{'PASS' if all(values.values()) else 'FAIL'}`")
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)


def write_outputs(summary: dict) -> None:
    write_report_outputs(
        summary=summary,
        json_path=JSON_OUT,
        markdown_path=MD_OUT,
        markdown=render_markdown(summary),
    )
