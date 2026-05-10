"""Markdown rendering for support classification reports."""

from __future__ import annotations

from typing import Any

from objc3c_tooling.reports import markdown_table


def render_support_classification_markdown(summary: dict[str, Any]) -> str:
    rows = markdown_table(
        ["Surface", "Class", "Claim Class", "Required Evidence"],
        [
            [
                row["surface"],
                row["current_class"],
                row["claim_class"],
                ", ".join(row["required_evidence_families"]) or "none",
            ]
            for row in summary["classifications"]
        ],
    )
    classification_table = "\n".join(rows)
    return (
        "# Objective-C 3.0 Support Classification Summary\n\n"
        f"- Contract: `{summary['source_contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Support classes: `{summary['support_class_count']}`\n"
        f"- Evidence families: `{summary['evidence_family_count']}`\n"
        f"- Classified surfaces: `{summary['classification_count']}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n\n"
        "## Class Counts\n\n"
        + "\n".join(
            f"- `{key}`: `{value}`" for key, value in summary["class_counts"].items()
        )
        + "\n\n"
        "## Surface Classifications\n\n"
        f"{classification_table}\n"
    )


__all__ = ["render_support_classification_markdown"]
