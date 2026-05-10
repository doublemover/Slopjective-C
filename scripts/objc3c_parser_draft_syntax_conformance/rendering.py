from __future__ import annotations

from typing import Any


def render_markdown(summary: dict[str, Any]) -> str:
    lines = [
        "# Parser Draft Syntax Conformance",
        "",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Issue: `{summary['issue']}`",
        f"- Positive fixture: `{summary['positive_fixture']}`",
        "",
        "## Checks",
    ]
    for name, passed in summary["checks"].items():
        lines.append(f"- `{name}`: `{'PASS' if passed else 'FAIL'}`")
    lines.extend(["", "## Surface Coverage"])
    for result in summary["surface_results"]:
        negative_count = len(result["negative_results"])
        lines.append(
            f"- `{result['id']}`: `{result['status']}`; "
            f"replay `{result['replay_key_field']}`=`{result['replay_key_count']}`; "
            f"negative fixtures=`{negative_count}`"
        )
    lines.extend(["", "## Validation Commands"])
    for command in summary["validation_commands"]:
        lines.append(f"- `{command}`")
    lines.append("")
    return "\n".join(lines)
