#!/usr/bin/env python3
"""Fail-closed validator for M226-A011 parser performance/quality guardrails."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-performance-quality-guardrails-contract-a011-v1"

PARSER_PATH = ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
CONTRACT_PATH = ROOT / "docs" / "contracts" / "m226_parser_performance_quality_guardrails_expectations.md"

MAX_FUNCTION_LINES: dict[str, int] = {
    "ParseCStyleCompatType": 180,
    "ParseCStyleCompatFunctionParameters": 90,
    "ParseTopLevelCompatFunctionDecl": 220,
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/reports/m226/M226-A011/parser_performance_quality_guardrails_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def function_line_spans(source: str, function_name: str) -> tuple[int, int] | None:
    specific = re.compile(rf"^\s*(?:bool|void)\s+{re.escape(function_name)}\s*\(", re.MULTILINE)
    match = specific.search(source)
    if not match:
        return None

    start_idx = match.start()
    start_line = source.count("\n", 0, start_idx) + 1

    any_fn = re.compile(r"^\s*(?:bool|void)\s+[A-Za-z_]\w*\s*\(", re.MULTILINE)
    next_match = any_fn.search(source, match.end())
    if next_match is None:
        end_line = source.count("\n") + 1
    else:
        end_line = source.count("\n", 0, next_match.start())
    return start_line, end_line


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    parser_source = load_text(PARSER_PATH, artifact="parser_source")
    contract_doc = load_text(CONTRACT_PATH, artifact="contract_doc")

    total_checks += 1
    if "Contract ID: `objc3c-parser-performance-quality-guardrails-contract/m226-a011-v1`" in contract_doc:
        passed_checks += 1
    else:
        findings.append(
            Finding(
                "contract_doc",
                "M226-A011-DOC-01",
                "missing expected contract id anchor",
            )
        )

    for function_name, max_lines in MAX_FUNCTION_LINES.items():
        total_checks += 1
        span = function_line_spans(parser_source, function_name)
        if span is None:
            findings.append(
                Finding(
                    "parser_source",
                    f"M226-A011-FUNC-{function_name}",
                    f"function not found: {function_name}",
                )
            )
            continue
        line_count = span[1] - span[0] + 1
        if line_count > max_lines:
            findings.append(
                Finding(
                    "parser_source",
                    f"M226-A011-LIMIT-{function_name}",
                    f"{function_name} spans {line_count} lines (max {max_lines})",
                )
            )
            continue
        passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
