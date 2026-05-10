"""Rendering helpers for public claim drift reports."""

from __future__ import annotations

from typing import Any, Mapping

from objc3c_tooling.reports import markdown_table


def render_markdown(summary: Mapping[str, Any]) -> str:
    findings = summary["findings"]
    if findings:
        finding_rows = markdown_table(
            ["Kind", "Location", "Pattern"],
            [
                [
                    finding["kind"],
                    f"{finding['path']}:{finding['line']}",
                    finding.get("pattern_id", "n/a"),
                ]
                for finding in findings
            ],
        )
    else:
        finding_rows = markdown_table(["Kind", "Location", "Pattern"], [["none", "n/a", "n/a"]])
    finding_table = "\n".join(finding_rows)
    return (
        "# Objective-C 3.0 Public Claim Drift Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Support summary: `{summary['support_summary_path']}`\n"
        f"- Scanned surfaces: `{summary['scanned_surface_count']}`\n"
        f"- Public claim surfaces: `{summary['public_claim_surface_count']}`\n"
        f"- Mapped claim lines: `{summary['claim_mapping_count']}`\n"
        f"- Findings: `{summary['finding_count']}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n\n"
        "## Findings\n\n"
        f"{finding_table}\n"
    )


def status_line(summary: Mapping[str, Any], report_path: str) -> str:
    return (
        "public claim drift: {status} (mapped={mapped}, findings={findings}, report={report})"
    ).format(
        status=summary["status"],
        mapped=summary["claim_mapping_count"],
        findings=summary["finding_count"],
        report=report_path,
    )


__all__ = ["render_markdown", "status_line"]
