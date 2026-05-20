#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
from pathlib import Path
from subprocess import CompletedProcess
from typing import Any
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.public_workflow_output import extract_line_value
from objc3c_tooling.subprocesses import run_capture
from objc3c_editor_tooling.paths import paths_for_source, resolve_source


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/language-server-navigation"
JSON_OUT = OUT_DIR / "language_server_navigation_summary.json"





def run_action(source_path: str) -> CompletedProcess[str]:
    return run_capture(
        public_workflow_command("inspect-editor-tooling", source_path),
        cwd=ROOT,
        capture_output=True,
        echo=False,
    )


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def editor_dump_path(source_text: str) -> Path:
    return paths_for_source(resolve_source(source_text)).editor_surface


def captured_or_materialized_path(captured_text: str, materialized_path: Path) -> str:
    if captured_text:
        return captured_text
    if materialized_path.is_file():
        return repo_rel(materialized_path)
    return ""


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

    positive_dump = captured_or_materialized_path(
        extract_line_value(positive.stdout, "dump_path:"),
        editor_dump_path(str(contract["positive_source"])),
    )
    negative_dump = captured_or_materialized_path(
        extract_line_value(negative.stdout, "dump_path:"),
        editor_dump_path(str(contract["negative_source"])),
    )
    expect(bool(positive_dump), "positive inspection did not publish dump_path", failures)
    expect(bool(negative_dump), "negative inspection did not publish dump_path", failures)

    positive_payload = load_json(ROOT / positive_dump) if positive_dump else {}
    negative_payload = load_json(ROOT / negative_dump) if negative_dump else {}

    positive_supported = set(positive_payload.get("language_server", {}).get("supported_capability_ids", []))
    positive_unpublished = set(positive_payload.get("language_server", {}).get("unpublished_capability_ids", []))
    positive_symbols = [symbol.get("name") for symbol in positive_payload.get("navigation", {}).get("symbols", [])]
    positive_document_symbols = [
        symbol.get("name")
        for symbol in positive_payload.get("navigation", {}).get("document_symbols", [])
    ]
    positive_definition_targets = [
        target.get("name")
        for target in positive_payload.get("navigation", {}).get("definition_targets", [])
    ]
    positive_workspace_index = positive_payload.get("navigation", {}).get("workspace_index", {})
    workspace_packages = positive_workspace_index.get("packages", []) if isinstance(positive_workspace_index, dict) else []
    workspace_package_ids = {
        package.get("package_id")
        for package in workspace_packages
        if isinstance(package, dict)
    }
    workspace_guardrails = positive_workspace_index.get("guardrails", {}) if isinstance(positive_workspace_index, dict) else {}
    workspace_guardrail_checks = workspace_guardrails.get("checks", {}) if isinstance(workspace_guardrails, dict) else {}
    negative_codes = [entry.get("code") for entry in negative_payload.get("diagnostics", {}).get("entries", [])]
    negative_entries = negative_payload.get("diagnostics", {}).get("entries", [])

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
        all(symbol_name in positive_document_symbols for symbol_name in contract["expected_positive_symbol_names"]),
        "positive navigation surface is missing expected document symbols",
        failures,
    )
    expect(
        all(symbol_name in positive_definition_targets for symbol_name in contract["expected_positive_symbol_names"]),
        "positive navigation surface is missing expected definition targets",
        failures,
    )
    expect(
        positive_workspace_index.get("available") is True,
        "positive navigation surface did not publish an available workspace index",
        failures,
    )
    expect(
        all(package_id in workspace_package_ids for package_id in contract["expected_workspace_package_ids"]),
        "workspace index is missing expected package ids",
        failures,
    )
    expect(
        all(workspace_guardrail_checks.get(check) is True for check in contract["expected_workspace_guardrail_checks"]),
        "workspace index package guardrails did not all pass",
        failures,
    )
    expect(
        len(str(positive_workspace_index.get("workspace_index_digest", ""))) == 64,
        "workspace index did not publish a stable SHA-256 digest",
        failures,
    )
    expect(
        all(code in negative_codes for code in contract["expected_negative_diagnostic_codes"]),
        "negative editor surface is missing expected diagnostic codes",
        failures,
    )
    for expected in contract["expected_negative_diagnostic_ranges"]:
        matches = [
            entry
            for entry in negative_entries
            if entry.get("code") == expected["code"]
            and entry.get("line") == expected["line"]
            and entry.get("column") == expected["column"]
        ]
        expect(
            bool(matches),
            f"negative diagnostics missing stable range for {expected['code']}",
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
