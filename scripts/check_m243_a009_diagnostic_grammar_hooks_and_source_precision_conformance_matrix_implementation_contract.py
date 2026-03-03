#!/usr/bin/env python3
"""Fail-closed checker for M243-A009 diagnostic grammar-hooks conformance matrix implementation."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = (
    "m243-a009-diagnostic-grammar-hooks-and-source-precision-"
    "conformance-matrix-implementation-contract-v1"
)

ARTIFACTS: dict[str, Path] = {
    "a008_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a008_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a009_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_packet.md",
    "frontend_types": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_types.h",
    "readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "frontend_artifacts": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_frontend_artifacts.cpp",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a008_contract_doc": (
        (
            "M243-A009-DEP-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-recovery-and-determinism-hardening/m243-a008-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M243-A009-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation/m243-a009-v1`",
        ),
        (
            "M243-A009-DOC-02",
            "parser_diagnostic_grammar_hooks_conformance_matrix_consistent",
        ),
        (
            "M243-A009-DOC-03",
            "parser_diagnostic_grammar_hooks_conformance_matrix_ready",
        ),
        (
            "M243-A009-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-A009-DOC-05",
            "npm run check:objc3c:m243-a009-lane-a-readiness",
        ),
        (
            "M243-A009-DOC-06",
            "Dependencies: `M243-A008`",
        ),
    ),
    "packet_doc": (
        ("M243-A009-PKT-01", "Packet: `M243-A009`"),
        ("M243-A009-PKT-02", "Dependencies: `M243-A008`"),
        (
            "M243-A009-PKT-03",
            "scripts/check_m243_a009_diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-A009-TYP-01",
            "bool parser_diagnostic_grammar_hooks_conformance_matrix_consistent = false;",
        ),
        (
            "M243-A009-TYP-02",
            "bool parser_diagnostic_grammar_hooks_conformance_matrix_ready = false;",
        ),
        (
            "M243-A009-TYP-03",
            "std::string parser_diagnostic_grammar_hooks_conformance_matrix_key;",
        ),
    ),
    "readiness_surface": (
        (
            "M243-A009-RDY-01",
            "BuildObjc3DiagnosticGrammarHooksConformanceMatrixKey(",
        ),
        (
            "M243-A009-RDY-02",
            "surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent =",
        ),
        (
            "M243-A009-RDY-03",
            "surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready =",
        ),
        (
            "M243-A009-RDY-04",
            "surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =",
        ),
        (
            "M243-A009-RDY-05",
            "surface.failure_reason =\n        \"parser diagnostic grammar hooks conformance matrix is inconsistent\";",
        ),
        (
            "M243-A009-RDY-06",
            "surface.failure_reason =\n        \"parser diagnostic grammar hooks conformance matrix is not ready\";",
        ),
        (
            "M243-A009-RDY-07",
            "surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&",
        ),
        (
            "M243-A009-RDY-08",
            "!surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty()",
        ),
    ),
    "frontend_artifacts": (
        (
            "M243-A009-ART-01",
            "parser_diagnostic_grammar_hooks_conformance_matrix_consistent",
        ),
        (
            "M243-A009-ART-02",
            "parser_diagnostic_grammar_hooks_conformance_matrix_ready",
        ),
        (
            "M243-A009-ART-03",
            "parser_diagnostic_grammar_hooks_conformance_matrix_key",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A009-ARC-01",
            "M243 lane-A A009 conformance matrix implementation anchors parser diagnostic",
        ),
    ),
    "package_json": (
        (
            "M243-A009-CFG-01",
            '"check:objc3c:m243-a009-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation-contract"',
        ),
        (
            "M243-A009-CFG-02",
            '"test:tooling:m243-a009-diagnostic-grammar-hooks-and-source-precision-conformance-matrix-implementation-contract"',
        ),
        (
            "M243-A009-CFG-03",
            '"check:objc3c:m243-a009-lane-a-readiness"',
        ),
        (
            "M243-A009-CFG-04",
            "npm run check:objc3c:m243-a008-lane-a-readiness",
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface": (
        (
            "M243-A009-FORB-01",
            "surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready = true;",
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
        default=Path(
            "tmp/reports/m243/M243-A009/"
            "diagnostic_grammar_hooks_and_source_precision_conformance_matrix_implementation_contract_summary.json"
        ),
    )
    parser.add_argument(
        "--emit-json",
        action="store_true",
        help="Emit the summary JSON to stdout in addition to writing --summary-out.",
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
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):
            total_checks += 1
            if snippet in text:
                findings.append(Finding(artifact, check_id, f"forbidden snippet present: {snippet}"))
            else:
                passed_checks += 1

    ok = not findings
    summary = {
        "mode": MODE,
        "ok": ok,
        "checks_total": total_checks,
        "checks_passed": passed_checks,
        "failures": [
            {"artifact": finding.artifact, "check_id": finding.check_id, "detail": finding.detail}
            for finding in findings
        ],
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if args.emit_json:
        print(canonical_json(summary), end="")

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))

