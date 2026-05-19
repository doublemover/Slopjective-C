from __future__ import annotations

from objc3c_performance_report.model import PerformanceReportModel


def render_markdown_report(model: PerformanceReportModel) -> str:
    markdown_lines = [
        "# Objective-C 3 Performance Report",
        "",
        f"- Release status: `{model.release_status}`",
        f"- Claim ready: `{str(model.claim_ready).lower()}`",
        f"- Blocking breaches: `{model.blocking_breach_count}`",
        f"- Warning or caution breaches: `{model.warning_breach_count}`",
        "",
        model.headline,
        "",
        "## Summary",
        "",
    ]
    markdown_lines.extend(f"- {line}" for line in model.summary_lines)
    markdown_lines.extend(["", "## Evidence", ""])
    markdown_lines.extend(f"- `{path}`" for path in model.evidence_paths)
    markdown_lines.extend(["", "## Policy Contracts", ""])
    markdown_lines.extend(
        f"- `{name}`: `{contract_id}`"
        for name, contract_id in sorted(model.publication_contracts.items())
    )
    markdown_lines.extend(["", "## Owner Split", ""])
    for owner_name, owner_paths in sorted(model.owner_split.items()):
        markdown_lines.append(f"- `{owner_name}`: {len(owner_paths)} checked-in paths")
    return "\n".join(markdown_lines) + "\n"
