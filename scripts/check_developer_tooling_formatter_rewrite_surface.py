#!/usr/bin/env python3
"""Validate formatter and safe source rewrite developer-tooling surfaces."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from subprocess import CompletedProcess
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.public_workflow_output import extract_line_value
from objc3c_tooling.subprocesses import run_capture
from scripts.format_objc3c_source import (
    REPORT_ROOT as FORMATTER_REPORT_ROOT,
    resolve_source as resolve_formatter_source,
    slugify as formatter_slugify,
)
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from scripts.rewrite_objc3c_source import (
    REPORT_ROOT as REWRITE_REPORT_ROOT,
    resolve_source as resolve_rewrite_source,
    slugify as rewrite_slugify,
)


CONTRACT_PATH = ROOT / "tests/tooling/fixtures/developer_tooling/formatter_rewrite_contract.json"
OUT_DIR = ROOT / "tmp/reports/developer-tooling/formatter-rewrite"
JSON_OUT = OUT_DIR / "formatter_rewrite_summary.json"


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def run_public_action(*args: str) -> CompletedProcess[str]:
    result = run_capture(
        public_workflow_command(*args),
        cwd=ROOT,
        capture_output=True,
        echo=False,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_optional_payload(path_text: str | None) -> dict[str, Any]:
    return load_json(ROOT / path_text) if path_text else {}


def read_text(path_text: str | None) -> str:
    return (ROOT / path_text).read_text(encoding="utf-8") if path_text else ""


def formatter_summary_path(source_text: str) -> Path:
    _, source_display = resolve_formatter_source(source_text)
    return FORMATTER_REPORT_ROOT / formatter_slugify(source_display) / "formatter-output.json"


def rewrite_summary_path(source_text: str) -> Path:
    _, source_display = resolve_rewrite_source(source_text)
    return REWRITE_REPORT_ROOT / rewrite_slugify(source_display) / "source-rewrite-summary.json"


def captured_or_materialized_path(captured_text: str, materialized_path: Path) -> str:
    if captured_text:
        return captured_text
    if materialized_path.is_file():
        return materialized_path.relative_to(ROOT).as_posix()
    return ""


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    failures: list[str] = []

    positive_format = run_public_action("format-objc3c", contract["format_source"])
    negative_format = run_public_action("format-objc3c", contract["negative_format_source"])
    rewrite = run_public_action(
        "rewrite-objc3c-source",
        contract["rewrite_source"],
        "--rule",
        "legacy-literal-aliases",
        "--rename-symbol",
        "legacyValue=canonicalValue",
    )

    expect(positive_format.returncode == 0, "positive formatter action failed", failures)
    expect(negative_format.returncode != 0, "negative formatter fixture must fail closed", failures)
    expect(rewrite.returncode == 0, "source rewrite action failed", failures)

    positive_summary_path = captured_or_materialized_path(
        extract_line_value(positive_format.stdout, "summary_path:"),
        formatter_summary_path(str(contract["format_source"])),
    )
    negative_summary_path = captured_or_materialized_path(
        extract_line_value(negative_format.stdout, "summary_path:"),
        formatter_summary_path(str(contract["negative_format_source"])),
    )
    rewrite_summary_path_text = captured_or_materialized_path(
        extract_line_value(rewrite.stdout, "summary_path:"),
        rewrite_summary_path(str(contract["rewrite_source"])),
    )
    expect(bool(positive_summary_path), "positive formatter did not publish summary_path", failures)
    expect(bool(negative_summary_path), "negative formatter did not publish summary_path", failures)
    expect(bool(rewrite_summary_path_text), "source rewrite did not publish summary_path", failures)

    positive_summary = load_optional_payload(positive_summary_path)
    negative_summary = load_optional_payload(negative_summary_path)
    rewrite_summary = load_optional_payload(rewrite_summary_path_text)
    expected_formatted = read_text(contract["expected_formatted_source"])
    formatted_text = read_text(str(positive_summary.get("formatted_output_path", "")))
    expected_rewritten = read_text(contract["expected_rewritten_source"])
    rewritten_text = read_text(str(rewrite_summary.get("rewritten_output_path", "")))

    expect(positive_summary.get("supported") is True, "positive formatter did not report supported=true", failures)
    expect(
        positive_summary.get("support_class") == "canonical-objc3-source-formatting",
        "formatter support class must publish canonical Objective-C 3 source coverage",
        failures,
    )
    expect(formatted_text == expected_formatted, "formatter output drifted from expected Objective-C 3 source", failures)
    for feature_id in contract["required_formatter_feature_ids"]:
        expect(feature_id in positive_summary.get("feature_ids", []), f"formatter missing feature id: {feature_id}", failures)

    expect(negative_summary.get("supported") is False, "negative formatter did not fail closed", failures)
    expect(negative_summary.get("diagnostics"), "negative formatter did not publish structured diagnostics", failures)

    expect(rewrite_summary.get("supported") is True, "rewrite summary did not report supported=true", failures)
    expect(rewrite_summary.get("support_class") == "deterministic-safe-rewrite", "rewrite support class drifted", failures)
    expect(rewritten_text == expected_rewritten, "rewritten output drifted from expected source", failures)
    expect(rewrite_summary.get("edit_count") == contract["expected_rewrite_edit_count"], "rewrite edit count drifted", failures)
    for rule_id in contract["required_rewrite_rule_ids"]:
        expect(rule_id in rewrite_summary.get("active_rule_ids", []), f"rewrite missing active rule: {rule_id}", failures)

    summary = {
        "contract_id": contract["contract_id"],
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "format_summary_path": positive_summary_path,
        "negative_format_summary_path": negative_summary_path,
        "rewrite_summary_path": rewrite_summary_path_text,
        "failures": failures,
    }
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, summary)
    print(f"summary_path: {JSON_OUT.relative_to(ROOT).as_posix()}")
    print(json.dumps(summary, indent=2))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
