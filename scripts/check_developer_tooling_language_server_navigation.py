#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import load_json_object as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.public_workflow_output import extract_line_value


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/language-server-navigation"
JSON_OUT = OUT_DIR / "language_server_navigation_summary.json"





def run_action(source_path: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        public_workflow_command("inspect-editor-tooling", source_path),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    positive = run_action(contract["positive_source"])
    negative = run_action(contract["negative_source"])
    if positive.stdout:
        sys.stdout.write(positive.stdout)
    if positive.stderr:
        sys.stderr.write(positive.stderr)
    if negative.stdout:
        sys.stdout.write(negative.stdout)
    if negative.stderr:
        sys.stderr.write(negative.stderr)

    failures: list[str] = []
    expect(positive.returncode == 0, "positive editor tooling inspection failed", failures)
    expect(negative.returncode == 0, "negative editor tooling inspection failed", failures)

    positive_dump = extract_line_value(positive.stdout, "dump_path:")
    negative_dump = extract_line_value(negative.stdout, "dump_path:")
    expect(bool(positive_dump), "positive inspection did not publish dump_path", failures)
    expect(bool(negative_dump), "negative inspection did not publish dump_path", failures)

    positive_payload = load_json(ROOT / positive_dump) if positive_dump else {}
    negative_payload = load_json(ROOT / negative_dump) if negative_dump else {}

    positive_supported = set(positive_payload.get("language_server", {}).get("supported_capability_ids", []))
    positive_unpublished = set(positive_payload.get("language_server", {}).get("unpublished_capability_ids", []))
    positive_symbols = [symbol.get("name") for symbol in positive_payload.get("navigation", {}).get("symbols", [])]
    negative_codes = [entry.get("code") for entry in negative_payload.get("diagnostics", {}).get("entries", [])]

    expect(
        all(capability in positive_supported for capability in contract["expected_supported_capabilities"]),
        "positive editor surface is missing expected supported capabilities",
        failures,
    )
    expect(
        all(capability in positive_unpublished for capability in contract["expected_unpublished_capabilities"]),
        "positive editor surface is missing expected unpublished capabilities",
        failures,
    )
    expect(
        all(symbol_name in positive_symbols for symbol_name in contract["expected_positive_symbol_names"]),
        "positive navigation surface is missing expected declaration symbols",
        failures,
    )
    expect(
        all(code in negative_codes for code in contract["expected_negative_diagnostic_codes"]),
        "negative editor surface is missing expected diagnostic codes",
        failures,
    )
    expect(
        positive_payload.get("navigation", {}).get("available") is True,
        "positive navigation surface did not report available=true",
        failures,
    )
    expect(
        negative_payload.get("diagnostics", {}).get("status_name") == "diagnostics",
        "negative diagnostics surface did not preserve diagnostics status",
        failures,
    )

    summary = {
        "contract_id": contract["contract_id"],
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "positive_dump_path": positive_dump,
        "negative_dump_path": negative_dump,
        "failures": failures,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {JSON_OUT.relative_to(ROOT).as_posix()}")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
