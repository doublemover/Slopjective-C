from __future__ import annotations

import argparse
from pathlib import Path

from .config import (
    DEFAULT_JSON_REPORT,
    DEFAULT_TEXT_REPORT,
)
from .report_writer import write_reports
from .scanner import build_report

HARD_CUTOVER_GATE_CLI_CONTRACT_ID = "source-hygiene-hard-cutover-cli-v1"
HARD_CUTOVER_GATE_CLI_OWNER_SURFACE = "scripts/source_hygiene/hard_cutover_gate.py"
HARD_CUTOVER_GATE_SUMMARY_FIELDS: tuple[str, ...] = (
    "source_hygiene_cli_contract",
    "source_hygiene_json",
    "source_hygiene_text",
    "source_hygiene_ok",
    "active_findings",
    "tracked_generated_reports",
    "generated_truth_boundary_findings",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run the hard-cutover source hygiene gate.")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument("--json", type=Path, default=DEFAULT_JSON_REPORT)
    parser.add_argument("--text", type=Path, default=DEFAULT_TEXT_REPORT)
    return parser.parse_args()


def hard_cutover_gate_cli_contract_payload() -> dict[str, object]:
    return {
        "contract_id": HARD_CUTOVER_GATE_CLI_CONTRACT_ID,
        "owner_surface": HARD_CUTOVER_GATE_CLI_OWNER_SURFACE,
        "summary_fields": list(HARD_CUTOVER_GATE_SUMMARY_FIELDS),
        "json_report_default": DEFAULT_JSON_REPORT.as_posix(),
        "text_report_default": DEFAULT_TEXT_REPORT.as_posix(),
        "generated_boundary_findings_are_reported": True,
    }


def _repo_relative(path: Path, root: Path) -> str:
    return path.relative_to(root).as_posix()


def format_gate_summary(
    *,
    report: dict[str, object],
    json_path: Path,
    text_path: Path,
    root: Path,
) -> list[str]:
    stats = report["stats"]
    return [
        f"source_hygiene_cli_contract: {HARD_CUTOVER_GATE_CLI_CONTRACT_ID}",
        f"source_hygiene_json: {_repo_relative(json_path, root)}",
        f"source_hygiene_text: {_repo_relative(text_path, root)}",
        f"source_hygiene_ok: {str(report['ok']).lower()}",
        f"active_findings: {stats['active_finding_count']}",
        f"tracked_generated_reports: {stats['tracked_generated_report_count']}",
        "generated_truth_boundary_findings: "
        f"{stats['generated_truth_boundary_finding_count']}",
    ]


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    report = build_report(root=root)
    json_path = args.json if args.json.is_absolute() else root / args.json
    text_path = args.text if args.text.is_absolute() else root / args.text
    write_reports(report, json_path, text_path)
    for line in format_gate_summary(
        report=report,
        json_path=json_path,
        text_path=text_path,
        root=root,
    ):
        print(line)
    return 0 if report["ok"] else 1


__all__ = [
    "HARD_CUTOVER_GATE_CLI_CONTRACT_ID",
    "HARD_CUTOVER_GATE_CLI_OWNER_SURFACE",
    "HARD_CUTOVER_GATE_SUMMARY_FIELDS",
    "format_gate_summary",
    "hard_cutover_gate_cli_contract_payload",
    "main",
    "parse_args",
]
