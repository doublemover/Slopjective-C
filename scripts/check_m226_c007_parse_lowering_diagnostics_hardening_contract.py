#!/usr/bin/env python3
"""Fail-closed validator for M226-C007 parse/lowering diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-c007-parse-lowering-diagnostics-hardening-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "readiness_surface_header": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_types_header": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    "artifacts_source": ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parse_lowering_diagnostics_hardening_c007_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface_header": (
        ("M226-C007-RDY-01", "struct Objc3ParseLoweringDiagnosticCodeCoverage {"),
        ("M226-C007-RDY-02", "inline std::string TryExtractObjc3ParseLoweringDiagnosticCode("),
        ("M226-C007-RDY-03", "inline Objc3ParseLoweringDiagnosticCodeCoverage BuildObjc3ParseLoweringDiagnosticCodeCoverage("),
        ("M226-C007-RDY-04", "inline std::string BuildObjc3ParseArtifactDiagnosticsHardeningKey("),
        ("M226-C007-RDY-05", "surface.parser_diagnostic_surface_consistent ="),
        ("M226-C007-RDY-06", "surface.parser_diagnostic_code_count = parser_diagnostic_code_coverage.unique_code_count;"),
        ("M226-C007-RDY-07", "surface.parser_diagnostic_code_fingerprint = parser_diagnostic_code_coverage.unique_code_fingerprint;"),
        ("M226-C007-RDY-08", "surface.parser_diagnostic_code_surface_deterministic ="),
        ("M226-C007-RDY-09", "surface.parse_artifact_diagnostics_hardening_consistent ="),
        ("M226-C007-RDY-10", "surface.parse_artifact_diagnostics_hardening_key ="),
        ("M226-C007-RDY-11", "const bool parse_artifact_diagnostics_hardening_ready ="),
        ("M226-C007-RDY-12", "parse_artifact_diagnostics_hardening_ready &&"),
        ("M226-C007-RDY-13", "surface.failure_reason = \"parser diagnostics surface is inconsistent\";"),
        ("M226-C007-RDY-14", "surface.failure_reason = \"parser diagnostic code surface is not deterministic\";"),
        ("M226-C007-RDY-15", "surface.failure_reason = \"parse artifact diagnostics hardening is inconsistent\";"),
    ),
    "frontend_types_header": (
        ("M226-C007-TYP-01", "bool parser_diagnostic_surface_consistent = false;"),
        ("M226-C007-TYP-02", "bool parser_diagnostic_code_surface_deterministic = false;"),
        ("M226-C007-TYP-03", "bool parse_artifact_diagnostics_hardening_consistent = false;"),
        ("M226-C007-TYP-04", "std::size_t parser_diagnostic_code_count = 0;"),
        ("M226-C007-TYP-05", "std::uint64_t parser_diagnostic_code_fingerprint = 1469598103934665603ull;"),
        ("M226-C007-TYP-06", "std::string parse_artifact_diagnostics_hardening_key;"),
    ),
    "artifacts_source": (
        ("M226-C007-ART-01", '\\"parser_diagnostic_surface_consistent\\": '),
        ("M226-C007-ART-02", '\\"parser_diagnostic_code_surface_deterministic\\": '),
        ("M226-C007-ART-03", '\\"parse_artifact_diagnostics_hardening_consistent\\": '),
        ("M226-C007-ART-04", '\\"parser_diagnostic_code_count\\": '),
        ("M226-C007-ART-05", '\\"parser_diagnostic_code_fingerprint\\": '),
        ("M226-C007-ART-06", '\\"parse_artifact_diagnostics_hardening_key\\":\\"'),
    ),
    "contract_doc": (
        ("M226-C007-DOC-01", "Contract ID: `objc3c-parse-lowering-diagnostics-hardening-contract/m226-c007-v1`"),
        ("M226-C007-DOC-02", "parser_diagnostic_surface_consistent"),
        ("M226-C007-DOC-03", "parser_diagnostic_code_surface_deterministic"),
        ("M226-C007-DOC-04", "parse_artifact_diagnostics_hardening_consistent"),
        ("M226-C007-DOC-05", "parse_artifact_diagnostics_hardening_key"),
        ("M226-C007-DOC-06", "python scripts/check_m226_c007_parse_lowering_diagnostics_hardening_contract.py"),
        ("M226-C007-DOC-07", "python -m pytest tests/tooling/test_check_m226_c007_parse_lowering_diagnostics_hardening_contract.py -q"),
        ("M226-C007-DOC-08", "tmp/reports/m226/m226_c007_parse_lowering_diagnostics_hardening_contract_summary.json"),
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
        default=Path("tmp/reports/m226/m226_c007_parse_lowering_diagnostics_hardening_contract_summary.json"),
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

    summary = {
        "mode": MODE,
        "ok": not findings,
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

    if findings:
        for finding in findings:
            print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
