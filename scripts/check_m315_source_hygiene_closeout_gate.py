#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
AUDIT_SCRIPT = ROOT / "scripts/check_source_hygiene_authenticity.py"
ARCHIVE_SCRIPT = ROOT / "scripts/build_archive_boundary_compatibility_summary.py"
AUDIT_SUMMARY_PATH = ROOT / "tmp/reports/source_hygiene/source_hygiene_audit_summary.json"
ARCHIVE_SUMMARY_PATH = ROOT / "tmp/reports/source-hygiene/archive-boundary-compatibility/archive_boundary_compatibility_summary.json"
OUT_DIR = ROOT / "tmp/reports/source-hygiene/closeout-gate"
JSON_OUT = OUT_DIR / "source_hygiene_closeout_gate.json"
MD_OUT = OUT_DIR / "source_hygiene_closeout_gate.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )


def main() -> int:
    audit_result = run_command([sys.executable, str(AUDIT_SCRIPT)])
    archive_result = run_command([sys.executable, str(ARCHIVE_SCRIPT)])
    audit = read_json(AUDIT_SUMMARY_PATH)
    archive = read_json(ARCHIVE_SUMMARY_PATH)

    checks = {
        "source_hygiene_audit_exit_zero": audit_result.returncode == 0,
        "source_hygiene_audit_ok": audit.get("ok") is True,
        "archive_boundary_exit_zero": archive_result.returncode == 0,
        "archive_boundary_ok": archive.get("ok") is True,
        "archive_boundary_contract_present": archive.get("contract_id") == "objc3c.source_hygiene.archive_boundary_contract.v1",
        "source_hygiene_contract_present": audit.get("contract_id") == "objc3c.source_hygiene.enforcement_contract.v1",
        "all_live_checks_report_ok": all(entry.get("report_ok") for entry in audit.get("checks", [])),
        "product_surface_cleanliness_preserved": next(
            (entry.get("report_ok") for entry in audit.get("checks", []) if entry.get("check_id") == "product_surface_residue_zero"),
            False,
        )
        is True,
        "archive_history_preserved": archive.get("historical_reference_hit_count", 0) > 0,
    }

    summary = {
        "issue": "source-hygiene-closeout-gate",
        "audit_summary_path": "tmp/reports/source_hygiene/source_hygiene_audit_summary.json",
        "archive_summary_path": "tmp/reports/source-hygiene/archive-boundary-compatibility/archive_boundary_compatibility_summary.json",
        "audit_check_ids": [entry["check_id"] for entry in audit.get("checks", [])],
        "archive_root_file_counts": archive.get("archive_root_file_counts", {}),
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Source Hygiene Closeout Gate\n\n"
        f"- Audit summary: `{summary['audit_summary_path']}`\n"
        f"- Archive summary: `{summary['archive_summary_path']}`\n"
        f"- Live audit checks: `{', '.join(summary['audit_check_ids'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
