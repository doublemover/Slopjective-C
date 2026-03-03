#!/usr/bin/env python3
"""Fail-closed validator for M243-A006 diagnostic grammar-hooks edge expansion/robustness."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m243-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness-contract-a006-v1"

ARTIFACTS: dict[str, Path] = {
    "a005_contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a005_expectations.md",
    "contract_doc": ROOT
    / "docs"
    / "contracts"
    / "m243_diagnostic_grammar_hooks_and_source_precision_a006_expectations.md",
    "packet_doc": ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m243"
    / "m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_packet.md",
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
    "a005_contract_doc": (
        (
            "M243-A006-DEP-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-edge-case-compatibility-completion/m243-a005-v1`",
        ),
    ),
    "contract_doc": (
        (
            "M243-A006-DOC-01",
            "Contract ID: `objc3c-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness/m243-a006-v1`",
        ),
        (
            "M243-A006-DOC-02",
            "parser_diagnostic_grammar_hooks_edge_case_expansion_consistent",
        ),
        (
            "M243-A006-DOC-03",
            "parser_diagnostic_grammar_hooks_edge_case_robustness_ready",
        ),
        (
            "M243-A006-DOC-04",
            "Code/spec anchors and milestone optimization improvements are mandatory scope inputs.",
        ),
        (
            "M243-A006-DOC-05",
            "npm run check:objc3c:m243-a006-lane-a-readiness",
        ),
    ),
    "packet_doc": (
        ("M243-A006-PKT-01", "Packet: `M243-A006`"),
        ("M243-A006-PKT-02", "Dependencies: `M243-A005`"),
        (
            "M243-A006-PKT-03",
            "scripts/check_m243_a006_diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract.py",
        ),
    ),
    "frontend_types": (
        (
            "M243-A006-TYP-01",
            "bool parser_diagnostic_grammar_hooks_edge_case_expansion_consistent = false;",
        ),
        (
            "M243-A006-TYP-02",
            "bool parser_diagnostic_grammar_hooks_edge_case_robustness_ready = false;",
        ),
        (
            "M243-A006-TYP-03",
            "std::string parser_diagnostic_grammar_hooks_edge_case_robustness_key;",
        ),
    ),
    "readiness_surface": (
        (
            "M243-A006-RDY-01",
            "BuildObjc3DiagnosticGrammarHooksEdgeCaseRobustnessKey(",
        ),
        (
            "M243-A006-RDY-02",
            "surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent =",
        ),
        (
            "M243-A006-RDY-03",
            "surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready =",
        ),
        (
            "M243-A006-RDY-04",
            "surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key =",
        ),
        (
            "M243-A006-RDY-05",
            "parser diagnostic grammar hooks edge-case expansion is inconsistent",
        ),
        (
            "M243-A006-RDY-06",
            "parser diagnostic grammar hooks edge-case robustness is not ready",
        ),
    ),
    "architecture_doc": (
        (
            "M243-A006-ARC-01",
            "M243 lane-A A006 edge-case expansion and robustness anchors parser diagnostic",
        ),
    ),
    "package_json": (
        (
            "M243-A006-CFG-01",
            '"check:objc3c:m243-a006-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M243-A006-CFG-02",
            '"test:tooling:m243-a006-diagnostic-grammar-hooks-and-source-precision-edge-case-expansion-and-robustness-contract"',
        ),
        (
            "M243-A006-CFG-03",
            '"check:objc3c:m243-a006-lane-a-readiness"',
        ),
    ),
}

FORBIDDEN_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "readiness_surface": (
        (
            "M243-A006-FORB-01",
            "surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready = true;",
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
            "tmp/reports/m243/M243-A006/diagnostic_grammar_hooks_and_source_precision_edge_case_expansion_and_robustness_contract_summary.json"
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
