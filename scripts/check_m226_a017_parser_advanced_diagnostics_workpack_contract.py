#!/usr/bin/env python3
"""Fail-closed validator for M226-A017 parser advanced diagnostics workpack."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-advanced-diagnostics-workpack-contract-a017-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_advanced_diagnostics_workpack_a017_expectations.md",
    "frontend_artifacts": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        ("M226-A017-DOC-01", "Contract ID: `objc3c-parser-advanced-diagnostics-workpack-contract/m226-a017-v1`"),
        ("M226-A017-DOC-02", "`python scripts/check_m226_a017_parser_advanced_diagnostics_workpack_contract.py`"),
        ("M226-A017-DOC-03", "`python -m pytest tests/tooling/test_check_m226_a017_parser_advanced_diagnostics_workpack_contract.py -q`"),
    ),
    "frontend_artifacts": (
        ("M226-A017-ART-01", "struct Objc3ParserDiagnosticCodeCoverage {"),
        ("M226-A017-ART-02", "std::string TryExtractDiagnosticCode("),
        ("M226-A017-ART-03", "std::uint64_t MixParserDiagnosticCodeFingerprint("),
        ("M226-A017-ART-04", "Objc3ParserDiagnosticCodeCoverage BuildObjc3ParserDiagnosticCodeCoverage("),
        ("M226-A017-ART-05", "\\\"diagnostic_code_count\\\":"),
        ("M226-A017-ART-06", "\\\"diagnostic_code_fingerprint\\\":"),
        ("M226-A017-ART-07", "\\\"diagnostic_code_surface_deterministic\\\":"),
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
        default=Path("tmp/reports/m226/M226-A017/parser_advanced_diagnostics_workpack_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {path.as_posix()}")
    return path.read_text(encoding="utf-8")


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    findings: list[Finding] = []
    total_checks = 0
    passed_checks = 0

    for artifact, path in ARTIFACTS.items():
        text = load_text(path, artifact=artifact)
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))

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
