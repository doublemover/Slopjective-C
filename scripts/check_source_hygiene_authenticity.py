#!/usr/bin/env python3
"""Run the live source-hygiene and authenticity audit contract."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.subprocesses import command_text, python_script_command
from objc3c_tooling.subprocesses import run_completed as run_command

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "source_hygiene" / "source_hygiene_enforcement_contract.json"
OUTPUT_DIR = ROOT / "tmp" / "reports" / "source_hygiene"
SUMMARY_PATH = OUTPUT_DIR / "source_hygiene_audit_summary.json"
PREREQUISITE_SCRIPTS = (
    "scripts/build_residue_authenticity_inventory.py",
)


def normalize(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def run_script(script: str, *args: object) -> dict[str, Any]:
    command = python_script_command(script, *args)
    result = run_command(command, cwd=ROOT)
    return {
        "command": command_text(command),
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
    }


def implementation_command(entry: dict[str, Any]) -> list[str]:
    implementation_args = entry.get("implementation_args", [])
    if not isinstance(implementation_args, list):
        implementation_args = []
    return python_script_command(entry["implementation_anchor"], *implementation_args)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    prerequisite_results = [run_script(script) for script in PREREQUISITE_SCRIPTS]
    check_results: list[dict[str, Any]] = []

    for entry in contract["enforcement_checks"]:
        command = implementation_command(entry)
        result = run_command(command, cwd=ROOT)
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
                "entrypoint": entry["entrypoint"],
                "implementation_anchor": entry["implementation_anchor"],
                "implementation_command": command_text(command),
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
        "live_audit_entrypoint": contract["live_audit_entrypoint"],
        "owner_surfaces": contract["owner_surfaces"],
        "blocker_metadata": contract["blocker_metadata"],
        "generated_report_root": contract["generated_report_root"],
        "prerequisites": prerequisite_results,
        "checks": check_results,
        "ok": all(item["returncode"] == 0 for item in prerequisite_results)
        and all(item["returncode"] == 0 and item["report_ok"] for item in check_results),
    }

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {normalize(SUMMARY_PATH)}")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
