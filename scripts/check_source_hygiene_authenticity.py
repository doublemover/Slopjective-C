#!/usr/bin/env python3
"""Run the live source-hygiene and authenticity audit contract."""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "source_hygiene" / "source_hygiene_enforcement_contract.json"
OUTPUT_DIR = ROOT / "tmp" / "reports" / "source_hygiene"
SUMMARY_PATH = OUTPUT_DIR / "source_hygiene_audit_summary.json"


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


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
    contract = read_json(CONTRACT_PATH)
    check_results: list[dict[str, Any]] = []

    for entry in contract["enforcement_checks"]:
        command_text = entry["entrypoint"]
        command = command_text.split()
        result = run_command(command)
        report_path = ROOT / entry["expected_report"]
        report_payload: dict[str, Any] | None = None
        report_ok = False
        if report_path.is_file():
            try:
                report_payload = read_json(report_path)
            except json.JSONDecodeError:
                report_payload = None
            if isinstance(report_payload, dict):
                report_ok = bool(report_payload.get(entry["required_ok_field"]))
        check_results.append(
            {
                "check_id": entry["check_id"],
                "entrypoint": command_text,
                "expected_report": entry["expected_report"],
                "returncode": result.returncode,
                "stdout": result.stdout.strip(),
                "stderr": result.stderr.strip(),
                "report_present": report_path.is_file(),
                "report_ok": report_ok,
                "failure_condition": entry["failure_condition"],
            }
        )

    summary = {
        "contract_id": contract["contract_id"],
        "future_live_audit_entrypoint": contract["future_live_audit_entrypoint"],
        "canonical_report_root": contract["canonical_report_root"],
        "checks": check_results,
        "ok": all(item["returncode"] == 0 and item["report_ok"] for item in check_results),
    }

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {normalize(SUMMARY_PATH)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
