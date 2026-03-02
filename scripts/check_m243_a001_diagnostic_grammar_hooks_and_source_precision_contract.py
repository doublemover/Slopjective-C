#!/usr/bin/env python3
"""Fail-closed validator for M243-A001 diagnostic grammar hooks/source precision freeze."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-diagnostic-grammar-hooks-and-source-precision-freeze-contract-a001-v1"

ARTIFACTS: dict[str, Path] = {
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a001_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a001_diagnostic_grammar_hooks_and_source_precision_contract_and_architecture_freeze_packet.md",
    "parse_support_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parse_support.cpp",
    "parser_contract": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h",
    "parser_source": ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp",
    "readiness_surface": ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_parse_lowering_readiness_surface.h",
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "contract_doc": (
        (
            "M243-A001-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-freeze/m243-a001-v1`",
        ),
        (
            "M243-A001-DOC-02",
            "scripts/check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py",
        ),
        (
            "M243-A001-DOC-03",
            "tests/tooling/test_check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py",
        ),
        (
            "M243-A001-DOC-04",
            "tmp/reports/m243/M243-A001/diagnostic_grammar_hooks_and_source_precision_contract_summary.json",
        ),
    ),
    "packet_doc": (
        ("M243-A001-PKT-01", "Packet: `M243-A001`"),
        ("M243-A001-PKT-02", "Dependencies: none"),
        (
            "M243-A001-PKT-03",
            "scripts/check_m243_a001_diagnostic_grammar_hooks_and_source_precision_contract.py",
        ),
    ),
    "parse_support_source": (
        ("M243-A001-SUP-01", "std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message)"),
        ("M243-A001-SUP-02", "out << \"error:\" << line << \":\" << column << \": \" << message << \" [\" << code << \"]\";"),
    ),
    "parser_contract": (
        ("M243-A001-PAR-01", "BuildObjc3ParsedProgramAstShapeFingerprint"),
        ("M243-A001-PAR-02", "static_cast<std::uint64_t>(global.line)"),
        ("M243-A001-PAR-03", "static_cast<std::uint64_t>(global.column)"),
        ("M243-A001-PAR-04", "if (lhs.line != rhs.line)"),
        ("M243-A001-PAR-05", "if (lhs.column != rhs.column)"),
        ("M243-A001-PAR-06", "BuildObjc3ParsedProgramTopLevelLayoutFingerprint"),
    ),
    "parser_source": (
        ("M243-A001-PRS-01", "diagnostics_.push_back(MakeDiag(token.line, token.column, \"O3P100\""),
        ("M243-A001-PRS-02", "diagnostics_.push_back(MakeDiag(diag_line, diag_column, \"O3P114\""),
    ),
    "readiness_surface": (
        ("M243-A001-REA-01", "surface.parser_diagnostic_surface_consistent ="),
        ("M243-A001-REA-02", "surface.parser_diagnostic_code_surface_deterministic ="),
        ("M243-A001-REA-03", "BuildObjc3ParseArtifactDiagnosticsHardeningKey("),
    ),
    "architecture_doc": (
        (
            "M243-A001-ARC-01",
            "M243 lane-A A001 diagnostic grammar hooks/source precision anchors explicit",
        ),
        ("M243-A001-ARC-02", "parse/objc3_parse_support.cpp"),
        ("M243-A001-ARC-03", "pipeline/objc3_parse_lowering_readiness_surface.h"),
    ),
    "package_json": (
        (
            "M243-A001-CFG-01",
            '"check:objc3c:m243-a001-diagnostic-grammar-hooks-and-source-precision-contract"',
        ),
        (
            "M243-A001-CFG-02",
            '"test:tooling:m243-a001-diagnostic-grammar-hooks-and-source-precision-contract"',
        ),
        ("M243-A001-CFG-03", '"check:objc3c:m243-a001-lane-a-readiness"'),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "parser_source": (
        ("M243-A001-FORB-01", "MakeDiag(0, 0,"),
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
            "tmp/reports/m243/M243-A001/diagnostic_grammar_hooks_and_source_precision_contract_summary.json"
        ),
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

    if ok:
        return 0
    for finding in findings:
        print(f"[{finding.check_id}] {finding.artifact}: {finding.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(run(sys.argv[1:]))
