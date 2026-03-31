#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/formatter_debug_implementation_contract.json"
OUT_DIR = ROOT / "tmp" / "reports" / "developer-tooling" / "formatter-debug"
JSON_OUT = OUT_DIR / "formatter_debug_summary.json"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {path}")
    return payload


def extract_line_value(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    format_result = subprocess.run(
        [sys.executable, str(PUBLIC_RUNNER), "format-objc3c", contract["format_source"]],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    debug_result = subprocess.run(
        [sys.executable, str(PUBLIC_RUNNER), "inspect-editor-tooling", contract["debug_source"]],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if format_result.stdout:
        sys.stdout.write(format_result.stdout)
    if format_result.stderr:
        sys.stderr.write(format_result.stderr)
    if debug_result.stdout:
        sys.stdout.write(debug_result.stdout)
    if debug_result.stderr:
        sys.stderr.write(debug_result.stderr)

    failures: list[str] = []
    expect(format_result.returncode == 0, "formatter action failed", failures)
    expect(debug_result.returncode == 0, "debug editor surface action failed", failures)

    format_summary_path_text = extract_line_value(format_result.stdout, "summary_path:")
    debug_dump_path_text = extract_line_value(debug_result.stdout, "dump_path:")
    expect(bool(format_summary_path_text), "formatter did not publish summary_path", failures)
    expect(bool(debug_dump_path_text), "editor tooling surface did not publish dump_path", failures)

    format_summary = load_json(ROOT / format_summary_path_text) if format_summary_path_text else {}
    debug_surface = load_json(ROOT / debug_dump_path_text) if debug_dump_path_text else {}
    expected_formatted = (ROOT / contract["expected_formatted_source"]).read_text(encoding="utf-8")
    formatted_output_path = format_summary.get("formatted_output_path")
    formatted_text = (ROOT / formatted_output_path).read_text(encoding="utf-8") if isinstance(formatted_output_path, str) else ""
    debug_payload = debug_surface.get("debug", {})

    expect(format_summary.get("supported") is True, "formatter did not report supported=true", failures)
    expect(format_summary.get("support_class") == "preview", "formatter support_class must stay preview", failures)
    expect(formatted_text == expected_formatted, "formatter output drifted from expected canonical source", failures)
    for field in contract["required_debug_fields"]:
        expect(field in debug_payload, f"debug payload missing required field: {field}", failures)
    expect(debug_payload.get("supported") is True, "debug payload did not report supported=true", failures)
    expect(debug_payload.get("support_class") == "declaration-breakpoint-preview", "debug payload must publish declaration-breakpoint-preview support class", failures)
    expect(debug_payload.get("statement_level_stepping") is False, "debug payload must fail closed for statement-level stepping", failures)
    expect(debug_payload.get("source_map_supported") is False, "debug payload must fail closed for source-map support", failures)
    expect(int(debug_payload.get("declaration_breakpoint_anchor_count", 0)) >= 3, "debug payload did not publish expected declaration breakpoint anchors", failures)
    expect(bool(debug_payload.get("object_symbol_inventory_command")), "debug payload did not publish object symbol inventory command", failures)

    summary = {
        "contract_id": contract["contract_id"],
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "format_summary_path": format_summary_path_text,
        "debug_dump_path": debug_dump_path_text,
        "failures": failures,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {JSON_OUT.relative_to(ROOT).as_posix()}")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
