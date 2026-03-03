#!/usr/bin/env python3
"""Fail-closed validator for M243-A007 diagnostic grammar-hooks diagnostics hardening."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening-contract-a007-v1"

ARTIFACTS: dict[str, Path] = {
    "a006_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a006_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a007_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_packet.md",
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
    "architecture_doc": ROOT / "native" / "objc3c" / "src" / "ARCHITECTURE.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "a006_contract_doc": (
        (
            "M243-A007-DEP-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness/m243-a006-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M243-A007-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening/m243-a007-v1`",
        ),
        (
            "M243-A007-DOC-02",
            "parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent",
        ),
        (
            "M243-A007-DOC-03",
            "parser_diagnostic_grammar_hooks_diagnostics_hardening_ready",
        ),
        (
            "M243-A007-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-A007-DOC-05",
            "npm run check:objc3c:m243-a007-lane-a-readiness",
        ),
    ),
    "packet_doc": (
        ("M243-A007-PKT-01", "Packet: `M243-A007`"),
        ("M243-A007-PKT-02", "Dependencies: `M243-A006`"),
        (
            "M243-A007-PKT-03",
            "scripts/check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-A007-TYP-01",
            "bool parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent = false;",
        ),
        (
            "M243-A007-TYP-02",
            "bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = false;",
        ),
        (
            "M243-A007-TYP-03",
            "std::string parser_diagnostic_grammar_hooks_diagnostics_hardening_key;",
        ),
    ),
    "readiness_surface": (
        (
            "M243-A007-RDY-01",
            "BuildObjc3DiagnosticGrammarHooksDiagnosticsHardeningKey(",
        ),
        (
            "M243-A007-RDY-02",
            "surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent =",
        ),
        (
            "M243-A007-RDY-03",
            "surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready =",
        ),
        (
            "M243-A007-RDY-04",
            "surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key =",
        ),
        (
            "M243-A007-RDY-05",
            "parser diagnostic grammar hooks diagnostics hardening is inconsistent",
        ),
        (
            "M243-A007-RDY-06",
            "parser diagnostic grammar hooks diagnostics hardening is not ready",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A007-ARC-01",
            "M243 lane-A A007 diagnostics hardening anchors parser diagnostic grammar-hook",
        ),
    ),
    "package_json": (
        (
            "M243-A007-CFG-01",
            '"check:objc3c:m243-a007-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening-contract"',
        ),
        (
            "M243-A007-CFG-02",
            '"test:tooling:m243-a007-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening-contract"',
        ),
        (
            "M243-A007-CFG-03",
            '"check:objc3c:m243-a007-lane-a-readiness"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface": (
        (
            "M243-A007-FORB-01",
            "surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = true;",
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
            "tmp/reports/m243/M243-A007/diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract_summary.json"
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
        for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):  # pragma: no branch
            total_checks += 1
            if snippet in text:
                passed_checks += 1
            else:
                findings.append(Finding(artifact, check_id, f"expected snippet missing: {snippet}"))
        for check_id, snippet in FORBIDDEN_SNIPPETS.get(artifact, ()):  # pragma: no branch
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
            {"artifact": f.artifact, "check_id": f.check_id, "detail": f.detail}
            for f in findings
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
