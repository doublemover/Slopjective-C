from __future__ import annotations


def render_markdown(summary: dict) -> str:
    counts = summary["counts"]
    lines = [
        "# Manifest/Object/IR Truth Gate",
        "",
        f"- Issue: `{summary['issue']}`",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Required artifacts: `{counts['required_artifact_count']}`",
        f"- Deterministic artifacts: `{counts['deterministic_artifact_count']}`",
        f"- Object sections checked: `{counts['required_object_section_count']}`",
        f"- Object symbols checked: `{counts['required_object_symbol_count']}`",
        f"- Scratch output: `{summary['scratch_directory']}` (not source truth)",
        "",
        "## Checks",
        "",
    ]
    for name, value in summary["checks"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Artifact Determinism", ""])
    for name, value in summary["deterministic_artifacts"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Object Evidence", ""])
    for section, value in summary["object_inspection"]["sections"].items():
        lines.append(f"- section `{section}`: `{str(value).lower()}`")
    for symbol, value in summary["object_inspection"]["symbols"].items():
        lines.append(f"- symbol `{symbol}`: `{str(value).lower()}`")
    lines.append("")
    return "\n".join(lines)
