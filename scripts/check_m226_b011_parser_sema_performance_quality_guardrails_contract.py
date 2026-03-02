#!/usr/bin/env python3
"""Fail-closed validator for M226-B011 parser-sema performance/quality guardrails."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-performance-quality-guardrails-contract-b011-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m226_parser_sema_performance_quality_guardrails_b011_expectations.md",
    "sema_handoff_scaffold": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_parser_sema_handoff_scaffold.h",
    "sema_pass_manager_contract": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager.cpp",
    "integration_test": ROOT
    / "tests"
    / "tooling"
    / "test_objc3c_parser_contract_sema_integration.py",
}

MAX_FUNCTION_LINES: dict[str, int] = {
    "BuildObjc3ParserSemaConformanceMatrix": 190,
    "BuildObjc3ParserSemaConformanceCorpus": 75,
    "BuildObjc3ParserSemaHandoffScaffold": 80,
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M226-B011-DOC-01",
            "Contract ID: `objc3c-parser-sema-performance-quality-guardrails-contract/m226-b011-v1`",
        ),
        (
            "M226-B011-DOC-02",
            "`python scripts/check_m226_b011_parser_sema_performance_quality_guardrails_contract.py`",
        ),
        (
            "M226-B011-DOC-03",
            "`python -m pytest tests/tooling/test_check_m226_b011_parser_sema_performance_quality_guardrails_contract.py -q`",
        ),
        (
            "M226-B011-DOC-04",
            "`python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`",
        ),
    ),
    "sema_handoff_scaffold": (
        (
            "M226-B011-SEM-01",
            "kObjc3ParserSemaConformanceMatrixBuilderMaxLines = 190u;",
        ),
        (
            "M226-B011-SEM-02",
            "inline Objc3ParserSemaPerformanceQualityGuardrails BuildObjc3ParserSemaPerformanceQualityGuardrails(",
        ),
        (
            "M226-B011-SEM-03",
            "guardrails.required_guardrail_count = 7u;",
        ),
        (
            "M226-B011-SEM-04",
            "scaffold.parser_sema_performance_quality_guardrails =",
        ),
        (
            "M226-B011-SEM-05",
            "scaffold.parser_sema_performance_quality_guardrails.deterministic &&",
        ),
    ),
    "sema_pass_manager_contract": (
        (
            "M226-B011-CON-01",
            "struct Objc3ParserSemaPerformanceQualityGuardrails {",
        ),
        (
            "M226-B011-CON-02",
            "Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;",
        ),
        (
            "M226-B011-CON-03",
            "bool deterministic_parser_sema_performance_quality_guardrails = false;",
        ),
        (
            "M226-B011-CON-04",
            "surface.deterministic_parser_sema_performance_quality_guardrails &&",
        ),
        (
            "M226-B011-CON-05",
            "surface.parser_sema_performance_quality_guardrails.required_guardrail_count == 7u &&",
        ),
    ),
    "sema_pass_manager": (
        (
            "M226-B011-PM-01",
            "result.parser_sema_performance_quality_guardrails =",
        ),
        (
            "M226-B011-PM-02",
            "result.deterministic_parser_sema_performance_quality_guardrails =",
        ),
        (
            "M226-B011-PM-03",
            "if (!result.deterministic_parser_sema_performance_quality_guardrails) {",
        ),
        (
            "M226-B011-PM-04",
            "result.parity_surface.parser_sema_performance_quality_guardrails =",
        ),
        (
            "M226-B011-PM-05",
            "result.parity_surface.deterministic_parser_sema_performance_quality_guardrails =",
        ),
    ),
    "integration_test": (
        (
            "M226-B011-TST-01",
            'assert "struct Objc3ParserSemaPerformanceQualityGuardrails {" in pass_manager_contract',
        ),
        (
            "M226-B011-TST-02",
            'assert "BuildObjc3ParserSemaPerformanceQualityGuardrails(" in sema_handoff',
        ),
        (
            "M226-B011-TST-03",
            'assert "result.deterministic_parser_sema_performance_quality_guardrails =" in sema_pass_manager',
        ),
    ),
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
        default=Path("tmp/reports/m226/M226-B011/parser_sema_performance_quality_guardrails_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def function_line_spans(source: str, function_name: str) -> tuple[int, int] | None:
    signature = re.compile(r"^\s*inline\s+[^\n;{]*\b([A-Za-z_]\w*)\s*\(", re.MULTILINE)
    matches = list(signature.finditer(source))
    for index, match in enumerate(matches):
        if match.group(1) != function_name:
            continue
        start_line = source.count("\n", 0, match.start()) + 1
        if index + 1 < len(matches):
            end_line = source.count("\n", 0, matches[index + 1].start())
        else:
            end_line = source.count("\n") + 1
        return start_line, end_line
    return None


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    artifacts_text: dict[str, str] = {}
    for artifact, path in ARTIFACTS.items():
        artifacts_text[artifact] = load_text(path, artifact=artifact)

    for artifact, checks in REQUIRED_SNIPPETS.items():
        text = artifacts_text[artifact]
        for check_id, snippet in checks:
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

    handoff_source = artifacts_text["sema_handoff_scaffold"]
    for function_name, max_lines in MAX_FUNCTION_LINES.items():
        total_checks += 1
        span = function_line_spans(handoff_source, function_name)
        if span is None:
            findings.append(
                Finding(
                    "sema_handoff_scaffold",
                    f"M226-B011-FUNC-{function_name}",
                    f"function not found: {function_name}",
                )
            )
            continue
        line_count = span[1] - span[0] + 1
        if line_count > max_lines:
            findings.append(
                Finding(
                    "sema_handoff_scaffold",
                    f"M226-B011-LIMIT-{function_name}",
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
