#!/usr/bin/env python3
"""Fail-closed validator for M226-B009 parser-sema conformance matrix hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m226-parser-sema-conformance-matrix-contract-b009-v1"

ARTIFACTS: dict[str, Path] = {
    "sema_handoff_scaffold": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_parser_sema_handoff_scaffold.h",
    "sema_pass_manager_contract": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "objc3_sema_pass_manager_contract.h",
    "sema_pass_manager": ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp",
    "contract_doc": ROOT / "docs" / "contracts" / "m226_parser_sema_conformance_matrix_b009_expectations.md",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "sema_handoff_scaffold": (
        (
            "M226-B009-SEM-01",
            "inline Objc3ParserSemaConformanceMatrix BuildObjc3ParserSemaConformanceMatrix(",
        ),
        ("M226-B009-SEM-02", "matrix.top_level_declaration_count_matches ="),
        ("M226-B009-SEM-03", "matrix.parser_subset_count_consistent ="),
        ("M226-B009-SEM-04", "matrix.parser_contract_snapshot_fingerprint_matches ="),
        ("M226-B009-SEM-05", "matrix.deterministic ="),
        (
            "M226-B009-SEM-06",
            "scaffold.parser_sema_conformance_matrix = BuildObjc3ParserSemaConformanceMatrix(",
        ),
    ),
    "sema_pass_manager_contract": (
        ("M226-B009-CON-01", "struct Objc3ParserSemaConformanceMatrix {"),
        (
            "M226-B009-CON-02",
            "Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;",
        ),
        (
            "M226-B009-CON-03",
            "bool deterministic_parser_sema_conformance_matrix = false;",
        ),
        (
            "M226-B009-CON-04",
            "surface.deterministic_parser_sema_conformance_matrix &&",
        ),
        (
            "M226-B009-CON-05",
            "surface.parser_sema_conformance_matrix.parser_subset_count_consistent &&",
        ),
    ),
    "sema_pass_manager": (
        (
            "M226-B009-PASS-01",
            "result.parser_sema_conformance_matrix = handoff.parser_sema_conformance_matrix;",
        ),
        (
            "M226-B009-PASS-02",
            "result.deterministic_parser_sema_conformance_matrix =",
        ),
        (
            "M226-B009-PASS-03",
            "if (!result.deterministic_parser_sema_conformance_matrix) {\n    return result;\n  }",
        ),
        (
            "M226-B009-PASS-04",
            "result.parity_surface.deterministic_parser_sema_conformance_matrix =",
        ),
        (
            "M226-B009-PASS-05",
            "result.parity_surface.parser_sema_conformance_matrix\n          .parser_contract_snapshot_fingerprint_matches &&",
        ),
    ),
    "contract_doc": (
        (
            "M226-B009-DOC-01",
            "Contract ID: `objc3c-parser-sema-conformance-matrix-contract/m226-b009-v1`",
        ),
        ("M226-B009-DOC-02", "`Objc3ParserSemaConformanceMatrix`"),
        (
            "M226-B009-DOC-03",
            "`if (!result.deterministic_parser_sema_conformance_matrix) { return result; }`",
        ),
        (
            "M226-B009-DOC-04",
            "`python scripts/check_m226_b009_parser_sema_conformance_matrix_contract.py`",
        ),
        (
            "M226-B009-DOC-05",
            "`python -m pytest tests/tooling/test_check_m226_b009_parser_sema_conformance_matrix_contract.py -q`",
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
        default=Path("tmp/reports/m226/M226-B009/parser_sema_conformance_matrix_summary.json"),
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
